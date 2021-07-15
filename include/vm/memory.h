#ifndef PLASMA_MEMORY_H
#define PLASMA_MEMORY_H

// #include <vector>
#include <stack>
#include <unordered_map>

namespace plasma::memory {
    template<typename T>
    struct memory {
        struct page {
            size_t allocatedElements = 0;
            size_t length = 0;
            T *content = nullptr;

            explicit page(size_t len) {
                this->length = len;
                this->content = new T[len];
            }

            page() = default;

            ~page() {
                delete this->content;
            }
        };

        struct available_chunk {
            size_t pageIndex;
            T *chunk;
        };

        size_t nextIndex = 0;
        std::unordered_map<size_t, page> pages;
        std::stack<available_chunk> availableChunks;

        memory() {
            this->new_page(250);
        }

        ~memory() {
            pages.clear();
        }

        size_t next_index() {
            auto result = this->nextIndex;
            this->nextIndex++;
            return result;
        }

        size_t max_page_length() {
            size_t result = 0;
            for (const auto &keyValue : this->pages) {
                if (keyValue.first > result) {
                    result = keyValue.first;
                }
            }
            return result;
        }

        void new_page(size_t length) {
            page newPage(length);
            size_t pageIndex = this->next_index();
            for (size_t index = 0; index < length; index++) {
                this->availableChunks.push(
                        available_chunk{
                                .pageIndex = pageIndex,
                                .chunk = newPage.content + index
                        }
                );
            }
            this->pages[pageIndex] = newPage;
        }

        void remove_page(size_t pageIndex) {
            this->pages.erase(this->pages.find(pageIndex));
        }

        [[nodiscard]] bool empty() const {
            return this->availableChunks.empty();
        }

        available_chunk allocate() {
            available_chunk result = this->availableChunks.top();
            this->availableChunks.pop();
            this->pages[result.pageIndex].allocatedElements++;
            return result;
        }

        void deallocate(size_t pageIndex, T *chunk) {
            this->pages[pageIndex].allocatedElements--;
            this->availableChunks.push(
                    available_chunk{
                            .pageIndex = pageIndex,
                            .chunk = chunk
                    }
            );
        }
    };
}
#endif //PLASMA_MEMORY_H

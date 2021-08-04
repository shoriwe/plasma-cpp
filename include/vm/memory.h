#ifndef PLASMA_MEMORY_H
#define PLASMA_MEMORY_H

#include <iostream>
#include <stack>
#include <unordered_map>

namespace memory {

    template<typename T>
    struct page {
        size_t allocatedElements = 0;
        size_t length = 0;
        T *content = nullptr;
    };

    template<typename T>
    struct chunk {
        size_t page_index;
        T *object;
    };

    template<typename T>
    struct memory {
        size_t currentIndex = 0;
        std::unordered_map<size_t, page<T> *> pages;
        std::stack<chunk<T>> availableChunks;

        memory() {
            this->new_page(250);
        }

        ~memory() {
            for (const auto &keyValue : this->pages) {
                // delete keyValue.second->content;
                // delete keyValue.second;
            }
            this->pages.clear();
        }

        size_t next_index() {
            return this->currentIndex++;
        }

        size_t max_page_length() {
            size_t result = 0;
            for (const auto &keyValue: this->pages) {
                if (keyValue.second->length > result) {
                    result = keyValue.second->length;
                }
            }
            return result;
        }

        void new_page(size_t length) {
            size_t pageIndex = this->next_index();
            this->pages[pageIndex] = new page<T> {
                .length = length,
                .content = new T[length]
            };
            for (size_t memoryIndex = 0; memoryIndex < length; memoryIndex++) {
                this->availableChunks.push(
                        chunk{
                                .page_index=pageIndex,
                                .object=this->pages[pageIndex]->content + memoryIndex,
                        }
                );
            }
        }

        void remove_page(size_t pageIndex) {
            this->pages.erase(this->pages.begin() + pageIndex);
        }

        [[nodiscard]] bool empty() const {
            return this->availableChunks.empty();
        }

        chunk<T> allocate() {
            chunk result = this->availableChunks.top();
            this->availableChunks.pop();
            this->pages[result.page_index]->allocatedElements++;
            return result;
        }

        void deallocate(size_t pageIndex, T *o) {
            this->pages[pageIndex]->allocatedElements--;
            this->availableChunks.push(
                    chunk<T>{
                            .page_index = pageIndex,
                            .object = o
                    }
            );
        }
    };
}

#endif //PLASMA_MEMORY_H

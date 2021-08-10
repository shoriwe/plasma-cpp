#ifndef PLASMA_MEMORY_H
#define PLASMA_MEMORY_H

#include <iostream>
#include <memory>
#include <stack>
#include <unordered_map>

namespace memory {

    template<typename T>
    struct page {
        size_t allocatedElements;
        size_t length;
        std::vector<T> content;

        T *index(size_t i) {
            return this->content.data() + i;
        };
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

        explicit memory(size_t initialPageLength) {
            this->new_page(initialPageLength);
        }

        ~memory() {
            // FixMe: This is nasty, why the program collapse when manually deleting allocated elements? Find a way to fix it
            if (this->pages.empty()) {
                return;
            }
            std::cout << "Does this crash? 1\n";
            std::cout << "survived 1\n";
            std::cout << "Does this crash? 2\n";
            this->pages.clear();
            std::cout << "survived 2\n";
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
            this->pages[pageIndex] = new page<T>{
                    .allocatedElements = 0,
                    .length = length,
                    .content = std::vector<T>(length, T())
            };
            for (size_t memoryIndex = 0; memoryIndex < length; memoryIndex++) {
                this->availableChunks.push(
                        chunk<T>{
                                .page_index=pageIndex,
                                .object=this->pages[pageIndex]->content.data() + memoryIndex,
                        }
                );
            }
        }

        void shrink() {
            if (this->pages.empty()) {
                return;
            }
            std::vector<size_t> toRemovePages;
            for (const auto &keyValue : this->pages) {
                if (keyValue.second->allocatedElements == 0) {
                    toRemovePages.push_back(keyValue.first);
                }
            }
            if (toRemovePages.empty()) {
                return;
            }
            for (const auto &pageIndex : toRemovePages) {
                this->remove_page(pageIndex);
            }
        }

        void remove_page(size_t pageIndex) {
            auto p = this->pages.find(pageIndex);
            if (p == this->pages.end()) {
                return;
            }
            std::stack<chunk<T>> newChunkMap;
            while (!this->availableChunks.empty()) {
                auto element = this->availableChunks.top();
                this->availableChunks.pop();
                if (element.page_index == pageIndex) {
                    continue;
                }
                newChunkMap.push(element);
            }
            this->availableChunks = newChunkMap;
            p->second->content.clear();
            this->pages.erase(p);
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

#include "reader.h"

void plasma::reader::string_reader::next() {
    this->currentIndex++;
}

void plasma::reader::string_reader::redo() {
    this->currentIndex--;
}

int plasma::reader::string_reader::index() {
    return this->currentIndex;
}

char plasma::reader::string_reader::currentChar() {
    return this->content[this->currentIndex];
}

bool plasma::reader::string_reader::hasNext() {
    return this->currentIndex < this->content.length();
}

void plasma::reader::string_reader_new(string_reader *s_reader, std::string string) {
    s_reader->content = std::move(string);
    s_reader->currentIndex = 0;
}

bool plasma::reader::string_reader_new_from_file(string_reader *s_reader, std::ifstream file) {
    s_reader->content = std::string(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>()
    );
    s_reader->currentIndex = 0;
    return true;
}
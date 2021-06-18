#ifndef PLASMA_READER_H
#define PLASMA_READER_H

#include <string>
#include <fstream>
#include <utility>

namespace plasma::reader {
    struct reader {
        virtual void next() = 0;

        virtual void redo() = 0;

        virtual bool hasNext() = 0;

        virtual int index() = 0;

        virtual char currentChar() = 0;
    };

    struct string_reader : reader {
        std::string content;
        int currentIndex;

        void next() override;

        void redo() override;

        bool hasNext() override;

        int index() override;

        char currentChar() override;
    };

    void string_reader_new(string_reader *s_reader, std::string string);

    bool string_reader_new_from_file(string_reader *s_reader, std::ifstream file);
}

#endif //PLASMA_READER_H

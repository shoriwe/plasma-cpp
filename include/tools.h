#ifndef PLASMA_TOOLS_H
#define PLASMA_TOOLS_H

#include <string>
#include <bitset>
#include <sstream>

namespace plasma::general_tooling {
    std::string replace_escaped(const std::string &string);

    std::string remove_integer_definition(std::string string);

    std::string remove_floor(std::string string);

    int64_t parse_integer(const std::string &string, bool *success);

    double parse_float(const std::string &string, bool *success);
}

#endif //PLASMA_TOOLS_H

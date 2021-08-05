#include <string>
#include <sstream>
#include <algorithm>
#include <regex>
#include <iostream>

#include "tools.h"

static std::string directCharEscapeValue(char target) {
    switch (target) {
        case 'a':
            return std::string{7};
        case 'b':
            return std::string{8};
        case 'e':
            return std::string{'\\', 'e'};
        case 'f':
            return std::string{12};
        case 'n':
            return std::string{10};
        case 'r':
            return std::string{13};
        case 't':
            return std::string{9};
        case '?':
            return std::string{'\\', '?'};
        default:
            return "";
    }
}

std::string plasma::general_tooling::replace_escaped(const std::string &string) {
    bool escaped = false;
    std::string result;
    std::string hexOrUnicode;
    std::stringstream buffer;
    uint8_t hexChar;
    uint16_t unicodeChar;
    bool isBytes = string[0] == 'b';
    for (size_t index = 0; index < string.length(); index++) {
        if (index == 0 || index == string.length() - 1) {
            continue;
        } else if (index == 1  && isBytes) {
            continue;
        }
        char character = string[index];
        if (escaped) {
            switch (character) {
                case 'a':
                case 'b':
                case 'e':
                case 'f':
                case 'n':
                case 'r':
                case 't':
                case '?':
                    result += directCharEscapeValue(character);
                    break;
                case '\\':
                case '\'':
                case '"':
                case '`':
                    result.push_back(character);
                    break;
                case 'x':
                case 'X':
                    hexOrUnicode.clear();
                    buffer.clear();
                    index++;
                    hexOrUnicode.push_back(string[index]);
                    index++;
                    hexOrUnicode.push_back(string[index]);
                    buffer << std::hex << hexOrUnicode;
                    buffer >> hexChar;
                    result.push_back(hexChar);
                case 'u':
                    hexOrUnicode.clear();
                    buffer.clear();
                    index++;
                    hexOrUnicode.push_back(string[index]);
                    index++;
                    hexOrUnicode.push_back(string[index]);
                    index++;
                    hexOrUnicode.push_back(string[index]);
                    index++;
                    hexOrUnicode.push_back(string[index]);
                    buffer << std::hex << hexOrUnicode;
                    buffer >> unicodeChar;
                    result.push_back(unicodeChar & 0xFF);
                    result.push_back(unicodeChar >> 8);
                default:
                    break;
            }
            escaped = false;
        } else if (character == '\\') {
            escaped = true;
        } else {
            result.push_back(character);
        }
    }
    return result;
}

std::string plasma::general_tooling::remove_integer_definition(std::string string) {
    string.erase(0, 2);
    return string;
}

std::string plasma::general_tooling::remove_floor(std::string string) {
    string.erase(std::remove(string.begin(), string.end(), '_'), string.end());
    return string;
}

bool isBase10(const std::string &string) {
    return std::regex_search(string, std::regex("^[0-9]+[0-9_]*$"));
}

bool isBase16(const std::string &string) {
    return std::regex_search(string, std::regex("^0[xX][0-9a-fA-F]+[0-9a-fA-F_]*$"));
}

bool isBase2(const std::string &string) {
    return std::regex_search(string, std::regex("^0[bB][01]+[01_]*$"));
}

bool isBase8(const std::string &string) {
    return std::regex_search(string, std::regex("^0[oO][0-7]+[0-7_]*$"));
}

int64_t plasma::general_tooling::parse_integer(const std::string &string, bool *success) {
    (*success) = true;
    if (isBase10(string)) {
        return (int64_t) std::stoll(
                plasma::general_tooling::remove_floor(string), nullptr,
                10);
    } else if (isBase16(string)) {
        return (int64_t) std::stoll(
                plasma::general_tooling::remove_integer_definition(
                        plasma::general_tooling::remove_floor(string)),
                nullptr,
                16);
    } else if (isBase2(string)) {
        return (int64_t) std::stoll(
                plasma::general_tooling::remove_integer_definition(
                        plasma::general_tooling::remove_floor(string)),
                nullptr,
                2);
    } else if (isBase8(string)) {
        return (int64_t) std::stoll(
                plasma::general_tooling::remove_integer_definition(
                        plasma::general_tooling::remove_floor(string)),
                nullptr,
                8);
    }
    (*success) = false;
    return 0;
}
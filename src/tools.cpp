#include <string>
#include <sstream>
#include <algorithm>

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
    for (size_t index = 0; index < string.length(); index++) {
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

std::string plasma::general_tooling::remove_floor(std::string string) {
    string.erase(std::remove(string.begin(), string.end(), '_'), string.end());
    return string;
}
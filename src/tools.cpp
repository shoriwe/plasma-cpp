#include <string>
#include <algorithm>
#include <regex>

#include "tools.h"

static std::string directCharEscapeValue(char target) {
    switch (target) {
        case 'a':
            return "\a";
        case 'b':
            return "\b";
        case 'e':
            return "\\e";
        case 'f':
            return "\f";
        case 'n':
            return "\n";
        case 'r':
            return "\r";
        case 't':
            return "\t";
        case '?':
            return "\\?";
        default:
            return "";
    }
}

std::string UnicodeToUTF8(unsigned int codepoint) {
    std::string out;

    if (codepoint <= 0x7f)
        out.append(1, static_cast<char>(codepoint));
    else if (codepoint <= 0x7ff) {
        out.append(1, static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
        out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
    } else if (codepoint <= 0xffff) {
        out.append(1, static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
        out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
        out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
    } else {
        out.append(1, static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
        out.append(1, static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
        out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
        out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
    }
    return out;
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
        } else if (index == 1 && isBytes) {
            continue;
        }
        uint8_t first, second, third, fourth;
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
                    hexOrUnicode.push_back(string[index++]);
                    hexOrUnicode.push_back(string[index]);

                    hexChar = std::strtoul(hexOrUnicode.c_str(), nullptr, 16);

                    result.push_back(hexChar);
                    break;
                case 'u':
                    hexOrUnicode.clear();
                    buffer.clear();

                    index++;
                    first = string[index++];
                    second = string[index++];
                    third = string[index++];
                    fourth = string[index];

                    hexOrUnicode.push_back(first);
                    hexOrUnicode.push_back(second);
                    hexOrUnicode.push_back(third);
                    hexOrUnicode.push_back(fourth);

                    unicodeChar = std::strtoul(hexOrUnicode.c_str(), nullptr, 16);

                    result += UnicodeToUTF8(unicodeChar);
                    break;
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

static bool isFloat(const std::string &string) {
    if (std::regex_search(string, std::regex("^[0-9]+[0-9_]*\\.[0-9]+[0-9_]*$"))) {
        return true;
    } else if (std::regex_search(string, std::regex("^[0-9]+[0-9_]*[eE][-+][0-9]+[0-9_]*$"))) {
        return true;
    } else if (std::regex_search(string, std::regex("^[0-9]+[0-9_]*\\.[0-9]+[0-9_]*[eE][-+][0-9]+[0-9_]*$"))) {
        return true;
    }
    return false;
}

double plasma::general_tooling::parse_float(const std::string &string, bool *success) {
    if (isFloat(string)) {
        (*success) = true;
        return std::stod(plasma::general_tooling::remove_floor(string));
    }
    (*success) = false;
    return 0;
}
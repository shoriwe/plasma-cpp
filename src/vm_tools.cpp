#include <vector>
#include <algorithm>

#include "vm/virtual_machine.h"

// Content
plasma::vm::value *
plasma::vm::virtual_machine::content_index(context *c, value *index, value *source,
                                           bool *success) {
    bool fail = false;
    if (index->typeId == Integer) {
        size_t realIndex = plasma::vm::virtual_machine::calculate_index(index->integer, source->content.size(), &fail);
        if (fail) {
            (*success) = false;
            return this->NewIndexOutOfRange(c, source->content.size(), index->integer);
        }
        (*success) = true;
        return source->content[realIndex];
    } else if (index->typeId == Tuple) {
        if (index->content.size() != 2) {
            (*success) = false;
            return this->NewInvalidNumberOfArgumentsError(c, 2, index->content.size());
        }
        value *start = index->content[0];
        value *end = index->content[1];
        if (start->typeId != Integer) {
            (*success) = false;
            return this->NewInvalidTypeError(
                    c,
                    start->get_type(c, this),
                    std::vector<std::string>{IntegerName}
            );
        }
        if (end->typeId != Integer) {
            (*success) = false;
            return this->NewInvalidTypeError(
                    c,
                    end->get_type(c, this),
                    std::vector<std::string>{IntegerName}
            );
        }
        size_t startRealIndex = plasma::vm::virtual_machine::calculate_index(start->integer, source->content.size(),
                                                                             &fail);
        if (fail) {
            (*success) = false;
            return this->NewIndexOutOfRange(c, source->content.size(), start->integer);
        }
        fail = false;
        size_t endRealIndex = plasma::vm::virtual_machine::calculate_index(end->integer, source->content.size(), &fail);
        if (fail) {
            (*success) = false;
            return this->NewIndexOutOfRange(c, source->content.size(), end->integer);
        }
        (*success) = true;
        if (startRealIndex > endRealIndex) {
            if (source->typeId == Array) {
                return this->new_array(c, false, std::vector<value *>());
            }
            return this->new_tuple(c, false, std::vector<value *>());
        }
        if (source->typeId == Array) {
            return this->new_array(c, false, std::vector<value *>(&source->content[startRealIndex],
                                                                  &source->content[endRealIndex]));
        }
        return this->new_tuple(c, false, std::vector<value *>(&source->content[startRealIndex],
                                                              &source->content[endRealIndex]));
    }
    (*success) = false;
    return this->NewInvalidTypeError(
            c,
            index->get_type(c, this),
            std::vector<std::string>{IntegerName, TupleName}
    );
}

plasma::vm::value *
plasma::vm::virtual_machine::content_assign(context *c, value *container, value *index, value *object, bool *success) {
    if (index->typeId != Integer) {
        (*success) = false;
        return this->NewInvalidTypeError(c, index->get_type(c, this), std::vector<std::string>{
                                                 IntegerName
                                         }
        );
    }
    bool fail = false;
    size_t realIndex = plasma::vm::virtual_machine::calculate_index(index->integer, container->content.size(), &fail);
    if (fail) {
        (*success) = false;
        return this->NewIndexOutOfRange(c, container->content.size(), index->integer);
    }

    container->content[realIndex] = object;

    (*success) = true;
    return this->get_none(c);
}

plasma::vm::value *plasma::vm::virtual_machine::content_to_string(context *c, value *container, bool *success) {
    std::string result;
    bool first = true;
    bool found;
    bool callSuccess;
    for (const auto &object : container->content) {
        found = false;
        callSuccess = false;
        value *objectToString = object->get(c, this, ToString, &found);
        if (!found) {
            (*success) = false;
            return objectToString;
        }
        value *objectAsString = this->call_function(c, objectToString, std::vector<value *>(), &callSuccess);
        if (!callSuccess) {
            (*success) = false;
            return objectAsString;
        }
        if (objectAsString->typeId != String) {
            (*success) = false;
            return this->NewInvalidTypeError(c, objectAsString->get_type(c, this),
                                             std::vector<std::string>{StringName});
        }
        if (first) {
            first = false;
        } else {
            result += ", ";
        }
        result += objectAsString->string;
    }
    (*success) = true;
    if (container->typeId == Array) {
        return this->new_string(c, false, "[" + result + "]");
    }
    return this->new_string(c, false, "(" + result + ")");
}

plasma::vm::value *plasma::vm::virtual_machine::content_iterator(context *c, value *source) {
    value *iterator = this->new_iterator(c, false);
    iterator->integer = 0;
    iterator->source = source;
    iterator->set(HasNext,
                  this->new_function(
                          c,
                          false,
                          iterator,
                          new_builtin_callable(
                                  0,
                                  [=](value *self,
                                      const std::vector<value *> &arguments,
                                      bool *success) -> value * {
                                      (*success) = true;
                                      return this->get_boolean(c, self->integer < iterator->source->content.size());
                                  }
                          )
                  )
    );
    iterator->set(Next,
                  this->new_function(
                          c,
                          false,
                          iterator,
                          new_builtin_callable(
                                  0,
                                  [=](value *self,
                                      const std::vector<value *> &arguments,
                                      bool *success) -> value * {
                                      if (self->integer >= iterator->source->content.size()) {
                                          (*success) = false;
                                          return this->NewIndexOutOfRange(c, iterator->source->content.size(),
                                                                          self->integer);
                                      }
                                      (*success) = true;
                                      value *result = iterator->source->content[self->integer];
                                      self->integer++;
                                      return result;
                                  }
                          )
                  )
    );
    return iterator;
}

plasma::vm::value *plasma::vm::virtual_machine::content_equals(context *c, value *leftHandSide, value *rightHandSide,
                                                               bool *result) {
    if (leftHandSide->content.size() != rightHandSide->content.size()) {
        (*result) = false;
        return nullptr;
    }
    for (size_t index = 0; index < leftHandSide->content.size(); index++) {
        bool objectsComparison = false;
        value *comparisonError = this->equals(c, leftHandSide->content[index], rightHandSide->content[index],
                                              &objectsComparison);
        if (comparisonError != nullptr) {
            (*result) = false;
            return comparisonError;
        }
        if (!objectsComparison) {
            (*result) = false;
            return nullptr;
        }
    }
    (*result) = true;
    return nullptr;
}

plasma::vm::value *
plasma::vm::virtual_machine::content_contains(context *c, value *container, value *object, bool *result) {
    if (container->content.empty()) {
        (*result) = false;
        return nullptr;
    }
    for (const auto &contentObject : container->content) {

        value *comparisonError = this->equals(c, object, contentObject, result);
        if (comparisonError != nullptr) {
            (*result) = false;
            return comparisonError;
        }
        if (*result) {
            return nullptr;
        }

    }
    (*result) = false;
    return nullptr;
}

plasma::vm::value *
plasma::vm::virtual_machine::content_repeat(context *c, const std::vector<plasma::vm::value *> &content,
                                            size_t times,
                                            std::vector<plasma::vm::value *> *result) {
    if (times == 0) {
        return nullptr;
    }

    result->clear();
    result->shrink_to_fit();
    result->reserve(content.size() * times);

    std::vector<value *> copyFunctions;
    copyFunctions.reserve(content.size());
    value *copyFunction;
    bool getSuccess;
    for (auto originalValue : content) {
        getSuccess = false;
        copyFunction = originalValue->get(c, this, Copy, &getSuccess);
        if (!getSuccess) {
            return copyFunction;
        }
        copyFunctions.push_back(copyFunction);
    }

    value *copyFunctionResult;
    bool copySuccess;
    for (size_t time = 0; time < times; time++) {
        for (size_t index = 0; index < content.size(); index++) {
            copySuccess = false;
            copyFunction = copyFunctions[index];
            copyFunctionResult = this->call_function(c, copyFunction, std::vector<value *>(), &copySuccess);
            if (!copySuccess) {
                return copyFunctionResult;
            }
            result->push_back(copyFunctionResult);
        }
    }

    return nullptr;
}

// Bytes

plasma::vm::value *plasma::vm::virtual_machine::bytes_index(context *c, value *source, value *index, bool *success) {
    bool fail = false;
    if (index->typeId == Integer) {
        size_t realIndex = plasma::vm::virtual_machine::calculate_index(index->integer, source->bytes.size(), &fail);
        if (fail) {
            (*success) = false;
            return this->NewIndexOutOfRange(c, source->bytes.size(), index->integer);
        }
        (*success) = true;
        return this->new_integer(c, false, source->bytes[realIndex]);
    } else if (index->typeId == Tuple) {
        if (index->content.size() != 2) {
            (*success) = false;
            return this->NewInvalidNumberOfArgumentsError(c, 2, index->content.size());
        }
        value *start = index->content[0];
        value *end = index->content[1];
        if (start->typeId != Integer) {
            (*success) = false;
            return this->NewInvalidTypeError(
                    c,
                    start->get_type(c, this),
                    std::vector<std::string>{IntegerName}
            );
        }
        if (end->typeId != Integer) {
            (*success) = false;
            return this->NewInvalidTypeError(
                    c,
                    end->get_type(c, this),
                    std::vector<std::string>{IntegerName}
            );
        }
        size_t startRealIndex = plasma::vm::virtual_machine::calculate_index(start->integer, source->bytes.size(),
                                                                             &fail);
        if (fail) {
            (*success) = false;
            return this->NewIndexOutOfRange(c, source->bytes.size(), start->integer);
        }
        fail = false;
        size_t endRealIndex = plasma::vm::virtual_machine::calculate_index(end->integer, source->bytes.size(), &fail);
        if (fail) {
            (*success) = false;
            return this->NewIndexOutOfRange(c, source->bytes.size(), end->integer);
        }
        (*success) = true;
        if (startRealIndex > endRealIndex) {
            return this->new_bytes(c, false, std::vector<uint8_t>());
        }
        return this->new_bytes(c, false, std::vector<uint8_t>(&source->bytes[startRealIndex],
                                                              &source->bytes[endRealIndex]));
    }
    (*success) = false;
    return this->NewInvalidTypeError(
            c,
            index->get_type(c, this),
            std::vector<std::string>{IntegerName, TupleName}
    );
}

plasma::vm::value *plasma::vm::virtual_machine::bytes_to_string(context *c, value *bytesObject) {
    std::string result;
    result.reserve(bytesObject->bytes.size());
    for (const auto &byte : bytesObject->bytes) {
        result.push_back(byte);
    }
    return this->new_string(c, false, result);
}

std::vector<plasma::vm::value *> plasma::vm::virtual_machine::bytes_to_integer_content(context *c, value *bytes) {
    std::vector<value *> result;
    result.reserve(bytes->bytes.size());
    for (const auto &byte : bytes->bytes) {
        result.push_back(this->new_integer(c, false, byte));
    }
    return result;
}

plasma::vm::value *plasma::vm::virtual_machine::bytes_iterator(context *c, value *bytes) {
    value *iterator = this->new_iterator(c, false);
    iterator->integer = 0;
    iterator->source = bytes;
    iterator->set(HasNext,
                  this->new_function(
                          c,
                          false,
                          iterator,
                          new_builtin_callable(
                                  0,
                                  [=](value *self,
                                      const std::vector<value *> &arguments,
                                      bool *success) -> value * {
                                      (*success) = true;
                                      return this->get_boolean(c, self->integer < iterator->source->bytes.size());
                                  }
                          )
                  )
    );
    iterator->set(Next,
                  this->new_function(
                          c,
                          false,
                          iterator,
                          new_builtin_callable(
                                  0,
                                  [=](value *self,
                                      const std::vector<value *> &arguments,
                                      bool *success) -> value * {
                                      if (self->integer >= iterator->source->bytes.size()) {
                                          (*success) = false;
                                          return this->NewIndexOutOfRange(c, iterator->source->bytes.size(),
                                                                          self->integer);
                                      }
                                      (*success) = true;
                                      value *result = this->new_integer(c, false,
                                                                        iterator->source->bytes[self->integer]);
                                      self->integer++;
                                      return result;
                                  }
                          )
                  )
    );
    return iterator;
}

plasma::vm::value *
plasma::vm::virtual_machine::bytes_equals(value *leftHandSide, value *rightHandSide, bool *result) {
    (*result) = leftHandSide->bytes == rightHandSide->bytes;
    return nullptr;
}

plasma::vm::value *
plasma::vm::virtual_machine::bytes_contains(context *c, value *bytes, value *subBytes, bool *result) {
    if (subBytes->typeId != Bytes) {
        return this->NewInvalidTypeError(c, subBytes->get_type(c, this), std::vector<std::string>{Bytes});
    }
    if (subBytes->bytes.size() == 1) {
        (*result) = std::find(bytes->bytes.begin(), bytes->bytes.end(), subBytes->bytes[0]) != bytes->bytes.end();
        return nullptr;
    } else if (subBytes->bytes.empty()) {
        (*result) = false;
        return nullptr;
    }
    (*result) = std::search(bytes->bytes.begin(), bytes->bytes.end(), subBytes->bytes.begin(), subBytes->bytes.end()) !=
                bytes->bytes.end();
    return nullptr;
}

plasma::vm::value *
plasma::vm::virtual_machine::bytes_repeat(value *bytes, size_t times, std::vector<uint8_t> *result) {
    if (times == 0) {
        return nullptr;
    }
    result->reserve(bytes->bytes.size() * times);
    for (size_t time = 0; time < times; time++) {
        result->insert(result->end(), bytes->bytes.begin(), bytes->bytes.end());
    }
    return nullptr;
}


// String

plasma::vm::value *
plasma::vm::virtual_machine::string_index(context *c, value *source, value *index, bool *success) {
    bool fail = false;
    if (index->typeId == Integer) {
        size_t realIndex = plasma::vm::virtual_machine::calculate_index(index->integer, source->string.size(), &fail);
        if (fail) {
            (*success) = false;
            return this->NewIndexOutOfRange(c, source->string.size(), index->integer);
        }
        (*success) = true;
        return this->new_string(c, false, std::string(1, source->string[realIndex]));
    } else if (index->typeId == Tuple) {
        if (index->content.size() != 2) {
            (*success) = false;
            return this->NewInvalidNumberOfArgumentsError(c, 2, index->content.size());
        }
        value *start = index->content[0];
        value *end = index->content[1];
        if (start->typeId != Integer) {
            (*success) = false;
            return this->NewInvalidTypeError(
                    c,
                    start->get_type(c, this),
                    std::vector<std::string>{IntegerName}
            );
        }
        if (end->typeId != Integer) {
            (*success) = false;
            return this->NewInvalidTypeError(
                    c,
                    end->get_type(c, this),
                    std::vector<std::string>{IntegerName}
            );
        }
        size_t startRealIndex = plasma::vm::virtual_machine::calculate_index(start->integer, source->string.size(),
                                                                             &fail);
        if (fail) {
            (*success) = false;
            return this->NewIndexOutOfRange(c, source->string.size(), start->integer);
        }
        fail = false;
        size_t endRealIndex = plasma::vm::virtual_machine::calculate_index(end->integer, source->string.size(), &fail);
        if (fail) {
            (*success) = false;
            return this->NewIndexOutOfRange(c, source->string.size(), end->integer);
        }
        (*success) = true;
        if (startRealIndex > endRealIndex) {
            return this->new_string(c, false, std::string());
        }
        return this->new_string(c, false, std::string(&source->string[startRealIndex],
                                                      &source->string[endRealIndex]));
    }
    (*success) = false;
    return this->NewInvalidTypeError(
            c,
            index->get_type(c, this),
            std::vector<std::string>{IntegerName, TupleName}
    );
}

std::vector<plasma::vm::value *> plasma::vm::virtual_machine::string_to_integer_content(context *c, value *string) {
    std::vector<value *> result;
    result.reserve(string->string.size());
    for (const auto &stringCharacter : string->string) {
        result.push_back(this->new_integer(c, false, stringCharacter));
    }
    return result;
}

plasma::vm::value *plasma::vm::virtual_machine::string_iterator(context *c, value *string) {
    value *iterator = this->new_iterator(c, false);
    iterator->integer = 0;
    iterator->source = string;
    iterator->set(HasNext,
                  this->new_function(
                          c,
                          false,
                          iterator,
                          new_builtin_callable(
                                  0,
                                  [=](value *self,
                                      const std::vector<value *> &arguments,
                                      bool *success) -> value * {
                                      (*success) = true;
                                      return this->get_boolean(c, self->integer < iterator->source->string.size());
                                  }
                          )
                  )
    );
    iterator->set(Next,
                  this->new_function(
                          c,
                          false,
                          iterator,
                          new_builtin_callable(
                                  0,
                                  [=](value *self,
                                      const std::vector<value *> &arguments,
                                      bool *success) -> value * {
                                      if (self->integer >= iterator->source->string.size()) {
                                          (*success) = false;
                                          return this->NewIndexOutOfRange(c, iterator->source->string.size(),
                                                                          self->integer);
                                      }
                                      (*success) = true;
                                      value *result = this->new_string(c, false, std::string(1,
                                                                                             iterator->source->string[self->integer]));
                                      self->integer++;
                                      return result;
                                  }
                          )
                  )
    );
    return iterator;
}

plasma::vm::value *
plasma::vm::virtual_machine::string_equals(value *leftHandSide, plasma::vm::value *rightHandSide, bool *result) {
    (*result) = leftHandSide->string == rightHandSide->string;
    return nullptr;
}

plasma::vm::value *
plasma::vm::virtual_machine::string_contains(value *string, plasma::vm::value *subString, bool *result) {
    (*result) = string->string.find(subString->string) != std::string::npos;
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::string_repeat(value *string, size_t times, std::string *result) {
    if (times == 0) {
        return nullptr;
    }
    result->reserve(string->string.size() * times);
    for (size_t time = 0; time < times; time++) {
        result->insert(result->end(), string->string.begin(), string->string.end());
    }
    return nullptr;
}

// HashTable

plasma::vm::value *plasma::vm::virtual_machine::hashtable_index(context *c, value *source, value *key, bool *success) {
    int64_t hashKey;
    value *hashCalculationError = this->calculate_hash(c, key, &hashKey);
    if (hashCalculationError != nullptr) {
        (*success) = false;
        return hashCalculationError;
    }
    if (!source->keyValues.contains(hashKey)) {
        (*success) = false;
        return this->NewKeyNotFoundError(c, key);
    }
    bool comparison;
    value *comparisonError;
    for (const auto &entry : source->keyValues[hashKey]) {
        comparisonError = this->equals(c, entry.key, key, &comparison);
        if (comparisonError != nullptr) {
            (*success) = false;
            return comparisonError;
        }
        if (comparison) {
            (*success) = true;
            return entry.value;
        }
    }
    (*success) = false;
    return this->NewKeyNotFoundError(c, key);
}

plasma::vm::value *
plasma::vm::virtual_machine::hashtable_assign(context *c, value *source, value *key, value *object, bool *success) {
    int64_t hashKey;
    value *hashCalculationError = this->calculate_hash(c, key, &hashKey);
    if (hashCalculationError != nullptr) {
        (*success) = false;
        return hashCalculationError;
    }
    if (!source->keyValues.contains(hashKey)) {
        source->keyValues[hashKey] = std::vector<key_value>{
                key_value{
                        .key = key,
                        .value = object
                }
        };
        (*success) = true;
        return this->get_none(c);
    }
    bool comparison;
    value *comparisonError;
    for (size_t index = 0; index < source->keyValues[hashKey].size(); index++) {

        comparisonError = this->equals(c, source->keyValues[hashKey][index].key, key, &comparison);
        if (comparisonError != nullptr) {
            (*success) = false;
            return comparisonError;
        }
        if (comparison) {
            source->keyValues[hashKey][index].value = object;
            (*success) = true;
            return this->get_none(c);
        }
    }
    source->keyValues[hashKey].push_back(
            key_value{
                    .key = key,
                    .value = object
            }
    );
    (*success) = true;
    return this->get_none(c);
}


plasma::vm::value *plasma::vm::virtual_machine::hashtable_iterator(context *c, value *hashtable) {
    value *result = this->new_iterator(c, false);
    result->source = hashtable;
    result->integer = 0;

    /*
     * Nasty code here, find a way to just iterate over the existing map and not iterate over a new vector
     */
    auto content = plasma::vm::virtual_machine::hashtable_to_content(hashtable); // Fixme
    result->set(
            HasNext,
            this->new_function(
                    c,
                    false,
                    result,
                    new_builtin_callable(
                            0,
                            [=](value *self, const std::vector<value *> &arguments,
                                bool *success) -> value * {
                                (*success) = true;
                                return this->get_boolean(c, self->integer < content.size());
                            }
                    )
            )
    );
    result->set(
            Next,
            this->new_function(
                    c,
                    false,
                    result,
                    new_builtin_callable(
                            0,
                            [=](value *self, const std::vector<value *> &arguments,
                                bool *success) -> value * {
                                if (self->integer >= content.size()) {
                                    (*success) = false;
                                    return this->NewIndexOutOfRange(c, content.size(), self->integer);
                                }
                                return this->hashtable_index(c, self->source, content[self->integer], success);
                            }
                    )
            )
    );
    return result;
}

plasma::vm::value *plasma::vm::virtual_machine::hashtable_to_string(context *c, value *hashtableObject, bool *success) {

    std::string result;
    bool first = true;
    value *objectToString;
    value *objectAsString;
    for (const auto &entry : hashtableObject->keyValues) {
        for (const auto &keyValue : entry.second) {
            if (first) {
                first = false;
            } else {
                result += ", ";
            }
            objectToString = keyValue.key->get(c, this, ToString, success);
            if (!(*success)) {
                return objectToString;
            }
            objectAsString = this->call_function(c, objectToString, std::vector<value *>(), success);
            if (!(*success)) {
                return objectAsString;
            }
            if (objectAsString->typeId != String) {
                (*success) = false;
                return this->NewInvalidTypeError(c, objectAsString->get_type(c, this),
                                                 std::vector<std::string>{StringName}
                );
            }
            result += objectAsString->string + ": ";
            objectToString = keyValue.value->get(c, this, ToString, success);
            if (!(*success)) {
                return objectToString;
            }
            objectAsString = this->call_function(c, objectToString, std::vector<value *>(), success);
            if (!(*success)) {
                return objectAsString;
            }
            if (objectAsString->typeId != String) {
                (*success) = false;
                return this->NewInvalidTypeError(c, objectAsString->get_type(c, this),
                                                 std::vector<std::string>{StringName}
                );
            }
            result += objectAsString->string;
        }
    }
    (*success) = true;
    return this->new_string(c, false, "{" + result + "}");
}

std::vector<plasma::vm::value *> plasma::vm::virtual_machine::hashtable_to_content(value *source) {
    std::vector<value *> result;
    for (const auto &entry : source->keyValues) {
        for (const auto &keyValue : entry.second) {
            result.push_back(keyValue.key);
        }
    }
    return result;
}

plasma::vm::value *
plasma::vm::virtual_machine::hashtable_equals(context *c, value *leftHandSide, value *rightHandSide, bool *result) {
    (*result) = false;
    if (leftHandSide->keyValues.size() != rightHandSide->keyValues.size()) {
        return nullptr;
    }
    bool indexSuccess;
    bool valueComparison;
    value *indexResult;
    value *comparisonError;
    for (const auto &entry : leftHandSide->keyValues) {
        for (const key_value &keyValue : entry.second) {
            indexResult = this->hashtable_index(c, rightHandSide, keyValue.key, &indexSuccess);
            if (!indexSuccess) {
                return indexResult;
            }
            comparisonError = this->hashtable_equals(c, keyValue.value, indexResult, &valueComparison);
            if (comparisonError != nullptr) {
                return comparisonError;
            }
            if (!valueComparison) {
                return nullptr;
            }
        }
    }
    for (const auto &entry : rightHandSide->keyValues) {
        for (const key_value &keyValue : entry.second) {
            indexResult = this->hashtable_index(c, rightHandSide, keyValue.key, &indexSuccess);
            if (!indexSuccess) {
                return indexResult;
            }
            comparisonError = this->hashtable_equals(c, keyValue.value, indexResult, &valueComparison);
            if (comparisonError != nullptr) {
                return comparisonError;
            }
            if (!valueComparison) {
                return nullptr;
            }
        }
    }
    (*result) = true;
    return nullptr;
}

plasma::vm::value *
plasma::vm::virtual_machine::hashtable_contains(context *c, value *hashTable, value *key, bool *success) {
    value *indexedObject = this->hashtable_index(c, hashTable, key, success);
    if (!(*success)) {
        return indexedObject;
    }
    return nullptr;

}

plasma::vm::value *plasma::vm::virtual_machine::hashtable_copy(context *c, value *hashTable, bool *success) {
    value *result = this->new_hash_table(c, false);
    value *assignError;
    for (const auto &entry : hashTable->keyValues) {
        for (const auto &keyValue : entry.second) {
            assignError = this->hashtable_assign(c, result, keyValue.key, keyValue.value, success);
            if (!(*success)) {
                (*success) = false;
                return assignError;
            }
        }
    }
    (*success) = true;
    return result;
}

// Any

size_t plasma::vm::virtual_machine::calculate_index(int64_t index, size_t length, bool *fail) {
    if (((int64_t) length) <= index) {
        (*fail) = true;
        return 0;
    } else if (index < 0) {
        index = ((int64_t) length) + index;
        if (index < 0) {
            (*fail) = true;
            return 0;
        }
    }
    (*fail) = false;
    return index;
}


plasma::vm::value *
plasma::vm::virtual_machine::equals(context *c, value *leftHandSide, value *rightHandSide,
                                    bool *result) {
    (*result) = false;
    bool success = false;
    value *resultValue;
    value *equalsFunction = leftHandSide->get(c, this, Equals, &success);
    if (!success) {
        // Try with the right equals function
        equalsFunction = rightHandSide->get(c, this, RightEquals, &success);
        if (!success) {
            return equalsFunction;
        }
        success = false;
        resultValue = this->call_function(c, equalsFunction, std::vector<value *>{leftHandSide}, &success);
        if (!success) {
            return resultValue;
        }
        return this->interpret_as_boolean(c, resultValue, result);
    }
    success = false;
    resultValue = this->call_function(c, equalsFunction, std::vector<value *>{rightHandSide}, &success);
    if (!success) {
        return resultValue;
    }
    return this->interpret_as_boolean(c, resultValue, result);
}

plasma::vm::value *
plasma::vm::virtual_machine::calculate_hash(plasma::vm::context *c, plasma::vm::value *v, int64_t *hash_) {
    bool success = false;
    value *hashFunction = v->get(c, this, Hash, &success);
    if (!success) {
        return hashFunction;
    }
    success = false;
    value *hashValue = this->call_function(c, hashFunction, std::vector<value *>(), &success);
    if (!success) {
        return hashValue;
    }
    if (hashValue->typeId != Integer) {
        return this->NewInvalidTypeError(c, hashValue->get_type(c, this), std::vector<std::string>{IntegerName});
    }
    (*hash_) = hashValue->integer;
    return nullptr;
}

plasma::vm::value *
plasma::vm::virtual_machine::interpret_as_boolean(context *c, plasma::vm::value *v, bool *result) {
    (*result) = false;
    if (v->typeId == Boolean) {
        (*result) = v->boolean;
        return nullptr;
    }
    bool success = false;
    value *toBoolFunction = v->get(c, this, ToBool, &success);
    if (!success) {
        return toBoolFunction;
    }
    success = false;
    value *toBoolResult = this->call_function(c, toBoolFunction, std::vector<value *>(), &success);
    if (!success) {
        return toBoolResult;
    }
    if (toBoolResult->typeId != Boolean) {
        return this->NewInvalidTypeError(c, toBoolResult->get_type(c, this), std::vector<std::string>{BoolName,});
    }
    (*result) = toBoolResult->boolean;
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::interpret_as_iterator(context *c, struct value *v, bool *success) {
    // Check if the object has Next and HasNext Functions
    bool nextFound;
    v->get(c, this, Next, &nextFound);
    bool hasNextFound;
    v->get(c, this, HasNext, &hasNextFound);
    if (nextFound && hasNextFound) {
        (*success) = true;
        return v;
    }
    // If not do the transformation
    bool iterFound;
    auto iterFunc = v->get(c, this, Iter, &iterFound);
    if (!iterFound) {
        (*success) = false;
        return iterFunc;
    }
    return this->call_function(c, iterFunc, std::vector<value*>{}, success);
}

plasma::vm::value *
plasma::vm::virtual_machine::unpack_values(context *c, struct value *v, size_t expect, std::vector<value *> *result) {
    // Check if the result can be iterated
    bool interpretationSuccess = false;
    auto valueAsIter = this->interpret_as_iterator(c, v,
                                                   &interpretationSuccess);
    if (!interpretationSuccess) {
        return valueAsIter;
    }
    bool foundObject;
    auto hasNext = valueAsIter->get(c, this, HasNext, &foundObject);
    if (!foundObject) {
        return hasNext;
    }
    auto next = valueAsIter->get(c, this, Next, &foundObject);
    if (!foundObject) {
        return next;
    }
    // Unpack the next value into a list of arguments
    std::vector<value *> operationArguments;
    bool callSuccess = false;
    for (size_t argumentIndex = 0; argumentIndex < expect; argumentIndex++) {
        // Check if there are more values to unpack
        auto doesHaveNextValue = this->call_function(c, hasNext, std::vector<value *>{}, &callSuccess);
        if (!callSuccess) {
            return doesHaveNextValue;
        }
        bool doesHaveNext;
        auto interpretationError = this->interpret_as_boolean(c, doesHaveNextValue, &doesHaveNext);
        if (interpretationError != nullptr) {
            return interpretationError;
        }
        // If not, crash
        if (!hasNext) {
            return this->NewInvalidNumberOfArgumentsError(c, expect, argumentIndex);
        }
        // Capture the child value
        callSuccess = false;
        auto childValue = this->call_function(c, next, std::vector<value *>{}, &callSuccess);
        if (!callSuccess) {
            return childValue;
        }
        // Else push the value to the container result
        result->push_back(childValue);
    }
    return nullptr;
}
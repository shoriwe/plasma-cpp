#include "vm/virtual_machine.h"

plasma::vm::value *
plasma::vm::virtual_machine::new_object(struct context &c, bool isBuiltIn, const std::string &typeName,
                                        value *type) {
    value *result = c.allocate_value();
    result->typeId = Object;

    result->id = this->next_id();
    result->typeName = typeName;
    result->type = type;
    result->isBuiltIn = isBuiltIn;
    result->onDemandSymbols = std::unordered_map<std::string, on_demand_loader>();

    result->boolean = true;
    result->string = "";
    result->integer = 0;
    result->floating = 0;
    result->content = std::vector<value *>();
    result->keyValues = std::unordered_map<int64_t, std::vector<key_value>>();
    result->bytes = std::vector<uint8_t>();
    this->ObjectInitialize(isBuiltIn)(c, result);
    result->set(Self, result);
    return result;
}

plasma::vm::value *plasma::vm::virtual_machine::new_hash_table(struct context &c, bool isBuiltIn) {
    value *result = this->new_object(c, isBuiltIn, HashName, nullptr);
    result->typeId = HashTable;
    this->HashTableInitialize(isBuiltIn)(c, result);
    result->set(Self, result);
    return result;
}

plasma::vm::value *plasma::vm::virtual_machine::new_array(struct context &c, bool isBuiltIn,
                                                          const std::vector<plasma::vm::value *> &content) {
    value *result = this->new_object(c, isBuiltIn, ArrayName, nullptr);
    result->typeId = Array;
    result->content = content;
    this->ArrayInitialize(isBuiltIn)(c, result);
    result->set(Self, result);
    return result;
}

plasma::vm::value *
plasma::vm::virtual_machine::new_function(struct context &c, bool isBuiltIn,
                                          const struct callable &callable_) {

    value *result = this->new_object(c, isBuiltIn, FunctionName, nullptr);
    result->typeId = Function;
    result->callable_ = callable_;
    result->set(Self, result);
    return result;
}

plasma::vm::value *
plasma::vm::virtual_machine::new_bytes(struct context &c, bool isBuiltIn, const std::vector<uint8_t> &bytes) {
    value *result = this->new_object(c, isBuiltIn, BytesName, nullptr);
    result->typeId = Bytes;
    result->bytes = bytes;
    this->BytesInitialize(isBuiltIn)(c, result);
    result->set(Self, result);
    return result;
}

plasma::vm::value *plasma::vm::virtual_machine::new_iterator(struct context &c, bool isBuiltIn) {
    value *result = this->new_object(c, isBuiltIn, IteratorName, nullptr);
    result->typeId = Iterator;
    this->IteratorInitialize(isBuiltIn)(c, result);
    result->set(Self, result);
    return result;
}

plasma::vm::value *plasma::vm::virtual_machine::new_tuple(struct context &c, bool isBuiltIn,
                                                          const std::vector<plasma::vm::value *> &content) {
    value *result = this->new_object(c, isBuiltIn, TupleName, nullptr);
    result->typeId = Tuple;
    this->TupleInitialize(isBuiltIn)(c, result);
    result->set(Self, result);
    return result;

}

plasma::vm::value *plasma::vm::virtual_machine::new_none(struct context &c, bool isBuiltIn) {
    value *result = this->new_object(c, isBuiltIn, NoneName, nullptr);
    result->typeId = NoneType;
    this->NoneInitialize(isBuiltIn)(c, result);
    result->set(Self, result);
    return result;
}

plasma::vm::value *
plasma::vm::virtual_machine::new_type(struct context &c, bool isBuiltIn, const std::string &name,
                                      const std::vector<plasma::vm::value *> &inheritedTypes,
                                      const struct constructor &constructor) {
    value *result = this->new_object(c, isBuiltIn, TypeName, nullptr);
    result->typeId = Type;
    result->constructor_ = constructor;
    result->name = name;
    this->TypeInitialize(isBuiltIn)(c, result);
    result->set(Self, result);
    return result;
}

plasma::vm::value *plasma::vm::virtual_machine::new_float(struct context &c, bool isBuiltIn, long double value_) {
    value * result = this->new_object(c, isBuiltIn, FloatName, nullptr);
    result->typeId = Float;
    result->floating = value_;
    this->FloatInitialize(isBuiltIn)(c, result);
    result->set(Self, result);
    return result;
}

plasma::vm::value *plasma::vm::virtual_machine::new_module(struct context &c, bool isBuiltIn) {
    value *result = this->new_object(c, isBuiltIn, ModuleName, nullptr);
    result->typeId = Module;
    result->set(Self, result);
    return result;
}

plasma::vm::value *plasma::vm::virtual_machine::new_bool(struct context &c, bool isBuiltIn, bool value_) {
    value *result = this->new_object(c, isBuiltIn, BoolName, nullptr);
    result->typeId = Boolean;
    result->boolean = value_;
    this->BoolInitialize(isBuiltIn)(c, result);
    result->set(Self, result);
    return result;
}

plasma::vm::value *plasma::vm::virtual_machine::new_integer(struct context &c, bool isBuiltIn, int64_t value_) {
    value *result = this->new_object(c, isBuiltIn, IntegerName, nullptr);
    result->typeId = Integer;
    result->integer = value_;
    this->IntegerInitialize(isBuiltIn)(c, result);
    result->set(Self, result);
    return result;
}

plasma::vm::value *plasma::vm::virtual_machine::new_string(struct context &c, bool isBuiltIn, const std::string &value_) {
    value *result = this->new_object(c, isBuiltIn, StringName, nullptr);
    result->typeId = String;
    result->string = value_;
    this->StringInitialize(isBuiltIn)(c, result);
    result->set(Self, result);
    return result;
}


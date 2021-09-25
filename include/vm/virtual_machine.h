

#ifndef PLASMA_VIRTUAL_MACHINE_H
#define PLASMA_VIRTUAL_MACHINE_H

#include <functional>
#include <cinttypes>
#include <any>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <stack>
#include <iostream>
#include <deque>

#include "plasma_error.h"
#include "memory.h"

using defer = std::shared_ptr<void>;

namespace plasma::vm {
    // Block states
    enum {
        NoState,
        Return,
        Break,
        Continue,
        Redo,
    };

    // OP Codes
    enum {
        NewStringOP, // 0
        NewBytesOP,
        NewIntegerOP,
        NewFloatOP,
        GetTrueOP,
        GetFalseOP,
        NewLambdaFunctionOP,
        GetNoneOP,

        // Composite creation
        NewTupleOP,
        NewArrayOP,
        NewHashOP, // 10
        NewGeneratorOP,

        // If conditions
        // FixMe:
        IfOP,
        UnlessOP,
        IfOneLinerOP,
        UnlessOneLinerOP,

        // Unary Expressions
        NegateBitsOP,
        BoolNegateOP,
        NegativeOP,

        // Binary Expressions
        AddOP,
        SubOP,
        MulOP,
        DivOP,
        FloorDivOP,
        ModOP,
        PowOP,
        BitXorOP, // 20
        BitAndOP,
        BitOrOP,
        BitLeftOP,
        BitRightOP,
        AndOP,
        OrOP,
        XorOP,
        EqualsOP,
        NotEqualsOP,
        GreaterThanOP, // 30
        LessThanOP,
        GreaterThanOrEqualOP,
        LessThanOrEqualOP,
        ContainsOP,
        UnaryOP,
        BinaryOP,
        // Other expressions
        GetIdentifierOP,
        IndexOP,
        SelectNameFromObjectOP,
        MethodInvocationOP, // 40

        // Assign Statement
        AssignIdentifierOP,
        AssignSelectorOP,
        AssignIndexOP,

        // Loops
        // FixMe:
        ForLoopOP,
        WhileLoopOP,
        DoWhileLoopOP,
        UntilLoopOP,

        BreakOP, // 50
        RedoOP,
        ContinueOP,

        ReturnOP,

        // Special Instructions
        LoadFunctionArgumentsOP,
        NewFunctionOP,
        PushOP,
        NOP,

        TryOP,

        NewModuleOP,
        NewClassOP, // 60
        NewInterfaceOP,
        NewClassFunctionOP,

        RaiseOP,
    };
    typedef std::function<struct value *()> on_demand_loader;
    // typedef struct value *(*on_demand_loader)();

    typedef std::function<struct value *(struct context *, struct virtual_machine *)> object_loader;
    // typedef struct value *(*object_loader)(context *c, struct virtual_machine *);

    typedef std::function<struct value *(struct context *, value *)> constructor_callback;
    // typedef struct value *(*constructor_callback)(context *c, struct value *);

    /*
     * - Returns the result when success
     * - Returns an error object when fails
     */
    typedef std::function<struct value *(struct value *, const std::vector<struct value *> &,
                                         bool *)> function_callback;
    // typedef struct value *(*function_callback)(struct value *self, std::vector<struct value *> arguments,
    //                                            bool *success);

    // Types
    const char TypeName[] = "Type";
    const char CallableName[] = "Callable";
    const char ObjectName[] = "Object";
    const char FunctionName[] = "Function";
    const char StringName[] = "String";
    const char BoolName[] = "Bool";
    const char TupleName[] = "Tuple";
    const char IntegerName[] = "Integer";
    const char FloatName[] = "Float";
    const char ArrayName[] = "Array";
    const char NoneName[] = "NoneType";
    const char BytesName[] = "Bytes";
    const char HashTableName[] = "HashTable";
    const char IteratorName[] = "Iterator";
    const char ModuleName[] = "Module";
    // Constants
    const char True[] = "True";
    const char False[] = "False";
    const char None[] = "None";
    // default symbols  names
    const char Self[] = "self";
    const char Initialize[] = "Initialize";
    const char NegBits[] = "NegBits";
    const char Negate[] = "Negate";
    const char Negative[] = "Negative";
    const char Add[] = "Add";
    const char RightAdd[] = "RightAdd";
    const char Sub[] = "Sub";
    const char RightSub[] = "RightSub";
    const char Mul[] = "Mul";
    const char RightMul[] = "RightMul";
    const char Div[] = "Div";
    const char RightDiv[] = "RightDiv";
    const char FloorDiv[] = "FloorDiv";
    const char RightFloorDiv[] = "RightFloorDiv";
    const char Mod[] = "Mod";
    const char RightMod[] = "RightMod";
    const char Pow[] = "Pow";
    const char RightPow[] = "RightPow";
    const char BitXor[] = "BitXor";
    const char RightBitXor[] = "RightBitXor";
    const char BitAnd[] = "BitAnd";
    const char RightBitAnd[] = "RightBitAnd";
    const char BitOr[] = "BitOr";
    const char RightBitOr[] = "RightBitOr";
    const char BitLeft[] = "BitLeft";
    const char RightBitLeft[] = "RightBitLeft";
    const char BitRight[] = "BitRight";
    const char RightBitRight[] = "RightBitRight";
    const char And[] = "And";
    const char RightAnd[] = "RightAnd";
    const char Or[] = "Or";
    const char RightOr[] = "RightOr";
    const char Xor[] = "Xor";
    const char RightXor[] = "RightXor";
    const char Equals[] = "Equals";
    const char RightEquals[] = "RightEquals";
    const char NotEquals[] = "NotEquals";
    const char RightNotEquals[] = "RightNotEquals";
    const char GreaterThan[] = "GreaterThan";
    const char RightGreaterThan[] = "RightGreaterThan";
    const char LessThan[] = "LessThan";
    const char RightLessThan[] = "RightLessThan";
    const char GreaterThanOrEqual[] = "GreaterThanOrEqual";
    const char RightGreaterThanOrEqual[] = "RightGreaterThanOrEqual";
    const char LessThanOrEqual[] = "LessThanOrEqual";
    const char RightLessThanOrEqual[] = "RightLessThanOrEqual";
    const char Contains[] = "Contains";
    const char RightContains[] = "RightContains";
    const char Hash[] = "Hash";
    const char Copy[] = "Copy";
    const char Index[] = "Index";
    const char Assign[] = "Assign";
    const char Call[] = "Call";
    const char Iter[] = "Iter";
    const char HasNext[] = "HasNext";
    const char Next[] = "Next";
    const char Class[] = "Class";
    const char SubClasses[] = "SubClasses";
    const char ToInteger[] = "ToInteger";
    const char ToFloat[] = "ToFloat";
    const char ToString[] = "ToString";
    const char ToBool[] = "ToBool";
    const char ToArray[] = "ToArray";
    const char ToTuple[] = "ToTuple";
    const char GetInteger[] = "GetInteger";
    const char GetBool[] = "GetBool";
    const char GetBytes[] = "GetBytes";
    const char GetString[] = "GetString";
    const char GetFloat[] = "GetFloat";
    const char GetContent[] = "GetContent";
    const char GetKeyValues[] = "GetKeyValues";
    const char SetBool[] = "SetBool";
    const char SetBytes[] = "SetBytes";
    const char SetString[] = "SetString";
    const char SetInteger[] = "SetInteger";
    const char SetFloat[] = "SetFloat";
    const char SetContent[] = "SetContent";
    const char SetKeyValues[] = "SetKeyValues";
    // Built-In errors names
    const char RuntimeError[] = "RuntimeError";
    const char InvalidTypeError[] = "InvalidTypeError";
    const char NotImplementedCallableError[] = "NotImplementedCallableError";
    const char ObjectConstructionError[] = "ObjectConstructionError";
    const char ObjectWithNameNotFoundError[] = "ObjectWithNameNotFoundError";
    const char InvalidNumberOfArgumentsError[] = "InvalidNumberOfArgumentsError";
    const char UnhashableTypeError[] = "UnhashableTypeError";
    const char IndexOutOfRangeError[] = "IndexOutOfRangeError";
    const char KeyNotFoundError[] = "KeyNotFoundError";
    const char IntegerParsingError[] = "IntegerParsingError";
    const char FloatParsingError[] = "FloatParsingError";
    const char BuiltInSymbolProtectionError[] = "BuiltInSymbolProtectionError";
    const char ObjectNotCallableError[] = "ObjectNotCallableError";
    // Type identifier
    enum {
        Array,
        Boolean,
        Bytes,
        Float,
        Function,
        HashTable,
        Integer,
        Iterator,
        Module,
        NoneType,
        Object,
        String,
        Tuple,
        Type
    };

    struct generator_information {
        size_t numberOfReceivers;
        size_t operationLength;
    };

    struct function_information {
        std::string name;
        size_t bodyLength;
        size_t numberOfArguments;
    };
    struct class_information {
        std::string name;
        size_t bodyLength;
        size_t numberOfBases;
    };

    struct for_loop_information {
        std::vector<std::string> receivers;
        size_t onFinishJump;
    };

    struct except_block_information {
        std::string captureName;
        size_t bodySize;
        size_t targetsSize;
    };

    struct try_block_information {
        size_t bodySize;
        std::vector<except_block_information> exceptBlocks;
        size_t finallySize;
    };

    struct instruction {
        uint8_t op_code;
        std::any value;
        size_t line;
    };

    struct condition_information {
        std::vector<instruction> body;
        std::vector<instruction> elseBody;
    };

    struct bytecode {
        std::vector<instruction> instructions;
        size_t index = 0;

        size_t length() const;

        [[nodiscard]] bool has_next() const;

        instruction peek();

        instruction next();

        std::vector<instruction> nextN(size_t n);

        void jump(size_t offset);

        void rjump(size_t offset);
    };

    struct symbol_table {
        // Garbage collector
        size_t pageIndex = SIZE_MAX;
        uint64_t count = 0;
        bool isSet = false;
        //
        symbol_table *parent = nullptr;
        std::unordered_map<std::string, value *> symbols;

        void set(const std::string &symbol, value *v);

        value *get_self(const std::string &symbol);

        value *get_any(const std::string &symbol);

        explicit symbol_table(symbol_table *parentSymbolTable);

        symbol_table();

        ~symbol_table();
    };

    struct key_value {
        value *key;
        value *value;
    };

    struct callable {
        bool isBuiltIn; // When is built-in the callback should be executed, if not, the code will be pushed to be executed
        size_t numberOfArguments;
        std::vector<instruction> code;
        function_callback callback;
    };

    callable new_builtin_callable(size_t number_of_arguments, function_callback callback);

    callable new_plasma_callable(size_t number_of_arguments, std::vector<instruction> code);


    struct constructor {
        bool isBuiltIn;
        constructor_callback callback;
        std::vector<instruction> code;

        /*
         * Construct the object approaching it's initializer
         * - Returns nullptr on success
         * - Returns an error object when fails
         */
        value *construct(context *c, virtual_machine *vm, value *self) const;
    };

    constructor new_plasma_constructor(const std::vector<instruction> &code);

    struct value {
        // Garbage collector
        size_t pageIndex;
        bool marked = false;
        bool isSet = false; // Used to  distinguish between values initialized
        //
        // Type Identifier
        uint8_t typeId = Object;
        // Type Related (used only when typeId is equal to Type)
        constructor constructor_;
        std::string name; // This is the name of the identifier that initially own this type
        // Function Related (used only when typeId is equal to Function)
        callable callable_;
        value *self = nullptr; // Also used as root by functions defined in classes and interfaces
        // Iterator Related
        value *source = nullptr; // Also used as root by iterators
        //
        bool isBuiltIn = false;
        int64_t id = 0;
        std::string typeName;
        value *type;
        std::vector<value *> subTypes;
        int64_t hash = 0; // Used by strings, integers and floats to cache the hash
        // Values
        std::string string;
        std::vector<uint8_t> bytes;
        std::vector<value *> content;
        std::unordered_map<int64_t, std::vector<key_value>> keyValues;
        bool boolean = false;
        double floating = 0;
        int64_t integer = 0;
        size_t iterIndex = 0;
        // Symbols
        symbol_table *symbols;
        std::unordered_map<std::string, on_demand_loader> onDemandSymbols;

        //
        void set_on_demand_symbol(const std::string &symbol, const on_demand_loader &loader);

        void set(const std::string &symbol, value *v) const;

        void set_symbols(symbol_table *symbolTable);

        /*
         * - Returns the requested object when success is true
         * - Returns an error object when the success is false
         */
        value *get(context *c, virtual_machine *vm, const std::string &symbol, bool *success);

        value *get_type(context *c, virtual_machine *vm) const;

        bool implements(context *c, virtual_machine *vm, value *type_);

        std::unordered_map<std::string, uint8_t> dir();

        /*
         * - Returns nullptr on success
         * - Return an error object when fails
         */
        value *add_key_value(context *c, virtual_machine *vm, value *key, value *v);
    };


    struct context {
        uint8_t lastState = NoState;
        std::deque<value *> objectsInUse;
        value *lastObject = nullptr;

        memory::memory<symbol_table> symbol_table_heap;
        memory::memory<value> value_heap;

        std::vector<value *> value_stack;
        std::vector<symbol_table *> symbol_table_stack;
        symbol_table *master = nullptr;

        explicit context(size_t initialPageLength);

        ~context();

        value *allocate_value();

        symbol_table *allocate_symbol_table(symbol_table *parentSymbolTable);

        void collect_values();

        void collect_symbol_tables();

        void push_value(value *v);

        value *peek_value();

        value *pop_value();

        void push_symbol_table(symbol_table *s);

        symbol_table *pop_symbol_table();

        symbol_table *peek_symbol_table();

        void protect_value(value *v);

        void restore_protected_state(size_t state);

        size_t protected_values_state() const;
    };

    struct virtual_machine {
        // Attributes
        uint64_t seed;
        uint64_t currentId = 1;
        std::istream &stdin_file;
        std::ostream &stdout_file;
        std::ostream &stderr_file;

        virtual_machine(std::istream &stdinFile,
                        std::ostream &stdoutFile,
                        std::ostream &stderrFile);

        // Initialization

        void load_builtin_object(context *c, const std::string &symbol, const object_loader &loader);

        void load_builtin_symbols(context *c, const std::unordered_map<std::string, object_loader> &symbols);

        void initialize_context(context *c);

        void initialize_builtin_symbols(context *c);

        uint64_t next_id();

        // Object creation tools
        value *construct_subtype(context *c, value *subType, value *self);

        value *construct_object(context *c, struct value *type, bool *success);

        // Function calls
        /*
         * - Returns the result on success
         * - Returns an error object when fails
         */
        struct value *call_function(context *c, struct value *function,
                                    const std::vector<struct value *> &arguments, bool *success);

        // Object Creators
        struct value *new_object(context *c, bool isBuiltIn, const std::string &typeName, value *type);

        struct value *new_hash_table(context *c, bool isBuiltIn);

        struct value *new_array(context *c, bool isBuiltIn, const std::vector<struct value *> &content);

        struct value *new_function(context *c, bool isBuiltIn, value *self, const struct callable &callable_);

        struct value *new_bytes(context *c, bool isBuiltIn, const std::vector<uint8_t> &bytes);

        struct value *new_iterator(context *c, bool isBuiltIn);

        struct value *new_tuple(context *c, bool isBuiltIn, const std::vector<struct value *> &content);

        struct value *new_none(context *c, bool isBuiltIn);

        struct value *
        new_type(context *c, bool isBuiltIn, const std::string &name,
                 const std::vector<struct value *> &inheritedTypes,
                 const struct constructor &constructor);

        value *new_float(context *c, bool isBuiltIn, double value_);

        value *new_module(context *c, bool isBuiltIn);

        value *new_bool(context *c, bool isBuiltIn, bool value_);

        value *new_integer(context *c, bool isBuiltIn, int64_t value_);

        value *new_string(context *c, bool isBuiltIn, const std::string &value_);

        // Error Creators
        value *NewFloatParsingError(context *c);

        value *new_integer_parsing_error(context *c);

        value *new_key_not_found_error(context *c, struct value *key);

        value *new_index_out_of_range_error(context *c, size_t length, size_t requestedIndex);

        value *new_unhashable_type_error(context *c, struct value *objectType);

        value *new_not_implemented_callable_error(context *c, std::string symbol);

        value *new_invalid_number_of_arguments_error(context *c, size_t expected, size_t received);

        value *
        new_object_with_name_not_found_error(context *c, struct value *source, const std::string &symbol);

        value *
        new_object_with_name_not_found_error(context *c, const std::string &symbol);

        value *
        new_invalid_type_error(context *c, struct value *receivedType,
                               const std::vector<std::string> &expectedTypes);

        value *new_object_construction_error(context *c, value *type, const std::string &errorMessage);

        value *
        new_builtin_symbol_protection_error(context *c, struct value *source, const std::string &symbol);

        struct value *new_object_not_callable_error(context *c, struct value *objectType);

        // Basic object caching
        struct value *get_none(context *c);

        struct value *get_false(context *c);

        struct value *get_true(context *c);

        struct value *get_boolean(context *c, bool condition);

        // Object Initializers
        constructor_callback runtime_error_initialize(bool isBuiltIn);

        constructor_callback float_parsing_error_initialize(bool isBuiltIn);

        constructor_callback integer_parsing_error_initialize(bool isBuiltIn);

        constructor_callback key_not_found_error_initialize(bool isBuiltIn);

        constructor_callback index_out_of_range_error_initialize(bool isBuiltIn);

        constructor_callback unhashable_type_error_initialize(bool isBuiltIn);

        constructor_callback not_implemented_callable_error_initialize(bool isBuiltIn);

        constructor_callback invalid_number_of_arguments_error_initialize(bool isBuiltIn);

        constructor_callback object_with_name_not_found_error_initialize(bool isBuiltIn);

        constructor_callback invalid_type_error_initialize(bool isBuiltIn);

        constructor_callback object_construction_error_initialize(bool isBuiltIn);

        constructor_callback builtin_symbol_protection_error_initialize(bool isBuiltIn);

        constructor_callback object_not_callable_error_initialize(bool isBuiltIn);


        constructor_callback callable_initialize(bool isBuiltIn);

        constructor_callback type_initialize(bool isBuiltIn);

        constructor_callback array_initialize(bool isBuiltIn);

        constructor_callback object_initialize(bool isBuiltIn);

        constructor_callback hash_table_initialize(bool isBuiltIn);

        constructor_callback tuple_initialize(bool isBuiltIn);

        constructor_callback iterator_initialize(bool isBuiltIn);

        constructor_callback bytes_initialize(bool isBuiltIn);

        constructor_callback string_initialize(bool isBuiltIn);

        constructor_callback integer_initialize(bool isBuiltIn);

        constructor_callback float_initialize(bool isBuiltIn);

        constructor_callback bool_initialize(bool isBuiltIn);

        constructor_callback none_initialize(bool isBuiltIn);

        // Force Operation
        struct value *force_get_from_source(context *c, const std::string &symbol, struct value *source);

        struct value *force_any_from_master(context *c, const std::string &symbol);

        struct value *force_construction(context *c, struct value *type_);

        void
        force_initialization(plasma::vm::context *c, struct value *object,
                             const std::vector<struct value *> &initArgument);

        // Code execution

        value *execute(context *c, bytecode *bc, bool *success);

        // Tools
        //// Content (Arrays and Tuples) related

        value *content_index(context *c, value *index, value *source, bool *success);

        value *content_assign(context *c, value *container, value *index, value *object, bool *success);

        value *content_to_string(context *c, value *container, bool *success);

        value *content_iterator(context *c, value *source);

        value *content_equals(context *c, value *leftHandSide, value *rightHandSide, bool *result);

        value *content_contains(context *c, value *container, value *object, bool *result);

        value *
        content_repeat(context *c, const std::vector<struct value *> &content, size_t times,
                       std::vector<struct value *> *result);

        //// Bytes

        value *bytes_index(context *c, value *source, value *index, bool *success);

        value *bytes_to_string(context *c, value *bytesObject);

        std::vector<value *> bytes_to_integer_content(context *c, value *bytes);

        value *bytes_iterator(context *c, value *bytes);

        static value *bytes_equals(value *leftHandSide, value *rightHandSide, bool *result);

        value *bytes_contains(context *c, value *bytes, value *subBytes, bool *result);

        static value *bytes_repeat(value *bytes, size_t times, std::vector<uint8_t> *result);

        //// Strings

        value *string_index(context *c, value *source, value *index, bool *success);

        std::vector<value *> string_to_integer_content(context *c, value *string);

        value *string_iterator(context *c, value *string);

        static value *string_equals(value *leftHandSide, value *rightHandSide, bool *result);

        static value *string_contains(value *string, value *subString, bool *result);

        static value *string_repeat(value *string, size_t times, std::string *result);

        //// HashTable

        value *hashtable_index(context *c, value *source, value *key, bool *success);

        value *hashtable_assign(context *c, value *source, value *key, value *object, bool *success);

        value *hashtable_iterator(context *c, value *hashtable);

        value *hashtable_to_string(context *c, value *hashtableObject, bool *success);

        static std::vector<value *> hashtable_to_content(value *hashTable);

        value *hashtable_equals(context *c, value *leftHandSide, value *rightHandSide, bool *result);

        value *hashtable_contains(context *c, value *hashTable, value *object, bool *success);

        value *hashtable_copy(context *c, value *hashTable, bool *success);

        //// Hashing

        int64_t hash_array(const std::vector<int64_t> &values);

        int64_t hash_string(const std::string &string);

        int64_t hash_bytes(const std::vector<uint8_t> &bytes);

        //// Any
        static size_t calculate_index(int64_t index, size_t length, bool *fail);

        value *equals(context *c, struct value *leftHandSide, struct value *rightHandSide, bool *result);

        value *calculate_hash(context *c, struct value *v, int64_t *hash_);

        value *interpret_as_boolean(context *c, struct value *v, bool *result);

        value *interpret_as_iterator(context *c, struct value *v, bool *success);

        value *unpack_values(context *c, struct value *b, size_t expect, std::vector<value *> *result);

        // Operations callbacks
        //// Object creation
        value *new_tuple_op(context *c, size_t numberOfElements);

        value *new_array_op(context *c, size_t numberOfElements);

        value *new_hash_op(context *c, size_t numberOfElements);

        value *new_string_op(context *c, const std::string &string);

        value *new_bytes_op(context *c, const std::string &bytes);

        value *new_integer_op(context *c, int64_t integer);

        value *new_float_op(context *c, double floating);

        value *new_function_op(context *c, bytecode *bc, const function_information &functionInformation);

        value *new_module_op(context *c, bytecode *bc, const class_information &moduleInformation);

        value *new_class_op(context *c, bytecode *bc, const class_information &classInformation);

        value *new_class_function_op(context *c, bytecode *bc, const function_information &functionInformation);

        value *new_lambda_function_op(context *c, bytecode *bc,
                                      const function_information &functionInformation);

        value *new_generator_op(context *c, bytecode *bc,
                                const generator_information &generatorInformation);

        //// Loop setup and operation

        value *for_loop_op(context *c);

        value *while_loop_op(context *c,
                             bytecode *bc,
                             const for_loop_information &forLoopInformation);

        value *do_while_loop_op(context *c,
                                bytecode *bc,
                                const for_loop_information &forLoopInformation);

        value *until_loop_op(context *c,
                             bytecode *bc,
                             const for_loop_information &forLoopInformation);

        //// Try blocks
        value *execute_try_block(context *c, bytecode *bc, const try_block_information &tryBlockInformation);

        value *raise_op(context *c);

        //// Conditions (if, unless and switch)
        value *if_op(context *c, const condition_information& conditionInformation);

        value *unless_op(context *c, const condition_information& conditionInformation);

        value *if_one_liner_op(context *c, const condition_information& conditionInformation);

        value *unless_one_liner_op(context *c, const condition_information& conditionInformation);

        value *unary_op(context *c, uint8_t instruction);

        value *binary_op(context *c, uint8_t instruction);

        //// Function calls
        static value *load_function_arguments_op(context *c, const std::vector<std::string> &arguments);

        value *method_invocation_op(context *c, size_t numberOfArguments);

        //// Symbol assign and request
        value *select_name_from_object_op(context *c, const std::string &identifier);

        value *get_identifier_op(context *c, const std::string &identifier);

        static value *assign_identifier_op(context *c, const std::string &symbol);

        static value *assign_selector_op(context *c, const std::string &symbol);

        //// Index assign and request
        value *assign_index_op(context *c);

        value *index_op(context *c);

        //// Function  return
        value *return_op(context *c, size_t numberOfReturnValues);
    };

}

#endif //PLASMA_VIRTUAL_MACHINE_H

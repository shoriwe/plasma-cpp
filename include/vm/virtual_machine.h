

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

#include "error.h"
#include "memory.h"

namespace plasma::vm {
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
        PowOP, // 20
        BitXorOP,
        BitAndOP,
        BitOrOP,
        BitLeftOP,
        BitRightOP,
        AndOP,
        OrOP,
        XorOP,
        EqualsOP,
        NotEqualsOP, // 30
        GreaterThanOP,
        LessThanOP,
        GreaterThanOrEqualOP,
        LessThanOrEqualOP,
        ContainsOP,
        UnaryOP,
        BinaryOP,
        // Other expressions
        GetIdentifierOP,
        IndexOP,
        SelectNameFromObjectOP, // 40
        MethodInvocationOP,

        // Assign Statement
        AssignIdentifierOP,
        AssignSelectorOP,
        AssignIndexOP,
        IfJumpOP,
        LoadForReloadOP,
        UnlessJumpOP,
        SetupLoopOP,
        PopLoopOP,
        UnpackForLoopOP, // 50
        BreakOP,
        RedoOP,
        ContinueOP,

        ReturnOP,

        // Special Instructions
        LoadFunctionArgumentsOP,
        NewFunctionOP,
        JumpOP,
        PushOP,
        PopOP,
        NOP, // 60
        NewIteratorOP,

        SetupTryOP,
        PopTryOP,
        ExceptOP,

        NewModuleOP,
        NewClassOP,
        NewClassFunctionOP,

        RaiseOP,
        CaseOP,
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

    struct FunctionInformation {
        std::string name;
        size_t bodyLength;
        size_t numberOfArguments;
    };
    struct ClassInformation {
        std::string name;
        size_t bodyLength;
        size_t numberOfBases;
    };

    struct instruction {
        uint8_t op_code;
        std::any value;
        size_t line;
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
    };

    struct symbol_table {
        // Garbage collector
        size_t pageIndex = SIZE_MAX;
        uint64_t count = 0;
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
        value *source; // Also used as root by iterators
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
        // Symbols
        symbol_table *symbols;
        std::unordered_map<std::string, on_demand_loader> onDemandSymbols;

        //
        void set_on_demand_symbol(const std::string &symbol, const on_demand_loader &loader);

        void set(const std::string &symbol, value *v) const;

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
        bool isMaster = false;
        value *lastObject = nullptr;
        memory::memory<symbol_table> symbol_table_heap;
        memory::memory<value> value_heap;
        std::vector<value *> value_stack;
        std::vector<symbol_table *> symbol_table_stack;
        symbol_table *master;
        // LoopStack   *LoopStack // ToDo:
        // TryStack    *TryStack // ToDo:

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
    };

    struct virtual_machine {
        // Attributes
        uint64_t seed;
        uint64_t currentId = 1;
        std::istream *stdin_file;
        std::ostream *stdout_file;
        std::ostream *stderr_file;

        virtual_machine(std::istream *stdinFile,
                        std::ostream *stdoutFile,
                        std::ostream *stderrFile);

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

        value *NewIntegerParsingError(context *c);

        value *NewKeyNotFoundError(context *c, struct value *key);

        value *NewIndexOutOfRange(context *c, size_t length, size_t requestedIndex);

        value *NewUnhashableTypeError(context *c, struct value *objectType);

        value *NewNotImplementedCallableError(context *c, std::string symbol);

        value *NewInvalidNumberOfArgumentsError(context *c, size_t expected, size_t received);

        value *
        NewObjectWithNameNotFoundError(context *c, struct value *source, const std::string &symbol);

        value *
        NewObjectWithNameNotFoundError(context *c, const std::string &symbol);

        value *
        NewInvalidTypeError(context *c, struct value *receivedType,
                            const std::vector<std::string> &expectedTypes);

        value *NewObjectConstructionError(context *c, value *type, const std::string &errorMessage);

        value *
        NewBuiltInSymbolProtectionError(context *c, struct value *source, const std::string &symbol);

        struct value *NewObjectNotCallableError(context *c, struct value *objectType);

        // Basic object caching
        struct value *get_none(context *c);

        struct value *get_false(context *c);

        struct value *get_true(context *c);

        struct value *get_boolean(context *c, bool condition);

        // Object Initializers
        struct value *RuntimeErrorInitialize(context *c, struct value *errorObject);

        constructor_callback CallableInitialize(bool isBuiltIn);

        constructor_callback TypeInitialize(bool isBuiltIn);

        constructor_callback ArrayInitialize(bool isBuiltIn);

        constructor_callback ObjectInitialize(bool isBuiltIn);

        constructor_callback HashTableInitialize(bool isBuiltIn);

        constructor_callback TupleInitialize(bool isBuiltIn);

        constructor_callback IteratorInitialize(bool isBuiltIn);

        constructor_callback BytesInitialize(bool isBuiltIn);

        constructor_callback StringInitialize(bool isBuiltIn);

        constructor_callback IntegerInitialize(bool isBuiltIn);

        constructor_callback FloatInitialize(bool isBuiltIn);

        constructor_callback BoolInitialize(bool isBuiltIn);

        constructor_callback NoneInitialize(bool isBuiltIn);

        // Force Operation
        struct value *force_get_from_source(context *c, const std::string &symbol, struct value *source);

        struct value *force_any_from_master(context *c, const std::string &symbol);

        struct value *force_construction(context *c, struct value *type_);

        void
        force_initialization(plasma::vm::context *c, struct value *object,
                             const std::vector<struct value *> &initArgument);

        // Code execution
        value *execute(bytecode *bc, bool *success);

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

        // Operations callbacks
        //// Object creation
        value *newTupleOP(context *c, size_t numberOfElements);

        value *newArrayOP(context *c, size_t numberOfElements);

        value *newHashOP(context *c, size_t numberOfElements);

        value *newStringOP(context *c, const std::string &string);

        value *newBytesOP(context *c, const std::string &bytes);

        value *newIntegerOP(context *c, int64_t integer);

        value *newFloatOP(context *c, double floating);

        value *newFunctionOP(context *c, bytecode *bc, instruction instruct);

        value *newIteratorOP(context *c, bytecode *bc, instruction instruct, value *);

        value *newModuleOP(context *c, bytecode *bc, instruction instruct);

        value *newClassOP(context *c, bytecode *bc, const ClassInformation &classInformation);

        value *newClassFunctionOP(context *c, bytecode *bc, const FunctionInformation &functionInformation);

        value *newLambdaFunctionOP(context *c, bytecode *bc, instruction instruct);

        //// Loop setup and operation
        value *setupForLoopOP(context *c, instruction instruct, value *);

        value *loadForLoopArguments(context *c, value *);

        // FixMe:
        // value *unpackForArguments(context *c, loopSettings *LoopSettings, result Value, value *);

        value *unpackForLoopOP(context *c, bytecode *bc, value *);

        //// Try blocks
        value *setupTryOP(context *c, bytecode *bc, instruction instruct);

        value *popTryOP(context *c);

        value *exceptOP(context *c, bytecode *bc, instruction instruct);

        value *tryJumpOP(context *c, bytecode *bc);

        value *raiseOP(context *c, value *);

        //// Conditions (if, unless and switch)
        value *caseOP(context *c, bytecode *bc, instruction instruct, value *);

        value *ifJumpOP(context *c, bytecode *bc, instruction instruct, value *);

        value *unlessJumpOP(context *c, bytecode *bc, instruction instruct, value *);

        value *unaryOP(context *c, uint8_t instruction);

        value *binaryOP(context *c, uint8_t instruction);

        //// Function calls
        static value *loadFunctionArgumentsOP(context *c, const std::vector<std::string> &arguments);

        value *noArgsGetAndCall(context *c, std::string, value *);

        value *methodInvocationOP(context *c, size_t numberOfArguments);

        //// Symbol assign and request
        value *selectNameFromObjectOP(context *c, const std::string &identifier);

        value *getIdentifierOP(context *c, const std::string &identifier);

        static value *assignIdentifierOP(context *c, const std::string &symbol);

        static value *assignSelectorOP(context *c, const std::string &symbol);

        //// Index assign and request
        value *assignIndexOP(context *c);

        value *indexOP(context *c);

        //// Function  return
        value *returnOP(context *c, size_t numberOfReturnValues);
    };

}

#endif //PLASMA_VIRTUAL_MACHINE_H

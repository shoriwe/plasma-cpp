

#ifndef PLASMA_VIRTUAL_MACHINE_H
#define PLASMA_VIRTUAL_MACHINE_H

#include <cinttypes>
#include <any>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

#include "error.h"
#include "memory.h"

namespace plasma::vm {
    typedef struct value *(*on_demand_loader)();

    typedef struct value *(*object_loader)(struct context &c, struct virtual_machine &);

    typedef struct value *(*constructor_callback)(struct context &c, struct value *);

    /*
     * - Returns the result when success
     * - Returns an error object when fails
     */
    typedef struct value *(*function_callback)(struct value *self, std::vector<struct value *> arguments,
                                               bool *success);

    //
    const char TypeName[] = "Type";
    const char ObjectName[] = "Object";
    const char FunctionName[] = "Function";
    const char StringName[] = "String";
    const char BoolName[] = "Bool";
    const char TrueName[] = "True";
    const char FalseName[] = "False";
    const char TupleName[] = "Tuple";
    const char IntegerName[] = "Integer";
    const char FloatName[] = "Float";
    const char ArrayName[] = "Array";
    const char NoneName[] = "NoneType";
    const char BytesName[] = "Bytes";
    const char HashName[] = "Hash";
    const char IteratorName[] = "Iterator";
    const char ModuleName[] = "Module";
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
    const char GetLength[] = "GetLength";
    const char SetBool[] = "SetBool";
    const char SetBytes[] = "SetBytes";
    const char SetString[] = "SetString";
    const char SetInteger[] = "SetInteger";
    const char SetFloat[] = "SetFloat";
    const char SetContent[] = "SetContent";
    const char SetKeyValues[] = "SetKeyValues";
    const char SetLength[] = "SetLength";
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

    struct instruction {
        uint8_t op_code;
        std::any value;
        size_t line;
    };

    struct bytecode {
        std::vector<instruction> instructions;
        size_t index = 0;

        explicit bytecode(std::vector<instruction> instructs);

        size_t length();

        [[nodiscard]] bool has_next() const;

        instruction peek();

        instruction next();

        std::vector<instruction> nextN(size_t n);

        void jump(size_t offset);
    };

    struct virtual_machine {
        // Attributes
        uint64_t seed;
        uint64_t currentId;
        context &masterContext;
        std::basic_ifstream<unsigned char> stdin_file;
        std::basic_ifstream<unsigned char> stdout_file;
        std::basic_ifstream<unsigned char> stderr_file;
        struct symbol_table *builtInSymbolTable;

        // Methods
        int64_t hash_array(const std::vector<int64_t> &values);

        int64_t hash_string(const std::string &string);

        int64_t hash_bytes(const std::vector<int8_t> &bytes);

        void load_builtin_object(const std::string &, object_loader);

        void load_builtin_symbols(std::unordered_map<std::string, object_loader>);

        // IO related

        [[nodiscard]] struct symbol_table *built_in_symbols() const;

        int64_t next_id();

        /*
         * Prepare the context to be used  by the virtual machine
         */
        void InitializeContext(struct context &);

        /*
         * Initialize the vm built in symbol just once
         */
        void InitializeBuiltIn();

        // Object creation tools
        bool construct_subtype(struct context &c, struct value *, struct value *, struct value *);

        bool construct_object(struct context &c, struct value *, struct symbol_table *, struct value *, struct value *);

        // Function calls
        /*
         * - Returns the result on success
         * - Returns an error object when fails
         */
        struct value *call_function(struct context &c, struct value *function,
                                    const std::vector<struct value *> &arguments, bool *success);

        // Object Creators
        struct value *NewObject(struct context &c, bool isBuiltIn, const std::string &typeName);

        struct value *NewHashTable(struct context &c, bool isBuiltIn);

        struct value *NewArray(struct context &c, bool isBuiltIn, std::vector<struct value *> content);

        struct value *NewFunction(struct context &c, bool isBuiltIn, struct callable callable_);

        struct value *NewBytes(struct context &c, bool isBuiltIn, std::vector<uint8_t> bytes);

        struct value *NewIterator(struct context &c, bool isBuiltIn);

        struct value *NewTuple(struct context &c, bool isBuiltIn, std::vector<struct value *> content);

        struct value *NewNone(struct context &c, bool isBuiltIn);

        struct value *
        NewType(struct context &c, bool isBuiltIn, const std::string &name, std::vector<struct value *> inheritedTypes,
                struct constructor constructor);

        struct value *NewFloat(struct context &c, bool isBuiltIn, long double value);

        struct value *NewModule(struct context &c, bool isBuiltIn);

        struct value *NewBool(struct context &c, bool isBuiltIn, bool value);

        struct value *NewInteger(struct context &c, bool isBuiltIn, int64_t value);

        struct value *NewString(struct context &c, bool isBuiltIn, std::string value);

        // Error Creators
        struct value *NewFloatParsingError(struct context &c);

        struct value *NewIntegerParsingError(struct context &c);

        struct value *NewKeyNotFoundError(struct context &c, struct value *key);

        struct value *NewIndexOutOfRange(struct context &c, size_t length, size_t requestedIndex);

        struct value *NewUnhashableTypeError(struct context &c, struct value *objectType);

        struct value *NewNotImplementedCallableError(struct context &c, std::string symbol);

        struct value *NewRuntimeError(struct context &c, error::error *err);

        struct value *NewInvalidNumberOfArgumentsError(struct context &c, size_t expected, size_t received);

        struct value *
        NewObjectWithNameNotFoundError(struct context &c, struct value *source, const std::string &symbol);

        struct value *
        NewInvalidTypeError(struct context &c, struct value *receivedType, std::vector<std::string> expectedTypes);

        struct value *NewObjectConstructionError(struct context &c, const std::string &, std::string);

        struct value *NewBuiltInSymbolProtectionError(struct context &c, struct value *source, std::string symbol);

        struct value *NewObjectNotCallable(struct context &c, struct value *objectType);

        // Basic object caching
        struct value *GetNone();

        struct value *GetFalse();

        struct value *GetTrue();

        // Object Initializers
        struct value *RuntimeErrorInitialize(struct context &c, struct value *errorObject);

        constructor_callback CallableInitialize(bool isBuiltIn);

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
        struct value *ForceGetSelf(struct context &c, const std::string &symbol, struct value *source);

        struct value *ForceMasterGetAny(const std::string &symbol);

        struct value *ForceConstruction(struct context &c, struct value *type_);

        void
        ForceInitialization(struct context &c, struct value *object, const std::vector<struct value *> &initArgument);

        // Code execution
        struct value *Execute(struct context &c, bytecode *bc, bool *success);

        // Tools
        value *equals(struct context &c, struct value *leftHandSide, struct value *rightHandSide, bool *result);

        value *calculate_hash(struct context &c, struct value *v, int64_t *hash_);

        struct value *
        repeat(struct context &c, const std::vector<struct value *> &content, size_t times,
               std::vector<struct value *> *result);

        struct value *quickGetBool(struct context &c, struct value *v, bool *result);

        // Operations callbacks
        //// Object creation
        struct value *newTupleOP(struct context &c, instruction instruct, struct value *);

        struct value *newArrayOP(struct context &c, instruction instruct, struct value *);

        struct value *newHashOP(struct context &c, instruction instruct, struct value *);

        struct value *newStringOP(struct context &c, instruction instruct, struct value *);

        struct value *newBytesOP(struct context &c, instruction instruct, struct value *);

        struct value *newIntegerOP(struct context &c, instruction instruct, struct value *);

        struct value *newFloatOP(struct context &c, instruction instruct, struct value *);

        struct value *newFunctionOP(struct context &c, bytecode *bc, instruction instruct, struct value *);

        struct value *
        newIteratorOP(struct context &c, bytecode *bc, instruction instruct, struct value *, struct value *);

        struct value *newModuleOP(struct context &c, bytecode *bc, instruction instruct, struct value *);

        struct value *newClassOP(struct context &c, bytecode *bc, instruction instruct, struct value *);

        struct value *newClassFunctionOP(struct context &c, bytecode *bc, instruction instruct, struct value *);

        struct value *newLambdaFunctionOP(struct context &c, bytecode *bc, instruction instruct, struct value *);

        //// Object request
        struct value *newTrueBoolOP(struct value *);

        struct value *newFalseBoolOP(struct value *);

        struct value *getNoneOP(struct value *);

        //// Loop setup and operation
        struct value *setupForLoopOP(struct context &c, instruction instruct, struct value *);

        struct value *loadForLoopArguments(struct context &c, struct value *);

        struct value *unpackForArguments(struct context &c, loopSettings *LoopSettings, result Value, struct value *);

        struct value *unpackForLoopOP(struct context &c, bytecode *bc, struct value *);

        //// Try blocks
        struct value *setupTryOP(struct context &c, bytecode *bc, instruction instruct);

        struct value *popTryOP(struct context &c);

        struct value *exceptOP(struct context &c, bytecode *bc, instruction instruct);

        struct value *tryJumpOP(struct context &c, bytecode *bc);

        struct value *raiseOP(struct context &c, struct value *);

        //// Conditions (if, unless and switch)
        struct value *caseOP(struct context &c, bytecode *bc, instruction instruct, struct value *);

        struct value *ifJumpOP(struct context &c, bytecode *bc, instruction instruct, struct value *);

        struct value *unlessJumpOP(struct context &c, bytecode *bc, instruction instruct, struct value *);

        //// Binary, unary and parentheses operations
        struct value *newParenthesesOP(struct context &c, struct value *);

        struct value *leftBinaryExpressionFuncCall(struct context &c, std::string, struct value *);

        struct value *
        rightBinaryExpressionFuncCall(struct context &c, struct value *, struct value *, std::string, struct value *,
                                      struct value *);

        //// Function calls
        struct value *loadFunctionArgumentsOP(struct context &c, instruction instruct, struct value *);

        struct value *noArgsGetAndCall(struct context &c, std::string, struct value *);

        struct value *methodInvocationOP(struct context &c, instruction instruct, struct value *, struct value *);

        //// Symbol assign and request
        struct value *selectNameFromObjectOP(struct context &c, instruction instruct, struct value *, struct value *);

        struct value *getIdentifierOP(struct context &c, instruction instruct, struct value *, struct value *);

        struct value *assignIdentifierOP(struct context &c, instruction instruct, struct value *);

        struct value *assignSelectorOP(struct context &c, instruction instruct, struct value *);

        //// Index assign and request
        struct value *assignIndexOP(struct context &c, struct value *);

        struct value *indexOP(struct context &c, struct value *, struct value *);

        //// Function  return
        struct value *returnOP(struct context &c, instruction instruct, struct value *);

        //// other control flow related
        struct value *jumpOP(bytecode *bc, instruction instruct, struct value *);
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

    struct constructor {
        bool isBuiltIn;
        constructor_callback callback;
        std::vector<instruction> code;

        /*
         * Construct the object approaching it's initializer
         * - Returns nullptr on success
         * - Returns an error object when fails
         */
        value *construct(struct context &c, virtual_machine *vm, value *self) const;
    };

    struct symbol_table {
        // Garbage collector
        size_t pageIndex;
        uint64_t count = 0;
        //
        symbol_table *parent = nullptr;
        std::unordered_map<std::string, value *> symbols;

        void set(const std::string &symbol, value *v);

        value *get_self(const std::string &symbol);

        value *get_any(const std::string &symbol);

        explicit symbol_table(symbol_table *parentSymbolTable);

        ~symbol_table();
    };

    struct value {
        // Garbage collector
        size_t pageIndex;
        value *self = nullptr; // Used as root by functions defined in classes and interfaces
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
        long double floating = 0;
        int64_t integer = 0;
        // Symbols
        symbol_table *symbols;
        std::unordered_map<std::string, on_demand_loader> onDemandSymbols;

        //
        void set_on_demand_symbol(const std::string &symbol, on_demand_loader loader);

        void set(const std::string &symbol, value *v) const;

        /*
         * - Returns the requested object when success is true
         * - Returns an error object when the success is false
         */
        value *get(struct context &c, virtual_machine *vm, const std::string &symbol, bool *success);

        value *get_type(virtual_machine *vm) const;

        bool implements(virtual_machine *vm, value *type_);

        std::unordered_map<std::string, uint8_t> dir();

        /*
         * - Returns nullptr on success
         * - Return an error object when fails
         */
        value *add_key_value(struct context &c, virtual_machine *vm, value *key, value *v);
    };

    struct context {
        memory::memory<symbol_table> symbol_table_heap;
        memory::memory<value> value_heap;
        std::vector<value *> value_stack;
        std::vector<symbol_table *> symbol_table_stack;
        // LoopStack   *LoopStack // ToDo:
        // TryStack    *TryStack // ToDo:

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
}

#endif //PLASMA_VIRTUAL_MACHINE_H

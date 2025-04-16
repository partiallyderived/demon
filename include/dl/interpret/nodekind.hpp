#pragma once

#include <ostream>

namespace dl {

enum class NodeKind {
    ADD,
    ADDR,
    ADDR_TYPE,
    AND,
    ARG_DEF,
    ARGS,
    AS,
    ASSIGN,
    BAND,
    BNOT,
    BLOCK,
    BOOL,
    BOR,
    BREAK,
    BXOR,
    CACHED_CALL,
    CACHED_CALL_ATTR,
    CALL,
    CALL_ATTR,
    CASE,
    CHAR,
    CONTINUE,
    DECLARE,
    DEF,
    DEF_CASE,
    DEFAULTED,
    DIV,
    ENTER_OBJECT,
    ENTRY,
    ERROR,
    ERROR_WITH_COMP,
    EQ,
    EXPANSION,
    FLOAT32,
    FLOAT64,
    FOR,
    FOR_VARS,
    GET_ATTR,
    GT,
    GTE,
    IADD,
    IBAND,
    IBOR,
    IBXOR,
    ID,
    IDIV,
    IF,
    ILSH,
    IMOD,
    IMUL,
    IN,
    INIT,
    INT8,
    INT16,
    INT32,
    INT64,
    IPOW,
    IRSH,
    ISUB,
    KWARG,
    LAMBDA,
    LAMBDA_ARGS,
    LAMBDA_KWARGS,
    LAMBDA_VAR,
    LIST,
    LSH,
    LT,
    LTE,
    MAP,
    MATCH,
    MATCH_ARGS,
    MATCH_CASE,
    MATCH_KWARG,
    MATCH_LIST,
    MATCH_MAP,
    MATCH_OBJECT,
    MATCH_OPTIONS,
    MATCH_TUPLE,
    MOD,
    MUL,
    NEG,
    NEQ,
    NONE,
    NOT,
    NULL_,
    NUM_ID,
    OR,
    PLACEHOLDER,
    POW,
    RAISE,
    RAW_FLOAT32,
    RAW_FLOAT64,
    RAW_INT8,
    RAW_INT16,
    RAW_INT32,
    RAW_INT64,
    RAW_UINT8,
    RAW_UINT16,
    RAW_UINT32,
    RAW_UINT64,
    RETURN,
    RSH,
    SEQ,
    SET,
    SET_ATTR,
    SLICE,
    SUB,
    STRING,
    SYMBOL,
    TERNARY,
    THIS,
    TUPLE,
    TRY,
    TYPE,
    TYPE_MATCH,
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    UNEXPECTED_BLOCK,
    UNEXPECTED_CASE,
    UNEXPECTED_CONSTRUCT,
    UPDATE,
    UPDATE_ATTR,
    VAR,
    WHILE
};

std::ostream& operator<<(std::ostream& os, NodeKind x) {
    using enum NodeKind;

    switch(x) {
    case ADD:
        return os << "ADD";
    case ADDR:
        return os << "ADDR";
    case ADDR_TYPE:
        return os << "ADDR_TYPE";
    case AND:
        return os << "AND";
    case ARG_DEF:
        return os << "ARG_DEF";
    case ARGS:
        return os << "ARGS";
    case AS:
        return os << "AS";
    case ASSIGN:
        return os << "ASSIGN";
    case BAND:
        return os << "BAND";
    case BLOCK:
        return os << "BLOCK";
    case BNOT:
        return os << "BNOT";
    case BOOL:
        return os << "BOOL";
    case BOR:
        return os << "BOR";
    case BREAK:
        return os << "BREAK";
    case BXOR:
        return os << "BXOR";
    case CACHED_CALL:
        return os << "CACHED_CALL";
    case CACHED_CALL_ATTR:
        return os << "CACHED_CALL_ATTR";
    case CALL:
        return os << "CALL";
    case CALL_ATTR:
        return os << "CALL_ATTR";
    case CASE:
        return os << "CASE";
    case CHAR:
        return os << "CHAR";
    case CONTINUE:
        return os << "CONTINUE";
    case DECLARE:
        return os << "DECLARE";
    case DEF:
        return os << "DEF";
    case DEF_CASE:
        return os << "DEF_CASE";
    case DEFAULTED:
        return os << "DEFAULTED";
    case DIV:
        return os << "DIV";
    case ENTER_OBJECT:
        return os << "ENTER_OBJECT";
    case ENTRY:
        return os << "ENTRY";
    case ERROR:
        return os << "ERROR";
    case ERROR_WITH_COMP:
        return os << "ERROR_WITH_COMP";
    case EQ:
        return os << "EQ";
    case EXPANSION:
        return os << "EXPANSION";
    case FLOAT32:
        return os << "FLOAT32";
    case FLOAT64:
        return os << "FLOAT64";
    case FOR:
        return os << "FOR";
    case FOR_VARS:
        return os << "FOR_VARS";
    case GET_ATTR:
        return os << "GET_ATTR";
    case GT:
        return os << "GT";
    case GTE:
        return os << "GTE";
    case IADD:
        return os << "IADD";
    case IBAND:
        return os << "IBAND";
    case IBOR:
        return os << "IBOR";
    case IBXOR:
        return os << "IBXOR";
    case ID:
        return os << "ID";
    case IDIV:
        return os << "IDIV";
    case IF:
        return os << "IF";
    case ILSH:
        return os << "ILSH";
    case IMOD:
        return os << "IMOD";
    case IMUL:
        return os << "IMUL";
    case IN:
        return os << "IN";
    case INIT:
        return os << "INIT";
    case INT8:
        return os << "INT8";
    case INT16:
        return os << "INT16";
    case INT32:
        return os << "INT32";
    case INT64:
        return os << "INT64";
    case IPOW:
        return os << "IPOW";
    case IRSH:
        return os << "IRSH";
    case ISUB:
        return os << "ISUB";
    case KWARG:
        return os << "KWARG";
    case LAMBDA:
        return os << "LAMBDA";
    case LAMBDA_ARGS:
        return os << "LAMBDA_ARGS";
    case LAMBDA_KWARGS:
        return os << "LAMBDA_KWARGS";
    case LAMBDA_VAR:
        return os << "LAMBDA_VAR";
    case LIST:
        return os << "LIST";
    case LSH:
        return os << "LSH";
    case LT:
        return os << "LT";
    case LTE:
        return os << "LTE";
    case MAP:
        return os << "MAP";
    case MATCH:
        return os << "MATCH";
    case MATCH_ARGS:
        return os << "MATCH_ARGS";
    case MATCH_CASE:
        return os << "MATCH_CASE";
    case MATCH_KWARG:
        return os << "MATCH_KWARG";
    case MATCH_LIST:
        return os << "MATCH_LIST";
    case MATCH_MAP:
        return os << "MATCH_MAP";
    case MATCH_OBJECT:
        return os << "MATCH_OBJECT";
    case MATCH_OPTIONS:
        return os << "MATCH_OPTIONS";
    case MATCH_TUPLE:
        return os << "MATCH_TUPLE";
    case MOD:
        return os << "MOD";
    case MUL:
        return os << "MUL";
    case NEG:
        return os << "NEG";
    case NEQ:
        return os << "NEQ";
    case NONE:
        return os << "NONE";
    case NOT:
        return os << "NOT";
    case NULL_:
        return os << "NULL_";
    case NUM_ID:
        return os << "NUM_ID";
    case OR:
        return os << "OR";
    case PLACEHOLDER:
        return os << "PLACEHOLDER";
    case POW:
        return os << "POW";
    case RAISE:
        return os << "RAISE";
    case RAW_FLOAT32:
        return os << "RAW_FLOAT32";
    case RAW_FLOAT64:
        return os << "RAW_FLOAT64";
    case RAW_INT8:
        return os << "RAW_INT8";
    case RAW_INT16:
        return os << "RAW_INT16";
    case RAW_INT32:
        return os << "RAW_INT32";
    case RAW_INT64:
        return os << "RAW_INT64";
    case RAW_UINT8:
        return os << "RAW_UINT8";
    case RAW_UINT16:
        return os << "RAW_UINT16";
    case RAW_UINT32:
        return os << "RAW_UINT32";
    case RAW_UINT64:
        return os << "RAW_UINT64";
    case RETURN:
        return os << "RETURN";
    case RSH:
        return os << "RSH";
    case SEQ:
        return os << "SEQ";
    case SET:
        return os << "SET";
    case SET_ATTR:
        return os << "SET_ATTR";
    case SLICE:
        return os << "SLICE";
    case SUB:
        return os << "SUB";
    case STRING:
        return os << "STRING";
    case SYMBOL:
        return os << "SYMBOL";
    case TERNARY:
        return os << "TERNARY";
    case THIS:
        return os << "THIS";
    case TUPLE:
        return os << "TUPLE";
    case TRY:
        return os << "TRY";
    case TYPE:
        return os << "TYPE";
    case TYPE_MATCH:
        return os << "TYPE_MATCH";
    case UINT8:
        return os << "UINT8";
    case UINT16:
        return os << "UINT16";
    case UINT32:
        return os << "UINT32";
    case UINT64:
        return os << "UINT64";
    case UNEXPECTED_BLOCK:
        return os << "UNEXPECTED_BLOCK";
    case UNEXPECTED_CASE:
        return os << "UNEXPECTED_CASE";
    case UNEXPECTED_CONSTRUCT:
        return os << "UNEXPECTED_CONSTRUCT";
    case UPDATE:
        return os << "UPDATE";
    case UPDATE_ATTR:
        return os << "UPDATE_ATTR";
    case VAR:
        return os << "VAR";
    case WHILE:
        return os << "WHILE";
    }
};

}

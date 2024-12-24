#pragma once

#include <ostream>

namespace dl {

enum class NodeID {
    ADDR,
    ADDR_TYPE,
    AND,
    ARG_CONST,
    ARG_DEF,
    ARGS,
    ARG_SPEC,
    BOOL,
    BLOCK,
    BREAK,
    CACHED_CALL,
    CACHED_CALL_ATTR,
    CALL,
    CALL_ATTR,
    CHAR,
    CONTINUE,
    DEF,
    DEF_CASE,
    EXCEPT,
    EXPANSION,
    FOR,
    FOR_EACH,
    GET_ATTR,
    ID,
    IF,
    INT8,
    INT16,
    INT32,
    INT64,
    KEYWORD_ARG,
    LIST,
    MATCH,
    MATCH_CASE,
    NONE,
    NOT,
    NULL_,
    OBJ_VAR,
    OR,
    RAISE,
    RETURN,
    SET,
    SET_ATTR,
    STRING,
    SYMBOL,
    TERNARY,
    THIS,
    TRY,
    TUPLE,
    TYPE,
    TYPE_CHECK,
    TYPE_MATCH,
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    UPDATE,
    UPDATE_ATTR,
    VAR,
    VAR_ARGS_DEF,
    VAR_KWARGS_DEF,
    WHILE
};

std::ostream& operator<<(std::ostream& os, NodeID id) {
    switch(id) {
    case ADDR:
        return os << "ADDR";
    case ADDR_TYPE:
        return os << "ADDR_TYPE";
    case AND:
        return os << "AND";
    case ARG_CONST:
        return os << "ARG_CONST";
    case ARG_DEF:
        return os << "ARG_DEF";
    case ARGS:
        return os << "ARGS";
    case ARG_SPEC:
        return os << "ARG_SPEC";
    case BOOL:
        return os << "BOOL";
    case BLOCK:
        return os << "BLOCK";
    case BREAK:
        return os << "BREAK";
    case CACHED_CALL:
        return os << "CACHED_CALL";
    case CACHED_CALL_ATTR:
        return os << "CACHED_CALL_ATTR";
    case CALL:
        return os << "CALL";
    case CALL_ATTR:
        return os << "CALL_ATTR";
    case CHAR:
        return os << "CHAR";
    case CONTINUE:
        return os << "CONTINUE";
    case DEF:
        return os << "DEF";
    case DEF_CASE:
        return os << "DEF_CASE";
    case EXCEPT:
        return os << "EXCEPT";
    case EXPANSION:
        return os << "EXPANSION";
    case FOR:
        return os << "FOR";
    case FOR_EACH:
        return os << "FOR_EACH";
    case GET_ATTR:
        return os << "GET_ATTR";
    case ID:
        return os << "ID";
    case IF:
        return os << "IF";
    case INT8:
        return os << "INT8";
    case INT16:
        return os << "INT16";
    case INT32:
        return os << "INT32";
    case INT64:
        return os << "INT64";
    case KEYWORD_ARG:
        return os << "KEYWORD_ARG";
    case LIST:
        return os << "LIST";
    case MATCH:
        return os << "MATCH";
    case MATCH_CASE:
        return os << "MATCH_CASE";
    case NONE:
        return os << "NONE";
    case NOT:
        return os << "NOT";
    case NULL_:
        return os << "NULL";
    case OBJ_VAR:
        return os << "OBJ_VAR";
    case OR:
        return os << "OR";
    case RAISE:
        return os << "RAISE";
    case RETURN:
        return os << "RETURN";
    case SET:
        return os << "SET";
    case SET_ATTR:
        return os << "SET_ATTR";
    case STRING:
        return os << "STRING";
    case SYMBOL:
        return os << "SYMBOL";
    case TERNARY:
        return os << "TERNARY";
    case THIS:
        return os << "THIS";
    case TRY:
        return os << "TRY";
    case TUPLE:
        return os << "TUPLE";
    case TYPE:
        return os << "TYPE";
    case TYPE_CHECK:
        return os << "TYPE_CHECK";
    case UINT8:
        return os << "UINT8";
    case UINT16:
        return os << "UINT16";
    case UINT32:
        return os << "UINT32";
    case UINT64:
        return os << "UINT64";
    case UPDATE:
        return os << "UPDATE";
    case UPDATE_ATTR:
        return os << "UPDATE_ATTR";
    case VAR:
        return os << "VAR";
    case VAR_ARGS_DEF:
        return os << "VAR_ARGS_DEF";
    case VAR_KWARGS_DEF:
        return os << "VAR_KWARGS_DEF";
    case WHILE:
        return os << "WHILE";
    }
}

}

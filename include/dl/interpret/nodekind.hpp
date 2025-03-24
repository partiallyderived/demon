#pragma once

#include <ostream>

namespace dl {

enum class NodeKind {
    ADDR,
    ADDR_TYPE,
    AND,
    ARG_DEF,
    ARGS,
    AS,
    ASSIGN,
    BOOL,
    BREAK,
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
    ENTRY,
    EXPANSION,
    FLOAT32,
    FLOAT64,
    FOR,
    GET_ATTR,
    ID,
    IF,
    IN,
    INIT,
    INT8,
    INT16,
    INT32,
    INT64,
    KWARG,
    LAMBDA,
    LAMBDA_ARGS,
    LAMBDA_KWARGS,
    LAMBDA_VAR,
    LIST,
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
    NONE,
    NOT,
    NULL_,
    NUM_ID,
    OR,
    PLACEHOLDER,
    RAISE,
    RETURN,
    SEQ,
    SET,
    SET_ATTR,
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
    UPDATE,
    UPDATE_ATTR,
    VAR,
    WHILE
};

std::ostream& operator<<(std::ostream& os, NodeKind x) {
    using enum NodeKind;

    switch(x) {
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
    case BOOL:
        return os << "BOOL";
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
    case ENTRY:
        return os << "ENTRY";
    case EXPANSION:
        return os << "EXPANSION";
    case FLOAT32:
        return os << "FLOAT32";
    case FLOAT64:
        return os << "FLOAT64";
    case FOR:
        return os << "FOR";
    case GET_ATTR:
        return os << "GET_ATTR";
    case ID:
        return os << "ID";
    case IF:
        return os << "IF";
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
    case NONE:
        return os << "NONE";
    case NOT:
        return os << "NOT";
    case NULL_:
        return os << "NULL";
    case NUM_ID:
        return os << "NUM_ID";
    case OR:
        return os << "OR";
    case PLACEHOLDER:
        return os << "PLACEHOLDER";
    case RAISE:
        return os << "RAISE";
    case RETURN:
        return os << "RETURN";
    case SEQ:
        return os << "SEQ";
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
    case UPDATE:
        return os << "UPDATE";
    case UPDATE_ATTR:
        return os << "UPDATE_ATTR";
    case VAR:
        return os << "VAR";
    case WHILE:
        return os << "WHILE";
    }
}

}

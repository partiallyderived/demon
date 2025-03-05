#pragma once

#include <ostream>

namespace dl {

enum class NodeCategory {
    BOOL,
    INT8,
    INT16,
    INT32,
    INT64,
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    FLOAT32,
    FLOAT64,
    CHAR,
    STRING,
    ID,
    NUM_ID,
    INIT,
    UPDATE,
    CALL_ATTR,
    SET_ATTR,
    UPDATE_ATTR,
    KEYWORD_ARG,
    TERNARY,
    VAR,
    LAMBDA_VAR,
    LAMBDA_ARGS,
    LAMBDA_KWARGS,
    NULLARY,
    UNARY,
    BINARY,
    SEQ,
    CALL,
    DEF,
    FOR,
    IF,
    MATCH,
    TRY,
    WHILE,
    TYPE
};

std::ostream& operator<<(std::ostream& os, NodeCategory x) {
    using enum NodeCategory;

    switch(x) {
    case BOOL:
        return os << "BOOL";
    case INT8:
        return os << "INT8";
    case INT16:
        return os << "INT16";
    case INT32:
        return os << "INT32";
    case INT64:
        return os << "INT64";
    case UINT8:
        return os << "UINT8";
    case UINT16:
        return os << "UINT16";
    case UINT32:
        return os << "UINT32";
    case UINT64:
        return os << "UINT64";
    case FLOAT32:
        return os << "FLOAT32";
    case FLOAT64:
        return os << "FLOAT64";
    case CHAR:
        return os << "CHAR";
    case STRING:
        return os << "STRING";
    case ID:
        return os << "ID";
    case NUM_ID:
        return os << "NUM_ID";
    case INIT:
        return os << "INIT";
    case UPDATE:
        return os << "UPDATE";
    case SET_ATTR:
        return os << "SET_ATTR";
    case CALL_ATTR:
        return os << "CALL_ATTR";
    case UPDATE_ATTR:
        return os << "UPDATE_ATTR";
    case KEYWORD_ARG:
        return os << "KEYWORD_ARG";
    case TERNARY:
        return os << "TERNARY";
    case VAR:
        return os << "VAR";
    case LAMBDA_VAR:
        return os << "LAMBDA_VAR";
    case LAMBDA_ARGS:
        return os << "LAMBDA_ARGS";
    case LAMBDA_KWARGS:
        return os << "LAMBDA_KWARGS";
    case NULLARY:
        return os << "NULLARY";
    case UNARY:
        return os << "UNARY";
    case BINARY:
        return os << "BINARY";
    case SEQ:
        return os << "SEQ";
    case CALL:
        return os << "CALL";
    case DEF:
        return os << "DEF";
    case FOR:
        return os << "FOR";
    case IF:
        return os << "IF";
    case MATCH:
        return os << "MATCH";
    case TRY:
        return os << "TRY";
    case WHILE:
        return os << "WHILE";
    case TYPE:
        return os << "TYPE";
    }
}

}

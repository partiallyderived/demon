#pragma once

#include <ostream>

namespace dl {

// All of the unique operations that tokens may be parsed to.
enum class OpID {
    ADD,
    ADDR,
    ADDR_TYPE,
    ALNUM,
    AND,
    ARROW,
    BAND,
    BLOCK,
    BNOT,
    BOR,
    BREAK,
    BXOR,
    BY,
    CALL,
    CASE,
    CONCAT,
    CONSTRUCT,
    CONTINUE,
    DEF,
    DIV,
    DONE,
    DOUBLE_STAR,
    ELIF,
    ELSE,
    END,
    EQ,
    EXP,
    EXPR,
    FALSE,
    FOR,
    FROM,
    GET,
    GROUP,
    GT,
    GTE,
    IADD,
    IBAND,
    IBOR,
    IBXOR,
    IDIV,
    IEXP,
    IF,
    ILSH,
    IMOD,
    IMUL,
    IN,
    INTERFACE,
    IRSH,
    ISUB,
    LABEL,
    LIST,
    LSH,
    LT,
    LTE,
    MATCH,
    MOD,
    MUL,
    NEG,
    NEQ,
    NONE,
    NOT,
    OR,
    RAISE,
    RSH,
    SEP,
    SET,
    STAR,
    STMT,
    STRING,
    SUB,
    SUFFIX,
    TERNARY_ELSE,
    TERNARY_IF,
    THIS,
    TRUE,
    TYPE,
    TYPE_INTERFACE,
    UNPACK_ARGS,
    UNPACK_KWARGS,
    UP,
    VARS,
    WAITING
};

std::ostream& operator<<(std::ostream& os, OpID id) {
    switch(id) {
    case ADD:
        return os << "ADD";
    case ADDR:
        return os << "ADDR";
    case ADDR_TYPE:
        return os << "ADDR_TYPE";
    case ALNUM:
        return os << "ALNUM";
    case AND:
        return os << "AND";
    case ARROW:
        return os << "ARROW";
    case BAND:
        return os << "BAND";
    case BLOCK:
        return os << "BLOCK";
    case BNOT:
        return os << "BNOT";
    case BOR:
        return os << "BOR";
    case BREAK:
        return os << "BREAK";
    case BXOR:
        return os << "BXOR";
    case BY:
        return os << "BY";
    case CACHED_CALL:
        return os << "CACHED_CALL";
    case CALL:
        return os << "CALL";
    case CASE:
        return os << "CASE";
    case CONCAT:
        return os << "CONCAT";
    case CONSTRUCT:
        return os << "CONSTRUCT";
    case CONTINUE:
        return os << "CONTINUE";
    case DEF:
        return os << "DEF";
    case DIV:
        return os << "DIV";
    case DONE:
        return os << "DONE";
    case DOUBLE_STAR:
        return os << "DOUBLE_STAR";
    case ELIF:
        return os << "ELIF";
    case ELSE:
        return os << "ELSE";
    case END:
        return os << "END";
    case EQ:
        return os << "EQ";
    case EXP:
        return os << "EXP";
    case EXPR:
        return os << "EXPR";
    case FALSE:
        return os << "FALSE";
    case FOR:
        return os << "FOR";
    case FROM:
        return os << "FROM";
    case GET:
        return os << "GET";
    case GROUP:
        return os << "GROUP";
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
    case IDIV:
        return os << "IDIV";
    case IEXP:
        return os << "IEXP";
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
    case INTERFACE:
        return os << "INTERFACE";
    case IRSH:
        return os << "IRSH";
    case ISUB:
        return os << "ISUB";
    case LABEL:
        return os << "LABEL";
    case LIST:
        return os << "LIST";
    case LSH:
        return os << "LSH";
    case LT:
        return os << "LT";
    case LTE:
        return os << "LTE";
    case MATCH:
        return os << "MATCH";
    case MIXED:
        return os << "MIXED";
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
    case OR:
        return os << "OR";
    case RAISE:
        return os << "RAISE";
    case RSH:
        return os << "RSH";
    case SEP:
        return os << "SEP";
    case SET:
        return os << "SET";
    case STAR:
        return os << "STAR";
    case STMT:
        return os << "STMT";
    case STRING:
        return os << "STRING";
    case SUB:
        return os << "SUB";
    case SUFFIX:
        return os << "SUFFIX";
    case TERNARY_ELSE:
        return os << "TERNARY_ELSE";
    case TERNARY_IF:
        return os << "TERNARY_IF";
    case THIS:
        return os << "THIS";
    case TRUE:
        return os << "TRUE";
    case TYPE:
        return os << "TYPE";
    case TYPE_INTERFACE:
        return os << "TYPE_INTERFACE";
    case UNPACK_ARGS:
        return os << "UNPACK_ARGS";
    case UNPACK_KWARGS:
        return os << "UNPACK_KWARGS";
    case UP:
        return os << "UP";
    case VARS:
        return os << "VARS";
    case WAITING:
        return os << "WAITING";
    }
}

}

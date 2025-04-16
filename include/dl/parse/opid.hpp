#pragma once

#include <ostream>

namespace dl {

// All of the unique operations that tokens may be parsed to.
enum class OpID {
    ADD,
    ADDR,
    ADDR_TYPE,
    AND,
    AS,
    BAND,
    BIND,
    BLOCK,
    BLOCKS,
    BNOT,
    BODY,
    BOR,
    BREAK,
    BXOR,
    CALL,
    CASE,
    CHAR,
    CONCAT,
    CONSTRUCT,
    CONTINUE,
    DEF,
    DIV,
    DONE,
    ELIF,
    ELSE,
    ENCLOSURE,
    END,
    ENTRY,
    EQ,
    ERROR,
    EXCEPT,
    FALSE,
    FINALLY,
    FLOAT_TAIL,
    FOR,
    FOR_IN,
    GET,
    GROUP,
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
    IPOW,
    IRSH,
    ISUB,
    LABEL,
    LAMBDA,
    LAMBDA_ARGS,
    LAMBDA_KWARGS,
    LIST,
    LITERALLY,
    LSH,
    LT,
    LTE,
    MATCH,
    MATCH_OR,
    MOD,
    MUL,
    NEG,
    NEQ,
    NONE,
    NOT,
    NOT_IN,
    NOTHING,
    NULL_,
    NUMBER,
    OR,
    PLACEHOLDER,
    PLAIN_INT,
    POS_KW_SEP,
    POW,
    RAISE,
    RAW_NUMBER,
    RETURN,
    RETURNS,
    RSH,
    SEP,
    SET,
    STMT,
    STRING,
    SUB,
    SUFFIX,
    SYMBOL,
    TERNARY_ELSE,
    TERNARY_IF,
    THIS,
    TRUE,
    TRY,
    TYPE_LABEL,
    UNPACK_ARGS,
    UNPACK_KWARGS,
    UP,
    WHILE,
    WAITING
};

std::ostream& operator<<(std::ostream& os, OpID id) {
    using enum OpID;
    switch(id) {
    case ADD:
        return os << "ADD";
    case ADDR:
        return os << "ADDR";
    case ADDR_TYPE:
        return os << "ADDR_TYPE";
    case AND:
        return os << "AND";
    case AS:
        return os << "AS";
    case BAND:
        return os << "BAND";
    case BIND:
        return os << "BIND";
    case BLOCK:
        return os << "BLOCK";
    case BLOCKS:
        return os << "BLOCKS";
    case BNOT:
        return os << "BNOT";
    case BODY:
        return os << "BODY";
    case BOR:
        return os << "BOR";
    case BREAK:
        return os << "BREAK";
    case BXOR:
        return os << "BXOR";
    case CALL:
        return os << "CALL";
    case CASE:
        return os << "CASE";
    case CHAR:
        return os << "CHAR";
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
    case ELIF:
        return os << "ELIF";
    case ELSE:
        return os << "ELSE";
    case ENCLOSURE:
        return os << "ENCLOSURE";
    case END:
        return os << "END";
    case ENTRY:
        return os << "ENTRY";
    case EQ:
        return os << "EQ";
    case ERROR:
        return os << "ERROR";
    case EXCEPT:
        return os << "EXCEPT";
    case FALSE:
        return os << "FALSE";
    case FINALLY:
        return os << "FINALLY";
    case FLOAT_TAIL:
        return os << "FLOAT_TAIL";
    case FOR:
        return os << "FOR";
    case FOR_IN:
        return os << "FOR_IN";
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
    case IPOW:
        return os << "IPOW";
    case IRSH:
        return os << "IRSH";
    case ISUB:
        return os << "ISUB";
    case LABEL:
        return os << "LABEL";
    case LAMBDA:
        return os << "LAMBDA";
    case LAMBDA_ARGS:
        return os << "LAMBDA_ARGS";
    case LAMBDA_KWARGS:
        return os << "LAMBDA_KWARGS";
    case LIST:
        return os << "LIST";
    case LITERALLY:
        return os << "LITERALLY";
    case LSH:
        return os << "LSH";
    case LT:
        return os << "LT";
    case LTE:
        return os << "LTE";
    case MATCH:
        return os << "MATCH";
    case MATCH_OR:
        return os << "MATCH_OR";
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
    case NOT_IN:
        return os << "NOT_IN";
    case NOTHING:
        return os << "NOTHING";
    case NULL_:
        return os << "NULL";
    case NUMBER:
        return os << "NUMBER";
    case OR:
        return os << "OR";
    case PLACEHOLDER:
        return os << "PLACEHOLDER";
    case PLAIN_INT:
        return os << "PLAIN_INT";
    case POS_KW_SEP:
        return os << "POS_KW_SEP";
    case POW:
        return os << "POW";
    case RAISE:
        return os << "RAISE";
    case RAW_NUMBER:
        return os << "RAW_NUMBER";
    case RETURN:
        return os << "RETURN";
    case RETURNS:
        return os << "RETURNS";
    case RSH:
        return os << "RSH";
    case SEP:
        return os << "SEP";
    case SET:
        return os << "SET";
    case STMT:
        return os << "STMT";
    case STRING:
        return os << "STRING";
    case SUB:
        return os << "SUB";
    case SUFFIX:
        return os << "SUFFIX";
    case SYMBOL:
        return os << "SYMBOL";
    case TERNARY_ELSE:
        return os << "TERNARY_ELSE";
    case TERNARY_IF:
        return os << "TERNARY_IF";
    case THIS:
        return os << "THIS";
    case TRUE:
        return os << "TRUE";
    case TRY:
        return os << "TRY";
    case TYPE_LABEL:
        return os << "TYPE_LABEL";
    case UNPACK_ARGS:
        return os << "UNPACK_ARGS";
    case UNPACK_KWARGS:
        return os << "UNPACK_KWARGS";
    case UP:
        return os << "UP";
    case WHILE:
        return os << "WHILE";
    case WAITING:
        return os << "WAITING";
    }
}

}

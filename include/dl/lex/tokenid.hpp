#pragma once

#include <ostream>

namespace dl {

// The different unique tokens that may be lexxed, excluding their associated
// data, such as the content of a string.
enum class TokenID {
    AMPERSAND,
    AMPERSAND_EQUALS,
    AND,
    AS,
    AT,
    BANG_EQUALS,
    BREAK,
    CAROT,
    CAROT_EQUALS,
    CASE,
    CHAR,
    COLON,
    COMMA,
    CONTINUE,
    DEF,
    DOUBLE_EQUALS,
    DOUBLE_LEFT_ANGLE,
    DOUBLE_LEFT_ANGLE_EQUALS,
    DOUBLE_PIPE,
    DOUBLE_RIGHT_ANGLE,
    DOUBLE_RIGHT_ANGLE_EQUALS,
    DOUBLE_STAR,
    DOUBLE_STAR_EQUALS,
    DOT,
    ELIF,
    ELSE,
    END_OF_FILE,
    EQUALS,
    EXCEPT,
    FALSE,
    FINALLY,
    FLOAT_TAIL,
    FOR,
    HASH,
    ID,
    IF,
    IN,
    LEFT_ANGLE,
    LEFT_ANGLE_EQUALS,
    LEFT_CURLY,
    LEFT_CURVED,
    LEFT_SQUARE,
    MATCH,
    MINUS,
    MINUS_EQUALS,
    MINUS_RIGHT_ANGLE,
    NEWLINE,
    NONE,
    NOT,
    NULL_,
    NUMBER,
    OR,
    PERCENT,
    PERCENT_DOUBLE_STAR,
    PERCENT_EQUALS,
    PERCENT_STAR,
    PIPE,
    PIPE_EQUALS,
    PLACEHOLDER,
    PLAIN_INT,
    PLUS,
    PLUS_EQUALS,
    RAISE,
    RETURN,
    RIGHT_ANGLE,
    RIGHT_ANGLE_EQUALS,
    RIGHT_CURLY,
    RIGHT_CURVED,
    RIGHT_SQUARE,
    SLASH,
    SLASH_EQUALS,
    SPACE,
    STAR,
    STAR_EQUALS,
    STRING,
    THIS,
    TILDE,
    TRUE,
    TRY,
    WHILE
};

std::ostream& operator<<(std::ostream& os, TokenID id) {
    using enum TokenID;
    switch(id) {
    case AMPERSAND:
        return os << "AMPERSAND";
    case AMPERSAND_EQUALS:
        return os << "AMPERSAND_EQUALS";
    case AND:
        return os << "AND";
    case AS:
        return os << "AS";
    case AT:
        return os << "AT";
    case BANG_EQUALS:
        return os << "BANG_EQUALS";
    case BREAK:
        return os << "BREAK";
    case CAROT:
        return os << "CAROT";
    case CAROT_EQUALS:
        return os << "CAROT_EQUALS";
    case CASE:
        return os << "CASE";
    case CHAR:
        return os << "CHAR";
    case COLON:
        return os << "COLON";
    case COMMA:
        return os << "COMMA";
    case CONTINUE:
        return os << "CONTINUE";
    case DEF:
        return os << "DEF";
    case DOUBLE_EQUALS:
        return os << "DOUBLE_EQUALS";
    case DOUBLE_LEFT_ANGLE:
        return os << "DOUBLE_LEFT_ANGLE";
    case DOUBLE_LEFT_ANGLE_EQUALS:
        return os << "DOUBLE_LEFT_ANGLE_EQUALS";
    case DOUBLE_PIPE:
        return os << "DOUBLE_PIPE";
    case DOUBLE_RIGHT_ANGLE:
        return os << "DOUBLE_RIGHT_ANGLE";
    case DOUBLE_RIGHT_ANGLE_EQUALS:
        return os << "DOUBLE_RIGHT_ANGLE_EQUALS";
    case DOUBLE_STAR:
        return os << "DOUBLE_STAR";
    case DOUBLE_STAR_EQUALS:
        return os << "DOUBLE_STAR_EQUALS";
    case DOT:
        return os << "DOT";
    case ELIF:
        return os << "ELIF";
    case ELSE:
        return os << "ELSE";
    case END_OF_FILE:
        return os << "END_OF_FILE";
    case EQUALS:
        return os << "EQUALS";
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
    case HASH:
        return os << "HASH";
    case ID:
        return os << "ID";
    case IF:
        return os << "IF";
    case IN:
        return os << "IN";
    case LEFT_ANGLE:
        return os << "LEFT_ANGLE";
    case LEFT_ANGLE_EQUALS:
        return os << "LEFT_ANGLE_EQUALS";
    case LEFT_CURLY:
        return os << "LEFT_CURLY";
    case LEFT_CURVED:
        return os << "LEFT_CURVED";
    case LEFT_SQUARE:
        return os << "LEFT_SQUARE";
    case MATCH:
        return os << "MATCH";
    case MINUS:
        return os << "MINUS";
    case MINUS_EQUALS:
        return os << "MINUS_EQUALS";
    case MINUS_RIGHT_ANGLE:
        return os << "MINUS_RIGHT_ANGLE";
    case NEWLINE:
        return os << "NEWLINE";
    case NONE:
        return os << "NONE";
    case NOT:
        return os << "NOT";
    case NULL_:
        return os << "NULL";
    case NUMBER:
        return os << "NUMBER";
    case OR:
        return os << "OR";
    case PERCENT:
        return os << "PERCENT";
    case PERCENT_DOUBLE_STAR:
        return os << "PERCENT_DOUBLE_STAR";
    case PERCENT_EQUALS:
        return os << "PERCENT_EQUALS";
    case PERCENT_STAR:
        return os << "PERCENT_STAR";
    case PIPE:
        return os << "PIPE";
    case PIPE_EQUALS:
        return os << "PIPE_EQUALS";
    case PLACEHOLDER:
        return os << "PLACEHOLDER";
    case PLAIN_INT:
        return os << "PLAIN_INT";
    case PLUS:
        return os << "PLUS";
    case PLUS_EQUALS:
        return os << "PLUS_EQUALS";
    case RAISE:
        return os << "RAISE";
    case RETURN:
        return os << "RETURN";
    case RIGHT_ANGLE:
        return os << "RIGHT_ANGLE";
    case RIGHT_ANGLE_EQUALS:
        return os << "RIGHT_ANGLE_EQUALS";
    case RIGHT_CURLY:
        return os << "RIGHT_CURLY";
    case RIGHT_CURVED:
        return os << "RIGHT_CURVED";
    case RIGHT_SQUARE:
        return os << "RIGHT_SQUARE";
    case SLASH:
        return os << "SLASH";
    case SLASH_EQUALS:
        return os << "SLASH_EQUALS";
    case SPACE:
        return os << "SPACE";
    case STAR:
        return os << "STAR";
    case STAR_EQUALS:
        return os << "STAR_EQUALS";
    case STRING:
        return os << "STRING";
    case THIS:
        return os << "THIS";
    case TILDE:
        return os << "TILDE";
    case TRUE:
        return os << "TRUE";
    case TRY:
        return os << "TRY";
    case WHILE:
        return os << "WHILE";
    }
}

}

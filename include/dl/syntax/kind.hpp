#pragma once

#include <ostream>

namespace dl::syntax {

// Enumeration of kinds of operators for use in switch statements.
enum class Kind {
    ADD,
    AND,
    BEGIN,
    BAND,
    BLOCK,
    BNOT,
    BOR,
    BREAK,
    BXOR,
    BY,
    CALL,
    CONCAT,
    CONTINUE,
    DIV,
    ELIF,
    ELSE,
    END_BLOCK,
    END_GROUP,
    END_LIST,
    END_OF_FILE,
    EQ,
    EXPAND,
    EXPR,
    FALSE,
    FOR,
    FROM,
    GET,
    GROUP,
    GT,
    GTE,
    HASH,
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
    INT,
    IRSH,
    ISUB,
    LABEL,
    LEADING_SPACE,
    LIST,
    LSH,
    LT,
    LTE,
    MEMBER,
    MIXED,
    MOD,
    MUL,
    NEG,
    NEQ,
    NEWLINE,
    NEWLINE_THEN_GLOBAL,
    NOT,
    OR,
    RETURN,
    RSH,
    SEQ,
    SET,
    SPACE,
    STRING,
    SUFFIX,
    STATEMENTS,
    SUB,
    THIS,
    TO,
    TRUE,
    VAR,
    VARS
};

std::ostream& operator<<(std::ostream& os, Kind kind) {
    using enum Kind;
    switch(kind) {
    case ADD:
        return os << "Add";
    case AND:
        return os << "And";
    case BEGIN:
        return os << "Begin";
    case BAND:
        return os << "BitwiseAnd";
    case BLOCK:
        return os << "Block";
    case BNOT:
        return os << "BitwiseNot";
    case BOR:
        return os << "BitwiseOr";
    case BREAK:
        return os << "Break";
    case BXOR:
        return os << "BitwiseXor";
    case BY:
        return os << "By";
    case CALL:
        return os << "Call";
    case CONCAT:
        return os << "Concat";
    case CONTINUE:
        return os << "Continue";
    case DIV:
        return os << "Divide";
    case ELIF:
        return os << "Elif";
    case ELSE:
        return os << "Else";
    case END_BLOCK:
        return os << "EndBlock";
    case END_GROUP:
        return os << "EndGroup";
    case END_LIST:
        return os << "EndList";
    case END_OF_FILE:
        return os << "EndOfFile";
    case EQ:
        return os << "Equals";
    case EXPAND:
        return os << "Expand";
    case EXPR:
        return os << "Expr";
    case FALSE:
        return os << "false";
    case FOR:
        return os << "For";
    case FROM:
        return os << "From";
    case GET:
        return os << "Get";
    case GROUP:
        return os << "Group";
    case GT:
        return os << "GreaterThan";
    case GTE:
        return os << "GreaterThanOrEqual";
    case HASH:
        return os << "Hash";
    case IADD:
        return os << "IAdd";
    case IBAND:
        return os << "IBitwiseAnd";
    case IBOR:
        return os << "IBitwiseOr";
    case IBXOR:
        return os << "IBitwiseXor";
    case ID:
        return os << "ID";
    case IDIV:
        return os << "IDivide";
    case IF:
        return os << "If";
    case ILSH:
        return os << "ILeftShift";
    case IMOD:
        return os << "IModulo";
    case IMUL:
        return os << "IMultiply";
    case INT:
        return os << "Int";
    case IRSH:
        return os << "IRightShift";
    case ISUB:
        return os << "ISubtract";
    case LABEL:
        return os << "Label";
    case LEADING_SPACE:
        return os << "LeadingSpace";
    case LIST:
        return os << "List";
    case LSH:
        return os << "LeftShift";
    case LT:
        return os << "LessThan";
    case LTE:
        return os << "LessThanOrEqual";
    case MEMBER:
        return os << "Member";
    case MIXED:
        return os << "Mixed";
    case MOD:
        return os << "Modulo";
    case MUL:
        return os << "Multiply";
    case NEG:
        return os << "Negate";
    case NEQ:
        return os << "NotEquals";
    case NEWLINE:
        return os << "Newline";
    case NEWLINE_THEN_GLOBAL:
        return os << "NewlineThenGlobal";
    case NOT:
        return os << "Not";
    case OR:
        return os << "Or";
    case RETURN:
        return os << "Return";
    case RSH:
        return os << "RightShift";
    case SEQ:
        return os << "Seq";
    case SET:
        return os << "Set";
    case SPACE:
        return os << "Space";
    case STATEMENTS:
        return os << "Statements";
    case STRING:
        return os << "String";
    case SUB:
        return os << "Subtract";
    case SUFFIX:
        return os << "Suffix";
    case THIS:
        return os << "This";
    case TO:
        return os << "To";
    case TRUE:
        return os << "True";
    case VAR:
        return os << "Var";
    case VARS:
        return os << "Vars";
    }
}

}

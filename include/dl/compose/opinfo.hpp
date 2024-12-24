#pragma once

#include "dl/parse/opid.hpp"
#include "dl/compose/opkind.hpp"
#include "dl/compose/precedence.hpp"

namespace dl {

struct OpInfo {
    OpKind kind;
    Precedence left_precedence;
    Precedence right_precedence;

    OpInfo(OpKind kind, Precedence left_precedence, Precedence right_precedence)
    noexcept:
    kind(kind),
    left_precedence(left_precedence),
    right_precedence(right_precedence) {}

    // Symmetric (left_precedence == right_precedence) constructor
    OpInfo(OpKind kind, Precedence precedence) noexcept:
    kind(kind), left_precedence(precedence), right_precedence(precedence) {}

    // Constructor for operators for which precedence does not apply.
    OpInfo(OpKind kind) noexcept:
    kind(kind),
    left_precedence(Precedence::START),
    right_precedence(Precedence::START) {}
};

const auto CMP_INFO_ = OpInfo(OpKind::BINARY, Precedence::CMP);

const auto FLOW_INFO_ = OpInfo(OpKind::UNARY, Precedence::FLOW);

const auto LEFT_INFO_ = OpInfo(
    OpKind::UNARY, Precedence::START, Precedence::UNARY
);

const auto PREFIX_INFO_ = OpInfo(
    OpKind::UNARY, Precedence::PREFIX, Precedence::UNARY
);

const auto SET_INFO_ = OpInfo(
    OpKind::BINARY, Precedence::LSET, Precedence::RSET
);

const OpInfo OP_INFO_[] = {
    // ADD
    OpInfo(OpKind::BINARY, Precedence::ADD),
    // ADDR
    PREFIX_INFO_,
    // ADDR_TYPE
    OpInfo(OpKind::UNARY, Precedence::UNARY),
    // AND
    OpInfo(OpKind::BINARY, Precedence::AND),
    // ARROW
    OpInfo(OpKind::BINARY, Precedence::LABEL),
    // BAND
    OpInfo(OpKind::BINARY, Precedence::BAND),
    // BIND
    OpInfo(OpKind::BINARY, Precedence::LABEL),
    // BLOCK
    OpInfo(OpKind::AGGREGATE, Precedence::START, Precedence::UNARY),
    // BNOT
    PREFIX_INFO_,
    // BOR
    OpInfo(OpKind::BINARY, Precedence::BOR),
    // BREAK
    OpInfo(OpKind::NULLARY, Precedence::FLOW),
    // BXOR
    OpInfo(OpKind::BINARY, Precedence::BXOR),
    // BY
    OpInfo(OpKind::BINARY, Precedence::FOR_PRED),
    // CALL
    OpInfo(OpKind::BINARY, Precedence::CALL),
    // CASE
    FLOW_INFO_,
    // CHAR
    OpInfo(OpKind::DATA),
    // CONCAT
    OpInfo(OpKind::BINARY, Precedence::CONCAT),
    // CONSTRUCT
    OpInfo(OpKind::AGGREGATE, Precedence::START, Precedence::UNARY),
    // CONTINUE
    OpInfo(OpKind::NULLARY, Precedence::FLOW),
    // DEF
    FLOW_INFO_,
    // DEF_AS
    OpInfo(OpKind::BINARY, Precedence::LABEL),
    // DIV
    OpInfo(OpKind::BINARY, Precedence::MUL),
    // DONE
    OpInfo(OpKind::DONE),
    // ELIF
    FLOW_INFO_,
    // ELSE
    FLOW_INFO_,
    // END
    OpInfo(OpKind::END, Precedence::END),
    // EQ
    CMP_INFO_,
    // EXCEPT
    FLOW_INFO_,
    // EXP
    OpInfo(OpKind::BINARY, Precedence::LEXP, Precedence::REXP),
    // EXPR
    PREFIX_INFO_,
    // FALSE
    OpInfo(OpKind::SINGLETON),
    // FINALLY
    FLOW_INFO_,
    // FOR
    FLOW_INFO_,
    // FROM
    OpInfo(OpKind::BINARY, Precedence::FOR_PRED),
    // GET
    OpInfo(OpKind::BINARY, Precedence::GET),
    // GROUP
    LEFT_INFO_,
    // GT
    CMP_INFO_,
    // GTE
    CMP_INFO_,
    // IADD
    SET_INFO_,
    // IBAND
    SET_INFO_,
    // IBOR
    SET_INFO_,
    // IBXOR
    SET_INFO_,
    // ID
    OpInfo(OpKind::DATA),
    // IDIV
    SET_INFO_,
    // IEXP
    SET_INFO_,
    // IF
    FLOW_INFO_,
    // ILSH
    SET_INFO_,
    // IMOD
    SET_INFO_,
    // IMUL
    SET_INFO_,
    // IN
    OpInfo(OpKind::BINARY, Precedence::IN),
    // IRSH
    SET_INFO_,
    // ISUB
    SET_INFO_,
    // LABEL
    OpInfo(OpKind::BINARY, Precedence::LABEL),
    // LIST
    LEFT_INFO_,
    // LSH
    OpInfo(OpKind::BINARY, Precedence::SHIFT),
    // LT
    CMP_INFO_,
    // LTE
    CMP_INFO_,
    // MATCH
    FLOW_INFO_,
    // MOD
    OpInfo(OpKind::BINARY, Precedence::MUL),
    // MUL
    OpInfo(OpKind::BINARY, Precedence::MUL),
    // NEG
    PREFIX_INFO_,
    // NEQ
    CMP_INFO_,
    // NONE
    OpInfo(OpKind::SINGLETON),
    // NOT
    PREFIX_INFO_,
    // NOT_IN
    PREFIX_INFO_,
    // NOTHING
    OpInfo(OpKind::SINGLETON),
    // NULL
    OpInfo(OpKind::SINGLETON),
    // NUMBER
    OpInfo(OpKind::DATA),
    // OR
    OpInfo(OpKind::BINARY, Precedence::OR),
    // POS_KW_SEP
    OpInfo(OpKind::SINGLETON),
    // RAISE
    FLOW_INFO_,
    // RETURN,
    FLOW_INFO_,
    // RSH
    OpInfo(OpKind::BINARY, Precedence::SHIFT),
    // SEP
    OpInfo(OpKind::BINARY, Precedence::SEP),
    // SET
    SET_INFO_,
    // STMT
    OpInfo(OpKind::UNARY, Precedence::STATEMENTS),
    // STRING
    OpInfo(OpKind::DATA),
    // SUB
    OpInfo(OpKind::BINARY, Precedence::ADD),
    // SUFFIX
    OpInfo(OpKind::BINARY, Precedence::SUFFIX),
    // TERNARY_ELSE
    OpInfo(OpKind::BINARY, Precedence::FLOW),
    // TERNARY_IF
    OpInfo(OpKind::BINARY, Precedence::FLOW),
    // THIS
    OpInfo(OpKind::SINGLETON),
    // TO
    OpInfo(OpKind::BINARY, Precedence::FOR_PRED),
    // TRUE
    OpInfo(OpKind::SINGLETON),
    // TRY
    FLOW_INFO_,
    // TYPE
    FLOW_INFO_,
    // UNPACK_ARGS
    PREFIX_INFO_,
    // UNPACK_KWARGS
    PREFIX_INFO_,
    // UP
    PREFIX_INFO_,
    // VARS
    OpInfo(OpKind::SINGLETON),
    // WHILE
    FLOW_INFO_,
    // WAITING
    OpInfo(OpKind::WAITING)
};

const OpInfo& opinfo(OpID op) noexcept {
    return OP_INFO_[static_cast<int>(op)];
}

bool has_precedence(OpID op1, OpID op2) noexcept {
    return opinfo(op1).left_precedence >= opinfo(op2).right_precedence;
}

}

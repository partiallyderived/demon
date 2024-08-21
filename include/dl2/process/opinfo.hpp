#pragma once

#include "dl/process/opid.hpp"
#include "dl/process/opkind.hpp"
#include "dl/process/precedence.hpp"

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
    // ALNUM
    OpInfo(OpKind::STRING),
    // AND
    OpInfo(OpKind::BINARY, Precedence::AND),
    // ARROW
    OpInfo(OpKind::BINARY, Precedence::LABEL),
    // BAND
    OpInfo(OpKind::BINARY, Precedence::BAND),
    // BLOCK
    OpInfo(OpKind::BLOCK, Precedence::START, Precedence::UNARY),
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
    // CONCAT
    OpInfo(OpKind::BINARY, Precedence::CONCAT),
    // CONSTRUCT
    FLOW_INFO_,
    // CONTINUE
    OpInfo(OpKind::NULLARY, Precedence::FLOW),
    // DEF
    FLOW_INFO_,
    // DIV
    OpInfo(OpKind::BINARY, Precedence::MUL),
    // DONE
    OpInfo(OpKind::DONE),
    // DOUBLE_STAR
    OpInfo(OpKind::SINGLETON),
    // ELIF
    FLOW_INFO_,
    // ELSE
    FLOW_INFO_,
    // END
    OpInfo(OpKind::END, Precedence::END),
    // EQ
    CMP_INFO_,
    // EXP
    OpInfo(OpKind::BINARY, Precedence::LEXP, Precedence::REXP),
    // EXPR
    PREFIX_INFO_,
    // FALSE
    OpInfo(OpKind::SINGLETON),
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
    // INTERFACE
    FLOW_INFO_,
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
    // OR
    OpInfo(OpKind::BINARY, Precedence::OR),
    // RAISE
    FLOW_INFO_,
    // RSH
    OpInfo(OpKind::BINARY, Precedence::SHIFT),
    // SEP
    OpInfo(OpKind::BINARY, Precedence::SEP),
    // SET
    SET_INFO_,
    // STAR
    OpInfo(OpKind::SINGLETON),
    // STMT
    OpInfo(OpKind::BINARY, Precedence::STATEMENTS),
    // STRING
    OpInfo(OpKind::STRING),
    // SUB
    OpInfo(OpKind::BINARY, Precedence::ADD),
    // SUFFIX
    OpInfo(OpKind::BINARY, Precedence::SUFFIX),
    // TERNARY_ELSE
    OpInfo(OpKind::BINARY, Precedence::TERNARY),
    // TERNARY_IF
    OpInfo(OpKind::BINARY, Precedence::TERNARY),
    // THIS
    OpInfo(OpKind::SINGLETON),
    // TRUE
    OpInfo(OpKind::SINGLETON),
    // TYPE
    FLOW_INFO_,
    // TYPE_INTERFACE
    FLOW_INFO_,
    // UNPACK_ARGS
    PREFIX_INFO_,
    // UNPACK_KWARGS
    PREFIX_INFO_,
    // UP
    PREFIX_INFO_,
    // VARS
    OpInfo(OpKind::SINGLETON),
    // WAITING
    OpInfo(OpKind::WAITING)
} 

const OpInfo& opinfo(OpID op) noexcept {
    return OP_INFO_[static_cast<int>(op)];
}

bool has_precedence(OpID op1, OpID op2) noexcept {
    return opinfo(op1).left_precedence >= opinfo(op2).right_precedence;
}

}

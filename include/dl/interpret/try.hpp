#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/block.hpp"
#include "dl/interpret/matchcase.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/span.hpp"
#include "dl/util.hpp"

namespace dl {

struct Try final: Node_<Try> {
    Block body;
    std::vector<MatchCase> excepts;
    Block finally;

    Try(
        Block&& body,
        std::vector<MatchCase>&& excepts,
        Block&& finally,
        Span src
    )
    noexcept:
    Node_<Try>(src),
    body(std::move(body)),
    excepts(std::move(excepts)),
    finally(std::move(finally)) {}

    virtual Try copy() const override {
        return Try(body.copy(), deep_copy(excepts), finally.copy(), this->src);
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Try&>(that);
        return
            body == casted.body &&
            excepts == casted.excepts &&
            finally == casted.finally;
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::TRY;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os <<
            body << ", " << OutContainerManip(excepts) << ", " << finally;
    }

    virtual Span span() const noexcept override {
        Span end = finally.code.size() > 0 ?
            finally.code.back()->span(): excepts.back().span();
        return Span(body.src, end);
    }
};

}

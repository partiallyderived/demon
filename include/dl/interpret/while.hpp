#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/block.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/span.hpp"
#include "dl/util.hpp"

namespace dl {

struct While final: Node_<While> {
    NodePtr content;
    NodePtr orelse;

    While(NodePtr&& content, NodePtr&& orelse, Span src) noexcept:
    Node_(src),
    content(std::move(content)),
    orelse(std::move(orelse)) {}

    virtual While copy() const override {
        return While(copy_np(content), copy_np(orelse), src);
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const While&>(that);
        return
            npeq(content, casted.content) && npeq(orelse, casted.orelse);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::WHILE;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, content, orelse);
    }

    virtual Span span() const noexcept override {
        if (orelse != nullptr)
            return Span(src, orelse->span());
        return content->span();
    }
};

}

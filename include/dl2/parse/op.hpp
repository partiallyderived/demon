#pragma once

#include <cstdint>

#include <string>
#include <utility>

#include "dl/parse/opid.hpp"

namespace dl {

struct Op {
    OpID id;
    std::string content;
    std::uint64_t src_id;

    Op(OpID id, std::uint64_t src_id = -1) noexcept:
    id(id), content(), src_id(src_id) {}

    Op(OpID id, std::string&& content, std::uint64_t src_id = -1) noexcept:
    id(id), content(std::move(content)), src_id(src_id) {}
};

}

#pragma once

#include <cstdint>

#include <ios>
#include <ostream>
#include <string>

#include "dl/interpret/node"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct None final: Node_<NodeID::NONE> {
    virtual bool equals(const Node& that) const noexcept override {
        return true;
    }

    virtual std::ostream& out_data(std::ostream& os) const {
        return os;
    }
};

struct Null final: Node_<NodeID::NULL_> {
    virtual bool equals(const Node& that) const noexcept override {
        return true;
    }

    virtual std::ostream& out_data(std::ostream& os) const {
        return os;
    }
};

struct Bool final: Node_<NodeID::BOOL> {
    bool val;

    virtual bool equals(const Node& that) const noexcept override {
        return val == dynamic_cast<const Bool&>(that).val;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << std::boolalpha << val;
    }
}

struct Int8 final: Node_<NodeID::INT8> {
    std::int8_t val;

    virtual bool equals(const Node& that) const noexcept override {
        return val == dynamic_cast<const Int8&>(that).val;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << val;
    }
};

struct UInt8 final: Node_<NodeID::UINT8> {
    std::uint8_t val;

    virtual bool equals(const Node& that) const noexcept override {
        return val == dynamic_cast<const UInt8&>(that).val;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << val;
    }
};

struct Int16 final: Node_<NodeID::INT16> {
    std::int16_t val;

    virtual bool equals(const Node& that) const noexcept override {
        return val == dynamic_cast<const Int16&>(that).val;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << val;
    }
};

struct UInt16 final: Node_<NodeID::UINT16> {
    std::uint16_t val;

    virtual bool equals(const Node& that) const noexcept override {
        return val == dynamic_cast<const UInt16&>(that).val;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << val;
    }
};

struct Int32 final: Node_<NodeID::INT32> {
    std::int32_t val;

    virtual bool equals(const Node& that) const noexcept override {
        return val == dynamic_cast<const Int32&>(that).val;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << val;
    }
};

struct UInt32 final: Node_<NodeID::UINT32> {
    std::uint32_t val;

    virtual bool equals(const Node& that) const noexcept override {
        return val == dynamic_cast<const UInt32&>(that).val;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << val;
    }
};

struct Int64 final: Node_<NodeID::INT64> {
    std::int64_t val;

    virtual bool equals(const Node& that) const noexcept override {
        return val == dynamic_cast<const Int64&>(that).val;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << val;
    }
};

struct UInt64 final: Node_<NodeID::UINT64> {
    std::uint64_t val;

    virtual bool equals(const Node& that) const noexcept override {
        return val == dynamic_cast<const UInt64&>(that).val;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << val;
    }
};

struct Char final: Node_<NodeID::CHAR> {
    std::uint32_t val;

    virtual bool equals(const Node& that) const noexcept override {
        return val == dynamic_cast<const Char&>(that).val;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << val;
    }
};

struct String final: Node_<NodeID::STRING> {
    std::string val;

    virtual bool equals(const Node& that) const noexcept override {
        return val == dynamic_cast<const String&>(that).val;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << val;
    }
};

}

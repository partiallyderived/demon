#include <ostream>
#include <utility>
#include <vector>

#include "dl/lex/token.hpp"
#include "dl/span.hpp"
#include "dl/util.hpp"

namespace dl {

template<typename T, typename... Ts>
struct vec_ {
    static std::vector<T> fn(Ts&&... args, T&& last) {
        std::vector<T> res = vec_<Ts...>::fn(std::move(args)...);
        res.push_back(std::move(last));
        return res;
    }
};

template<typename T>
struct vec_<T> {
    static std::vector<T> fn(T&& arg) {
        std::vector<T> res;
        res.push_back(std::move(arg));
        return res;
    }
};

template<typename... Ts>
auto vec(Ts&&... args) {
    return vec_<Ts...>::fn(std::move(args)...);
}

std::ostream& operator<<(std::ostream& os, const std::pair<Token, Span>& x) {
    return os << "(" << x.first << "@" << x.second << ")";
}

std::ostream& operator<<(
    std::ostream& os, const std::vector<std::pair<Token, Span>>& v
) {
    return os << OutContainerManip(v);
}

}

#include <utility>
#include <vector>

#include "dl/compose/comp.hpp"
#include "dl/pos.hpp"

using namespace dl;

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

Comp vec_to_csv_recurse(std::vector<Comp>& comps, int i) {
    if (i == 0)
        return std::move(comps[0]);
    if (i == 1)
        return Comp(
            OpID::SEP, std::move(comps[0]), std::move(comps[1]), Pos(0, 0)
        );
    return Comp(
        OpID::SEP,
        vec_to_csv_recurse(comps, i - 1),
        std::move(comps[i]),
        Pos(0, 0)
    );
}

Comp vec_to_csv(std::vector<Comp>&& comps) {
    return vec_to_csv_recurse(comps, comps.size() - 1);
}

template<typename... Ts>
auto csv(Ts&&... args) {
    return vec_to_csv(vec<Ts...>(std::move(args)...));
}

auto s_end = Pos(0, 1);
auto s0 = Pos(0, 0);
auto s1 = Pos(1, 1);
auto s2 = Pos(2, 2);
auto s3 = Pos(3, 3);
auto s4 = Pos(4, 4);
auto s5 = Pos(5, 5);
auto s6 = Pos(6, 6);
auto s7 = Pos(7, 7);
auto s8 = Pos(8, 8);
auto s9 = Pos(9, 9);
auto s10 = Pos(10, 10);
auto s11 = Pos(11, 11);
auto s12 = Pos(12, 12);
auto s13 = Pos(13, 13);
auto s14 = Pos(14, 14);
auto s15 = Pos(15, 15);
auto s16 = Pos(16, 16);
auto s17 = Pos(17, 17);
auto s18 = Pos(18, 18);
auto s19 = Pos(19, 19);
auto s20 = Pos(20, 20);
auto s21 = Pos(21, 21);
auto s22 = Pos(22, 22);
auto s23 = Pos(23, 23);
auto s24 = Pos(24, 24);
auto s25 = Pos(25, 25);
auto s26 = Pos(26, 26);
auto s27 = Pos(27, 27);
auto s28 = Pos(28, 28);
auto s29 = Pos(29, 29);
auto s30 = Pos(30, 30);
auto s31 = Pos(31, 31);
auto s32 = Pos(32, 32);
auto s33 = Pos(33, 33);
auto s34 = Pos(34, 34);
auto s35 = Pos(35, 35);
auto s36 = Pos(36, 36);
auto s37 = Pos(37, 37);
auto s38 = Pos(38, 38);
auto s39 = Pos(39, 39);
auto s40 = Pos(40, 40);
auto s41 = Pos(41, 41);
auto s42 = Pos(42, 42);
auto s43 = Pos(43, 43);
auto s44 = Pos(44, 44);
auto s45 = Pos(45, 45);
auto s46 = Pos(46, 46);
auto s47 = Pos(47, 47);
auto s48 = Pos(48, 48);

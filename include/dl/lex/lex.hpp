#pragma once

#include <cctype>
#include <cstdint>

#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include "dl/err.hpp"
#include "dl/pos.hpp"
#include "dl/lex/cursor.hpp"
#include "dl/syntax/export.hpp"

namespace dl::lex {

// Mapping from keywords to pointers to their corresponding token objects.
std::unordered_map<std::string_view, syntax::Token*> KEYWORDS {
    {"and", &syntax::And::TOKEN},
    {"break", &syntax::Break::TOKEN},
    {"by", &syntax::By::TOKEN},
    {"continue", &syntax::Continue::TOKEN},
    {"elif", &syntax::Elif::TOKEN},
    {"else", &syntax::Else::TOKEN},
    {"if", &syntax::If::TOKEN},
    {"false", &syntax::False::TOKEN},
    {"for", &syntax::For::TOKEN},
    {"from", &syntax::From::TOKEN},
    {"not", &syntax::Not::TOKEN},
    {"or", &syntax::Or::TOKEN},
    {"return", &syntax::Return::TOKEN},
    {"to", &syntax::To::TOKEN},
    {"this", &syntax::This::TOKEN},
    {"true", &syntax::True::TOKEN},
    {"vars", &syntax::Vars::TOKEN}
};

// Base class for errors discovered during the lexing process.
struct LexErr: LocatedErr {
    using LocatedErr::LocatedErr;

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "LexErr";
    }
};

// Errors which have `Str` data attached to them.
struct StrErr: LexErr {
    std::string str;

    StrErr(std::string&& s, Pos loc) noexcept: LexErr(loc), str(std::move(s)) {}

    virtual bool equals(const Err& that) const noexcept override {
        return
            LocatedErr::equals(that) &&
            str == dynamic_cast<const StrErr*>(&that)->str;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return LocatedErr::out_data(os) << ", " << str;
    }

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "StrErr";
    }
};

// Indicates that an invalid `\x` was parsed.
struct BadByteErr final: StrErr {
    using StrErr::StrErr;

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "BadByteErr";
    }
};

// Indicates that an invalid escape sequence was found.
struct BadEscapeErr final: StrErr {
    using StrErr::StrErr;

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "BadEscapeErr";
    }
};

// Indicates that an invalid `\U` was found.
struct BadLongUnicodeErr final: StrErr {
    using StrErr::StrErr;

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "BadLongUnicodeErr";
    }
};

// Indicates that an invalid `\u` was found.
struct BadUnicodeErr final: StrErr {
    using StrErr::StrErr;

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "BadUnicodeErr";
    }
};

// Indicates that a line ended with space which was unassociated with a string.
struct TrailingSpaceErr final: LexErr {
    using LexErr::LexErr;

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "TrailingSpaceErr";
    }
};

// Indicates that a string was started but was unclosed: the statement ended
// before the closing " was found.
struct UnclosedStrErr final: StrErr {
    using StrErr::StrErr;

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "UnclosedStrErr";
    }
};

// Indicates that an unexpected character was read.
struct UnexpectedCharErr final: LexErr {
    // The unexpected character.
    char c;

    constexpr UnexpectedCharErr(char c, Pos loc) noexcept: LexErr(loc), c(c) {}

    bool equals(const Err& that) const noexcept override {
        return
            LocatedErr::equals(that) &&
            c == dynamic_cast<const UnexpectedCharErr*>(&that)->c;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return LocatedErr::out_data(os) << ", " << c;
    }

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "UnexpectedCharErr";
    }
};

// Computes the numeric value of a hexadecimal digit.
constexpr char hexvalue(char c) noexcept {
    return std::tolower(c) - '0' - ('a' - '9' - 1) * (c >= 'a');
}

// Check if the next char matches `match`. If so, return true. Otherwise, put
// the char back and return false.
bool check_next(Cursor& cursor, char match) noexcept {
    int c = cursor.getc();
    if (c == match)
        return true;
    // Otherwise put it back, read for next token.
    cursor.ungetc(c);
    return false;
}

// Check if the next character matches `match`. If it does, return `iftoken`,
// otherwise return `elsetoken`.
// This is used instead of a ternary because `iftoken` and `elsetoken` are
// generally distinct pointer types and therefore would need to be casted in a
// ternary expression.
syntax::Located if_next_else(
    Cursor& cursor,
    char match,
    syntax::Token* iftoken,
    syntax::Token* elsetoken,
    Pos start
) noexcept {
    if (check_next(cursor, match))
        return syntax::Located(iftoken, start);
    return syntax::Located(elsetoken, start);
}

// Encodes the given valid unicode code point into the given string.
void encode_unicode(int code_point, std::string& s) noexcept {
    // https://en.wikipedia.org/wiki/UTF-8#Encoding
    if (code_point < 0x0080) {
        // 1 Byte, which is the code point itself.
        s += static_cast<char>(code_point);
        return;
    }
    if (code_point < 0x0800) {
        // 2 Bytes
        // Mask for 5 least significant digits:
        //    1    F
        // 0001 1111

        // bit-or with:
        //    C    0
        // 1100 0000
        s += static_cast<char>(code_point >> 6 & 0x1F | 0xC0);

        // Note: & has higher precedence than |
    } else {
        if (code_point < 0x10000) {
            // 3 Bytes
            // Mask for 4 least significant bits:
            //    0    F
            // 0000 1111

            // bit-or with:
            //    E   0
            // 1110 000
            s += static_cast<char>(code_point >> 12 & 0x0F | 0xE0);
        } else {
            // 4 Bytes
            // Mask for 3 least significant bits:
            //    0    7
            // 0000 0111

            // bit-or with:
            //    F    0
            // 1111 0000
            s += static_cast<char>(code_point >> 18 & 0x07 | 0xF0);

            // Mask for 6 least significant bits:
            //    3    F
            // 0011 1111

            // bit-or with:
            //    8    0
            // 1000 0000
            s += static_cast<char>(code_point >> 12 & 0x3F | 0x80);
        }

        // This step is common to code points larger than 0x0800.
        // Mask for 6 least significant bits:
        //    3    F
        // 0011 1111

        // bit-or with:
        //    8    0
        // 1000 0000
        s += static_cast<char>(code_point >> 6 & 0x3F | 0x80);
    }
    // This step is common to code points larger than 0x0080.
    // Mask for 6 least significant bits:
    //    3    F
    // 0011 1111

    // bit-or with:
    //    8    0
    // 1000 0000
    s += static_cast<char>(code_point & 0x3F | 0x80);
}

// Handles an escaped character in a string.
ErrPtr handle_escape(std::string& s, Cursor& cursor, Pos start) {
    int c = cursor.getc();
    switch(c) {
    case 'a':
        s += '\a';
        break;
    case 'b':
        s += '\b';
        break;
    case 'e':
        s += '\e';
        break;
    case 'f':
        s += '\f';
        break;
    case 'n':
        s += '\n';
        break;
    case 'r':
        s += '\r';
        break;
    case 't':
        s += '\t';
        break;
    case 'v':
        s += '\v';
        break;
    case 'x': {
        // Read a byte determined by two hexadecimal digits.
        int x1 = cursor.getc();
        int x2 = cursor.getc();
        if (!(std::isxdigit(x1) && std::isxdigit(x2))) {
            // We found at least one invalid hexadecimal digit
            return ErrPtr(new BadByteErr(std::move(s), start));
        }
        s += hexvalue(x1) << 4 | hexvalue(x2);
    }
        break;
    case 'u': {
        // Read a unicode code point determined by four hexadecimal digits.
        int u1 = cursor.getc();
        int u2 = cursor.getc();
        int u3 = cursor.getc();
        int u4 = cursor.getc();
        if (!(
            std::isxdigit(u1) &&
            std::isxdigit(u2) &&
            std::isxdigit(u3) &&
            std::isxdigit(u4)
        )) {
            // We found at least one invalid hexadecimal digit.
            return ErrPtr(new BadUnicodeErr(std::move(s), start));
        }

        int code_point = (
            hexvalue(u1) << 12 |
            hexvalue(u2) << 8  |
            hexvalue(u3) << 4  |
            hexvalue(u4)
        );

        encode_unicode(code_point, s);
    }
        break;
    case 'U': {
        // Read a unicode code point determined by eight hexadecimal digits.
        int u1 = cursor.getc();
        int u2 = cursor.getc();
        int u3 = cursor.getc();
        int u4 = cursor.getc();
        int u5 = cursor.getc();
        int u6 = cursor.getc();
        int u7 = cursor.getc();
        int u8 = cursor.getc();

        if (!(
            std::isxdigit(u1) &&
            std::isxdigit(u2) &&
            std::isxdigit(u3) &&
            std::isxdigit(u4) &&
            std::isxdigit(u5) &&
            std::isxdigit(u6) &&
            std::isxdigit(u7) &&
            std::isxdigit(u8)
        )) {
            // We found at least one invalid hexadecimal digit.
            return ErrPtr(
                new BadLongUnicodeErr(std::move(s), start)
            );
        }

        int code_point = (
            hexvalue(u1) << 28 |
            hexvalue(u2) << 24 |
            hexvalue(u3) << 20 |
            hexvalue(u4) << 16 |
            hexvalue(u5) << 12 |
            hexvalue(u6) << 8  |
            hexvalue(u7) << 4  |
            hexvalue(u8)
        );

        if (code_point > 0x10FFFF)
            // Too large to be a valid unicode code point.
            return ErrPtr(new BadLongUnicodeErr(std::move(s), start));
        encode_unicode(code_point, s);
    }
        break;
    case '\'':
    case '"':
    case '\\':
        // Interpret a single quote, a double quote, and a backslash literally
        // after an escape.
        s += c;
        break;
    default:
        // An invalid character was escaped.
        return ErrPtr(new BadEscapeErr(std::move(s), start));
    }
    return nullptr;
}

// Logic for handling a space character.
// It is more complicated than other tokens because we have to consider
// indentation, leading spaces, and trailing spaces.
syntax::TokenRes handle_space(Cursor& cursor, Pos start) {
    // First, check to see if this is space at the start of a line.
    // If so, look for leading spaces and dedents.
    int c;
    bool at_start = cursor.pos.col == 2;
    std::uint32_t n = 1;
    while ((c = cursor.getc()) == ' ')
        n += 1;
    // Put back the extra character we read.
    cursor.ungetc(c);
    if (c == '\n' || c == EOF)
        // Space at the end of a line is not allowed.
        return ErrPtr(new TrailingSpaceErr(start));
    if (at_start)
        return syntax::Located(new syntax::LeadingSpace::Token(n), start);
    // Don't bother allocating new memory, only need to keep track for space at
    // start of a line to determine indentation.
    return syntax::Located(&syntax::Space::TOKEN, start);
}

// Read an identifier starting with `c`.
syntax::TokenRes next_id(int c, Cursor& cursor, Pos start) {
    auto s = std::string();

    // Use a do-while since we know the first character is a digit.
    do {
        s += static_cast<char>(c);
        c = cursor.getc();
    } while (std::isalnum(c) || c == '_');

    // We read an extra character, put it back.
    cursor.ungetc(c);

    // Check if ID matches a keyword. If so, return the corresponding token.
    auto it = KEYWORDS.find(s);
    if (it != KEYWORDS.end())
        return syntax::Located(it->second, start);
    
    // Otherwise, it's an ID.
    return syntax::Located(new syntax::ID::Token(std::move(s)), start);
}

// Reads an integer token starting with `c`.
syntax::TokenRes next_int(int c, Cursor& cursor, Pos start) {
    auto s = std::string();

    // Use a do while since we know the first character is a digit.
    do {
        s += static_cast<char>(c);
        c = cursor.getc();
    } while (std::isdigit(c));

    if (std::isalnum(c) || c == '_') {
        // characters outside 0-9 exist in the token. For now, just parse all
        // such number as "Mixed" and do fancy things later.
        do {
            s += c;
            c = cursor.getc();
        } while (std::isalnum(c) || c == '_');
        // We read an extra character, put it back.
        cursor.ungetc(c);
        return syntax::Located(new syntax::Mixed::Token(std::move(s)), start);
    }

    // We read an extra character, put it back.
    cursor.ungetc(c);
    return syntax::Located(new syntax::Int::Token(std::move(s)), start);
}

// Reads a string.
syntax::TokenRes next_str(Cursor& cursor, Pos start) {
    auto s = std::string();
    while (true) {
        int c = cursor.getc();
        switch(c) {
        case EOF:
        case '\n':
            // We read EOF or a new line without encountering double quotes.
            // Therefore we have an unclosed string.
            return ErrPtr(new UnclosedStrErr(std::move(s), start));
        case '"':
            // Found the closing double quote, return the result.
            return syntax::Located(
                new syntax::String::Token(std::move(s)), start
            );
        case '\\': {
            // Found a backslash, handle an escaped character.
            ErrPtr e = handle_escape(s, cursor, start);
            if (e)
                return e;
        }
            break;
        default:
            s += c;
        }
    }
}

// Reads the next token in the file tracked by `cursor`.
syntax::TokenRes next(Cursor& cursor) {
    Pos start = cursor.pos;
    int c = cursor.getc();
    switch (c) {
    case EOF:
        return syntax::Located(&syntax::EndOfFile::TOKEN, start);
    case ' ':
        return handle_space(cursor, start);
    case '\n':
        switch(cursor.peek()) {
        case ' ':
        case '\n':
            // Next line is not in global scope unless we are already in global
            // scope.
            return syntax::Located(&syntax::Newline::TOKEN, start);
        default:
            // In this case, if we are indented, then the next line fully
            // dedents since the character is not a valid space character and
            // therefore the next line is in the global scope.
            return syntax::Located(&syntax::NewlineThenGlobal::TOKEN, start);
        }
        return syntax::Located(&syntax::Newline::TOKEN, start);
    case '#':
        // Comment, skip rest of line.
        while (c != '\n' && c != EOF)
            c = cursor.getc();
        cursor.ungetc(c);
        return syntax::Located(&syntax::Hash::TOKEN, start);
    case ':':
        return syntax::Located(&syntax::Colon::TOKEN, start);
    case ',':
        return syntax::Located(&syntax::Comma::TOKEN, start);
    case '.':
        return syntax::Located(&syntax::Dot::TOKEN, start);
    case '=':
        return if_next_else(
            cursor,
            '=',
            &syntax::DoubleEquals::TOKEN,
            &syntax::Equals::TOKEN,
            start
        );
    case '!':
        if (check_next(cursor, '='))
            return syntax::Located(&syntax::BangEquals::TOKEN, start);
        return ErrPtr(new UnexpectedCharErr('!', start));
    case '<':
        c = cursor.getc();
        switch(c) {
        case '=':
            return syntax::Located(&syntax::LeftAngleEquals::TOKEN, start);
        case '<':
            return if_next_else(
                cursor,
                '=',
                &syntax::DoubleLeftAngleEquals::TOKEN,
                &syntax::DoubleLeftAngle::TOKEN,
                start
            );
        default:
            cursor.ungetc(c);
            return syntax::Located(&syntax::LeftAngle::TOKEN, start);
        }
    case '>':
        c = cursor.getc();
        switch(c) {
        case '=':
            return syntax::Located(&syntax::RightAngleEquals::TOKEN, start);
        case '>':
            return if_next_else(
                cursor,
                '=',
                &syntax::DoubleRightAngleEquals::TOKEN,
                &syntax::DoubleRightAngle::TOKEN,
                start
            );
        default:
            cursor.ungetc(c);
            return syntax::Located(&syntax::RightAngle::TOKEN, start);
        }
    case '+':
        return if_next_else(
            cursor, '=', &syntax::PlusEquals::TOKEN, &syntax::Plus::TOKEN, start
        );
    case '-':
        return if_next_else(
            cursor,
            '=',
            &syntax::MinusEquals::TOKEN,
            &syntax::Minus::TOKEN,
            start
        );
    case '*':
        return if_next_else(
            cursor, '=', &syntax::StarEquals::TOKEN, &syntax::Star::TOKEN, start
        );
    case '/':
        return if_next_else(
            cursor,
            '=',
            &syntax::SlashEquals::TOKEN,
            &syntax::Slash::TOKEN,
            start
        );
    case '%':
        return if_next_else(
            cursor,
            '=',
            &syntax::PercentEquals::TOKEN,
            &syntax::Percent::TOKEN,
            start
        );
    case '~':
        return syntax::Located(&syntax::Tilde::TOKEN, start);
    case '&':
        return if_next_else(
            cursor,
            '=',
            &syntax::AmpersandEquals::TOKEN,
            &syntax::Ampersand::TOKEN,
            start
        );
    case '|':
        return if_next_else(
            cursor, '=', &syntax::PipeEquals::TOKEN, &syntax::Pipe::TOKEN, start
        );
    case '^':
        return if_next_else(
            cursor,
            '=',
            &syntax::CarotEquals::TOKEN,
            &syntax::Carot::TOKEN,
            start
        );
    case '(':
        return syntax::Located(&syntax::Curved::LEFT_TOKEN, start);
    case ')':
        return syntax::Located(&syntax::Curved::RIGHT_TOKEN, start);
    case '[':
        return syntax::Located(&syntax::Square::LEFT_TOKEN, start);
    case ']':
        return syntax::Located(&syntax::Square::RIGHT_TOKEN, start);
    case '"':
        return next_str(cursor, start);
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return next_int(static_cast<char>(c), cursor, start);
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    case '_':
        return next_id(static_cast<char>(c), cursor, start);
    default:
        return ErrPtr(new UnexpectedCharErr(c, start));
    }
}

}

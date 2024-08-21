#pragma once

#include <cctype>
#include <cstdint>

#include <ostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include "dl/convert.hpp"
#include "dl/err.hpp"
#include "dl/lex/literalsuffix.hpp"
#include "dl/lex/tokendata.hpp"
#include "dl/lex/tokenid.hpp"
#include "dl/lex/tokenptr.hpp"
#include "dl/lex/tokenres.hpp"

namespace dl {

// Mapping from keywords to pointers to their corresponding token objects.
std::unordered_map<std::string_view, TokenID> KEYWORDS {
    {"and", TokenID::AND},
    {"break", TokenID::BREAK},
    {"by", TokenID::BY},
    {"case", TokenID::CASE},
    {"continue", TokenID::CONTINUE},
    {"def", TokenID::DEF},
    {"elif", TokenID::ELIF},
    {"else", TokenID::ELSE},
    {"false", TokenID::FALSE},
    {"for", TokenID::FOR},
    {"from", TokenID::FROM},
    {"if", TokenID::IF},
    {"in", TokenID::IN},
    {"interface", TokenID::INTERFACE},
    {"match", TokenID::MATCH},
    {"not", TokenID::NOT},
    {"none", TokenID::NONE},
    {"null", TokenID::NULL},
    {"or", TokenID::OR},
    {"raise", TokenID::RAISE},
    {"return", TokenID::RETURN},
    {"this", TokenID::THIS},
    {"to", TokenID::TO},
    {"true", TokenID::TRUE},
    {"type", TokenID::TYPE},
    {"type_interface", TokenID::TYPE_INTERFACE},
    {"vars", TokenID::VARS}
};

// Errors which occur inside a string.
struct InStrErr: Err {
    std::string s;
    std::uint32_t i;

    InStrErr(std::string&& s, std::uint32_t i) noexcept:
    s(std::move(s)), i(i) {}

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << s << ", " << i;
    }
};

struct CodePointTooLargeErr final: InStrErr {
    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "CodePointTooLargeErr";
    }
};

// Indicates that an invalid escape sequence was found.
struct InvalidEscapeErr final: InStrErr {
    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "InvalidEscapeErr";
    }
};

struct InvalidHexDigitErr final: InStrErr {
    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "InvalidHexDigitErr";
    }
};

// Indicates that a line ended with space which was unassociated with a string.
struct TrailingSpaceErr final: Err {
    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "TrailingSpaceErr";
    }
};

// Indicates that a string was started but was unclosed: the statement ended
// before the closing " was found.
struct UnclosedStrErr final: Err {
    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "UnclosedStrErr";
    }
};

// Indicates that an unexpected character was read.
struct UnexpectedCharErr final: Err {
    char c;

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << c;
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

// Encodes the given valid unicode code point into the given string.
void encode_unicode(std::uint32_t code_point, std::string& s) noexcept {
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

// Read 1 up to 8 hexadecimal digits into an std::uint32_t.
Res<std::uint32_t, ErrPtr> read_hex_digits(
    const std::string& src, std::uint32_t& i, int n
) {
    std::uint32_t res = 0;
    for (int i = 0; i < n; i++) {
        char x = raw[i++];
        if (!std::isxdigit(x))
            return ErrPtr(new InvalidHexDigitErr(std::move(src), i - 1));
        res <<= 4;
        res |= hexvalue(x);
    }
    return res;
}

bool seek_closing_quote(Cursor& cursor) {
    while(true) {
        switch(cursor.getc()) {
        case '"':
            return true;
        case EOF:
            return false;
        }
    }
}

// Handles an escaped character in a string.
ErrPtr handle_escape(const std::string& raw, std::string& s, std::uint32_t& i) {
    char c = raw[i++];
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
        Res<std::uint32_t, ErrPtr> res = read_hex_digits(raw, i, 2);
        if (res.is_err)
            return res.err;
        s += static_cast<char>(res.res);
    }
        break;
    case 'u': {
        // Read a unicode code point determined by four hexadecimal digits.
        Res<std::uint32_t, ErrPtr> res = read_hex_digits(raw, i, 4);
        if (res.is_err)
            return res.err;

        encode_unicode(res.res, s);
    }
        break;
    case 'U': {
        // Read a unicode code point determined by eight hexadecimal digits.
        Res<std::uint32_t, ErrPtr> res = read_hex_digits(raw, i, 8);
        if (res.is_err)
            return res.err;

        if (res.res > 0x10FFFF)
            // Too large to be a valid unicode code point.
            return ErrPtr(new CodePointTooLargeErr(std::move(raw), i - 8));

        encode_unicode(res.res, s);
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
        return ErrPtr(new InvalidEscapeErr(std::move(raw), i - 1));
    }
    // No errors encountered.
    return nullptr;
}

// Read an identifier starting with `c`.
Token next_alnum(int c, Cursor& cursor) {
    std::string res;

    // Use a do-while since we know the first character is alphanumeric already.
    do {
        res += static_cast<char>(c);
        c = cursor.getc();
    } while (std::isalnum(c) || c == '_');

    // We read an extra character, put it back.
    cursor.ungetc(c);

    // Check a keyword is matched. If so, return the corresponding token.
    auto it = KEYWORDS.find(res);
    if (it != KEYWORDS.end())
        return Token(it->second);
    
    // Otherwise, it's an alphanumeric.
    return Token(TokenID::ALNUM, std::move(res));
}

Res<std::string, ErrPtr> raw_str(Cursor& cursor) {
    std::string raw;
    int c = cursor.getc();
    while (c != '"' && (raw.size() == 1 || raw[raw.size() - 2] != '\\')) {
        switch(c) {
        case EOF:
        case '\n':
            // We read EOF or a new line without encountering double quotes.
            // Therefore we have an unclosed string.
            return ErrPtr(new UnclosedStrErr());
        default:
            raw += static_cast<char>(c);
            c = cursor.getc();
        }
    }
    return raw;
}

// Read a string.
TokenRes next_str(Cursor& cursor) {
    // Useful for error reporting to get the entire raw string first.
    Res<std::string, ErrPtr> raw_res = raw_str(cursor);
    if (raw_res.is_err)
        return raw_res.err;
    std::string raw = std::move(raw.res);

    // The string with escape sequences handled will be put here.
    std::string s;

    std::uint32_t i;
    while (i < raw.size()) {
        char c = raw[i++];
        if (c == '\\') {
            ErrPtr err = handle_escape(raw, s, i);
            if (err)
                return err;
        } else
            s += c;
    }
}

// Reads the next token in the cursor, putting any data associated with the token
// in `data`.
TokenRes next(Cursor& cursor) {
    int c = cursor.getc();
    switch (c) {
    case EOF:
        return Token(TokenID::END_OF_FILE);
    case ' ':
        std::uint32_t space_count = 1;
        while ((c = cursor.getc()) == ' ')
            space_count++;
        // Unget the last character we read, which was not a space.
        cursor.ungetc(c);
        if (c == '\n') {
            // If the last character read was a newline, we found space at the
            // end of a line.
            return ErrPtr(new TrailingSpaceErr());
        }
        return Token(TokenID::SPACE, space_count);
    case '\n':
        return Token(TokenID::NEWLINE);
    case '#':
        // Comment, skip rest of line.
        while (c != '\n' && c != EOF)
            c = cursor.getc();
        cursor.ungetc(c);
        return Token(TokenID::HASH);
    case ':':
        return Token(TokenID::COLON);
    case ',':
        return Token(TokenID::COMMA);
    case '.':
        return Token(TokenID::DOT);
    case '=':
        cursor.check_next('=') ? TokenID::DOUBLE_EQUALS: TokenID::EQUALS;
    case '!':
        if (!check_next(cursor, '='))
            return ErrPtr(new UnexpectedCharErr(c));
        return Token(TokenID::BANG_EQUALS);
    case '<':
        c = cursor.getc();
        switch(c) {
        case '=':
            return Token(TokenID::LEFT_ANGLE_EQUALS);
        case '<':
            return check_next(cursor, '=') ?
                TokenID::DOUBLE_LEFT_ANGLE_EQUALS: TokenID::DOUBLE_LEFT_ANGLE;
        default:
            cursor.ungetc(c);
            return Token(TokenID::LEFT_ANGLE);
        }
    case '>':
        c = cursor.getc();
        switch(c) {
        case '=':
            return Token(TokenID::RIGHT_ANGLE_EQUALS);
        case '>':
            return check_next(cursor, '=') ?
                TokenID::DOUBLE_RIGHT_ANGLE_EQUALS: TokenID::DOUBLE_RIGHT_ANGLE;
        default:
            cursor.ungetc(c);
            return Token(TokenID::RIGHTANGLE);
        }
    case '+':
        return check_next(cursor, '=') ? TokenID::PLUS_EQUALS: TokenID::PLUS;
    case '-':
        c = cursor.getc();
        switch(c) {
        case '=':
            return Token(TokenID::MINUS_EQUALS);
        case '>':
            return Token(TokenID::MINUS_RIGHT_ANGLE);
        default:
            cursor.ungetc(c);
            return Token(TokenID::MINUS);
        }
    case '*':
        c = cursor.getc();
        switch(c) {
        case '=':
            return Token(TokenID::STAR_EQUALS);
        case '*':
            return check_next(cursor, '=') ?
                TokenID::DOUBLE_STAR_EQUALS: TokenID::DOUBLE_STAR;
        default:
            cursor.ungetc(c);
            return Token(TokenID::STAR);
        }
        return check_next(cursor, '=') ? TokenID::STAR_EQUALS: TokenID::STAR;
    case '/':
        return check_next(cursor, '=') ? TokenID::SLASH_EQUALS: TokenID::SLASH;
    case '%':
        return check_next(cursor, '=') ?
            TokenID::PERCENT_EQUALS: TokenID::PERCENT;
    case '~':
        return Token(TokenID::TILDE);
    case '&':
        return check_next(cursor, '=') ?
            TokenID:AMPERSAND_EQUALS: TokenID::AMPERSAND;
    case '|':
        return check_next(cursor, '=') ? TokenID::PIPE_EQUALS: TokenID::PIPE;
    case '^':
        return check_next(cursor, '=') ? TokenID::CAROT_EQUALS: TokenID::CAROT;
    case '(':
        return Token(TokenID::LEFT_CURVED);
    case ')':
        return Token(TokenID::RIGHT_CURVED);
    case '[':
        return Token(TokenID::LEFT_SQUARE);
    case ']':
        return Token(TokenID::RIGHT_SQUARE);
    case '"':
        return next_str(cursor, data);
    default:
        if (std::isalnum(c))
            return next_alnum(cursor);
        return ErrPtr(new UnexpectedCharErr(c));
    }
}

}

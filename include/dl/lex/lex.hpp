#pragma once

#include <cstdint>

#include <locale>
#include <ostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include "dl/convert.hpp"
#include "dl/err.hpp"
#include "dl/lex/cursor.hpp"
#include "dl/lex/literalsuffix.hpp"
#include "dl/lex/token.hpp"
#include "dl/lex/tokenid.hpp"
#include "dl/pos.hpp"
#include "dl/res.hpp"

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
    {"except", TokenID::EXCEPT},
    {"false", TokenID::FALSE},
    {"finally", TokenID::FINALLY},
    {"for", TokenID::FOR},
    {"from", TokenID::FROM},
    {"if", TokenID::IF},
    {"in", TokenID::IN},
    {"match", TokenID::MATCH},
    {"not", TokenID::NOT},
    {"none", TokenID::NONE},
    {"null", TokenID::NULL_},
    {"or", TokenID::OR},
    {"raise", TokenID::RAISE},
    {"return", TokenID::RETURN},
    {"this", TokenID::THIS},
    {"to", TokenID::TO},
    {"true", TokenID::TRUE},
    {"try", TokenID::TRY},
    {"type", TokenID::TYPE},
    {"vars", TokenID::VARS},
    {"while", TokenID::WHILE}
};

// Locale to use (C default).
const std::locale LOCALE("C");

// Errors which occur at a particular position in a file.
struct PosErr: Err {
    Pos pos;

    PosErr(const Pos& pos) noexcept: pos(pos) {}

    virtual bool equals(const Err& that) const noexcept override {
        return pos == dynamic_cast<const PosErr&>(that).pos;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << pos;
    }
};

struct InvalidCharErr final: PosErr {
    InvalidCharErr(const Pos& pos) noexcept: PosErr(pos) {}

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "InvalidCharErr";
    }
};

// Indicates that an invalid escape sequence was found.
struct InvalidEscapeErr final: PosErr {
    InvalidEscapeErr(const Pos& pos) noexcept: PosErr(pos) {}

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "InvalidEscapeErr";
    }
};

struct InvalidHexDigitErr final: PosErr {
    InvalidHexDigitErr(const Pos& pos) noexcept: PosErr(pos) {}

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "InvalidHexDigitErr";
    }
};

struct InvalidNumericLiteralErr final: PosErr {
    InvalidNumericLiteralErr(const Pos& pos) noexcept: PosErr(pos) {}

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "InvalidNumericLiteralErr";
    }
};

struct InvalidUnicodeCodePointErr final: PosErr {
    InvalidUnicodeCodePointErr(const Pos& pos) noexcept: PosErr(pos) {}

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "InvalidUnicodeCodePointErr";
    }
};

struct LeadingZeroesErr final: PosErr {
    LeadingZeroesErr(const Pos& pos) noexcept: PosErr(pos) {}

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "LeadingZeroesErr";
    }
};

struct OutOfRangeErr final: PosErr {
    OutOfRangeErr(const Pos& pos) noexcept: PosErr(pos) {}

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "OutOfRangeErr";
    }
};

// Indicates that a line ended with space which was unassociated with a string.
struct TrailingSpaceErr final: PosErr {
    TrailingSpaceErr(const Pos& pos) noexcept: PosErr(pos) {}

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "TrailingSpaceErr";
    }
};

// Indicates that a char was started but was unclosed: the statement ended
// before the closing ' was found.
struct UnclosedCharErr final: PosErr {
    UnclosedCharErr(const Pos& pos) noexcept: PosErr(pos) {}

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "UnclosedCharErr";
    }
};

// Indicates that a string was started but was unclosed: the statement ended
// before the closing " was found.
struct UnclosedStrErr final: PosErr {
    UnclosedStrErr(const Pos& pos) noexcept: PosErr(pos) {}

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "UnclosedStrErr";
    }
};

// Indicates that an unexpected character was read.
struct UnexpectedCharErr final: PosErr {
    std::int32_t c;

    UnexpectedCharErr(
        const Pos& pos, std::int32_t c
    ) noexcept: PosErr(pos), c(c) {}

    virtual bool equals(const Err& that) const noexcept override {
        return PosErr::equals(that) &&
        c == dynamic_cast<const UnexpectedCharErr&>(that).c;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return PosErr::out_data(os) << ", " << c;
    }

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "UnexpectedCharErr";
    }
};

bool is_alnum(std::int32_t c) noexcept {
    char chr = char(c);
    if (chr != c)
        return false;
    return std::isalnum(chr, LOCALE);
}

bool is_digit(std::int32_t c) noexcept {
    char chr = char(c);
    if (chr != c)
        return false;
    return std::isdigit(chr, LOCALE);
}

bool is_hex_digit(std::int32_t c) noexcept {
    char chr = char(c);
    if (chr != c)
        return false;
    return std::isxdigit(chr, LOCALE);
}

// Computes the numeric value of a hexadecimal digit.
constexpr char hexvalue(std::int32_t c) noexcept {
    c = std::tolower(char(c), LOCALE);
    return c - '0' - ('a' - '9' - 1) * (c >= 'a');
}

// Check if the next char matches `match`. If so, return true. Otherwise, put
// the char back and return false.
bool check_next(Cursor& cursor, std::int32_t match) noexcept {
    std::int32_t c = cursor.getc();
    if (c == match)
        return true;
    // Otherwise put it back, read for next token.
    cursor.ungetc();
    return false;
}

// Encodes the given valid unicode code point into the given string.
void encode_unicode(std::int32_t code_point, std::string& s) noexcept {
    // https://en.wikipedia.org/wiki/UTF-8#Description
    if (code_point < 0x0080) {
        // 1 Byte, which is the code point itself.
        s += char(code_point);
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
        s += char(code_point >> 6 & 0x1F | 0xC0);

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
            s += char(code_point >> 12 & 0x0F | 0xE0);
        } else {
            // 4 Bytes
            // Mask for 3 least significant bits:
            //    0    7
            // 0000 0111

            // bit-or with:
            //    F    0
            // 1111 0000
            s += char(code_point >> 18 & 0x07 | 0xF0);

            // Mask for 6 least significant bits:
            //    3    F
            // 0011 1111

            // bit-or with:
            //    8    0
            // 1000 0000
            s += char(code_point >> 12 & 0x3F | 0x80);
        }

        // This step is common to code points larger than 0x0800.
        // Mask for 6 least significant bits:
        //    3    F
        // 0011 1111

        // bit-or with:
        //    8    0
        // 1000 0000
        s += char(code_point >> 6 & 0x3F | 0x80);
    }
    // This step is common to code points larger than 0x0080.
    // Mask for 6 least significant bits:
    //    3    F
    // 0011 1111

    // bit-or with:
    //    8    0
    // 1000 0000
    s += char(code_point & 0x3F | 0x80);
}

// Read 1 up to 8 hexadecimal digits into an std::uint32_t.
Res<std::int32_t> read_hex_digits(Cursor& cursor, int n) {
    std::int32_t res = 0;
    for (int j = 0; j < n; j++) {
        std::int32_t x = cursor.getc();
        if (!is_hex_digit(x))
            return ErrPtr(
                new InvalidHexDigitErr(Pos(cursor.pos.line, cursor.pos.col - 1))
            );
        res <<= 4;
        res |= hexvalue(x);
    }
    return res;
}

Res<std::int32_t> escaped_char(Cursor& cursor) {
    std::int32_t c = cursor.getc();
    switch(c) {
    case 'a':
        return std::int32_t('\a');
    case 'b':
        return std::int32_t('\b');
    case 'e':
        return std::int32_t('\e');
    case 'f':
        return std::int32_t('\f');
    case 'n':
        return std::int32_t('\n');
    case 'r':
        return std::int32_t('\r');
    case 't':
        return std::int32_t('\t');
    case 'v':
        return std::int32_t('\v');
    case 'x':
        // Read a byte determined by two hexadecimal digits.
        return read_hex_digits(cursor, 2);
    case 'u': {
        // Read a unicode code point determined by four hexadecimal digits.
        Res<std::int32_t> res = read_hex_digits(cursor, 4);
        if (res.is_err)
            return res;

        if (res.res >= 0xD800 && res.res <= 0xDFFF)
            // Surrogate
            return ErrPtr(new InvalidUnicodeCodePointErr(
                Pos(cursor.pos.line, cursor.pos.col - 4)
            ));
        return res;
    }
    case 'U': {
        // Read a unicode code point determined by eight hexadecimal digits.
        Res<std::int32_t, ErrPtr> res = read_hex_digits(cursor, 8);
        if (res.is_err)
            return res;

        if (res.res > 0x10FFFF || (res.res >= 0xD800 && res.res <= 0xDFFF))
            // Surrogate or code point too large.
            return ErrPtr(new InvalidUnicodeCodePointErr(
                Pos(cursor.pos.line, cursor.pos.col - 8)
            ));
        return res;
    }
    case '\'':
    case '"':
    case '\\':
        // Interpret a single quote, a double quote, and a backslash literally
        // after an escape.
        return c;
    default:
        // An invalid character was escaped.
        return ErrPtr(
            new InvalidEscapeErr(Pos(cursor.pos.line, cursor.pos.col - 2))
        );
    }
}

void read_alnum(std::int32_t c, Cursor& cursor, std::string& res) {
    while (is_alnum(c) || c == '_') {
        res += char(c);
        c = cursor.getc();
    }
    cursor.ungetc();
}

template<typename T>
Res<Token> read_number(
    Pos start, const char* begin, const char* expected_end, int base
) noexcept {
    char* end;
    errno = 0;
    T res = strto<T>(begin, &end, base);
    if (errno == ERANGE)
        return ErrPtr(new OutOfRangeErr(start));
    if (end == begin || end < expected_end)
        return ErrPtr(new InvalidNumericLiteralErr(start));
    return Token(TokenID::NUMBER, res);
}

Res<Token> read_float(Pos start, const std::string& res) noexcept {
    const char* begin = &res[0];
    LiteralSuffix suffix = lit_suffix(res);
    const char* end = begin + res.size() - lit_suffix_len(suffix);

    switch(suffix) {
    case LiteralSuffix::F:
    case LiteralSuffix::F64:
    case LiteralSuffix::NONE:
        return read_number<double>(start, begin, end, 10);
    case LiteralSuffix::F32:
        return read_number<float>(start, begin, end, 10);
    default:
        return ErrPtr(new InvalidNumericLiteralErr(start));
    }
}

Res<Token> read_exponent_then_float(
    Cursor& cursor, Pos start, std::string& res
) noexcept {
    std::int32_t c = cursor.getc();
    if (c == '-' || is_alnum(c) || c == '_')
        res += char(c);
    else {
        // Invalid: floating point literal can't end with e.
        cursor.ungetc();
        return ErrPtr(new InvalidNumericLiteralErr(start));
    }
    // Eat the remaining alphanumeric characters.
    c = cursor.getc();
    read_alnum(c, cursor, res);

    return read_float(start, res);
}

bool seek_quote(Cursor& cursor, std::int32_t quote) {
    while (true) {
        std::int32_t c = cursor.getc();
        if (c == quote)
            return true;
        if (c == '\n' || c == EOF)
            return false;
    }
}

Res<Token> next_char(Cursor& cursor) {
    // Start at first single-quote.
    Pos start = Pos(cursor.pos.line, cursor.pos.col - 1);
    std::int32_t c = cursor.getc();

    switch(c) {
    case '\'':
        return ErrPtr(new InvalidCharErr(start));
    case '\\': {
        Res<std::int32_t> res = escaped_char(cursor);
        if (res.is_err) {
            // Prioritize UnclosedCharErr if it applies
            if (seek_quote(cursor, '\''))
                return std::move(res.err);
            return ErrPtr(new UnclosedCharErr(start));
        }
        c = res.res;
        break;
    }
    case '\n':
    case EOF:
        return ErrPtr(new UnclosedCharErr(start)); 
    }
    
    switch(cursor.getc()) {
    case '\'':
        return Token(TokenID::CHAR, c);
    case '\n':
    case EOF:
        return ErrPtr(new UnclosedCharErr(start));
    default:
        if (seek_quote(cursor, '\''))
            return ErrPtr(new InvalidCharErr(start));
        return ErrPtr(new UnclosedCharErr(start));
    }
}

// Read an identifier starting with `c`.
Token next_id(int c, Cursor& cursor) {
    std::string res;

    read_alnum(c, cursor, res);

    // Check a keyword is matched. If so, return the corresponding token.
    auto it = KEYWORDS.find(res);
    if (it != KEYWORDS.end())
        return Token(it->second);
    
    // Otherwise, it's an identifier.
    return Token(TokenID::ID, std::move(res));
}

// Read the next numeric literal.
// This function is very careful not to eat a `.` or a `-` unless the preceding
// characters are thus far indiciate a valid floating point literal.
// Additionally, any concatenated alphanumeric or underscore characters are
// read, not just digits, even if the result is invalid.
Res<Token> next_number(std::int32_t c, Cursor& cursor) {
    Pos start = Pos(cursor.pos.line, cursor.pos.col - 1);
    std::string res;

    int base = 10;

    if (c == '0') {
        c = cursor.getc();
        switch(c) {
        case 'o':
        case 'O':
            base = 8;
            c = cursor.getc();
            break;
        case 'x':
        case 'X':
            base = 16;
            c = cursor.getc();
            break;
        default:
            res += '0';
        }
    }

    if (base == 10) {
        while (is_digit(c)) {
            res += char(c);
            c = cursor.getc();
        }
        if (c == 'e' || c == 'E') {
            // Floating point literal with exponent but no fractional part.
            res += char(c);
            return read_exponent_then_float(cursor, start, res);
        }
        if (c == '.') {
            c = cursor.getc();
            if (is_digit(c)) {
                // Floating point literal.
                res += '.';
                do {
                    res += char(c);
                    c = cursor.getc();
                } while (is_digit(c));

                // Check for exponent.
                if (c == 'e' || c == 'E') {
                    res += char(c);
                    return read_exponent_then_float(cursor, start, res);
                }

                // Otherwise, eat remaining alphanum characters and then read
                // the float.
                read_alnum(c, cursor, res);
                return read_float(start, res);
            }
            // Not a necessarily a floating point literal, gotta put back two
            // chars.
            cursor.ungetc();
            cursor.ungetc();

    // By this point, we know the literal contains no decimal point and is not a
    // valid floating-point literal with an exponent.
    // Because of this, no minus sign or decimal point can be part of the
    // literal, and we are safe to read the remaining alphanumeric characters
    // without risk of reading a `.` or a `-` into an invalid float.
        } else
            read_alnum(c, cursor, res);
    } else
        read_alnum(c, cursor, res);

    if (res.size() > 1 && res[0] == '0' && is_digit(res[1]))
        return ErrPtr(new LeadingZeroesErr(start));

    LiteralSuffix suffix = lit_suffix(res);
    const char* begin = &res[0];
    const char* expected_end = begin + res.size() - lit_suffix_len(suffix);

    switch(suffix) {
    case LiteralSuffix::NONE:
    case LiteralSuffix::S:
    case LiteralSuffix::S32:
        return read_number<std::int32_t>(start, begin, expected_end, base);
    case LiteralSuffix::S8:
        return read_number<std::int8_t>(start, begin, expected_end, base);
    case LiteralSuffix::S16:
        return read_number<std::int16_t>(start, begin, expected_end, base);
    case LiteralSuffix::S64:
        return read_number<std::int64_t>(start, begin, expected_end, base);
    case LiteralSuffix::U:
    case LiteralSuffix::U32:
        return read_number<std::uint32_t>(start, begin, expected_end, base);
    case LiteralSuffix::U8:
        return read_number<std::uint8_t>(start, begin, expected_end, base);
    case LiteralSuffix::U16:
        return read_number<std::uint16_t>(start, begin, expected_end, base);
    case LiteralSuffix::U64:
        return read_number<std::uint64_t>(start, begin, expected_end, base);
    case LiteralSuffix::F:
    case LiteralSuffix::F64:
        if (base == 16) {
            // Suffix is valid hex, let it be read as such.
            expected_end += lit_suffix_len(suffix);
            return read_number<std::int32_t>(start, begin, expected_end, base);
        }
        else if (base == 8)
            // Octal floating point not allowed.
            return ErrPtr(new InvalidNumericLiteralErr(start));
        return read_number<double>(start, begin, expected_end, base);
    case LiteralSuffix::F32:
        if (base == 16) {
            expected_end += 3;
            return read_number<std::int32_t>(start, begin, expected_end, base);
        }
        else if (base == 8)
            return ErrPtr(new InvalidNumericLiteralErr(start));
        return read_number<float>(start, begin, expected_end, base);
    }
}

// Read a string.
Res<Token> next_str(Cursor& cursor) {
    Pos start = Pos(cursor.pos.line, cursor.pos.col - 1);
    std::string s;

    while (true) {
        std::int32_t c = cursor.getc();
        switch(c) {
        case '\\': {
            Res<std::int32_t> code_point = escaped_char(cursor);
            if (code_point.is_err) {
                // Prioritize UnclosedStrErr
                if (seek_quote(cursor, '"'))
                    return std::move(code_point.err);
                return ErrPtr(new UnclosedStrErr(start));
            }
            encode_unicode(code_point.res, s);
            break;
        }
        case '"':
            return Token(TokenID::STRING, s);
        case '\n':
        case EOF:
            return ErrPtr(new UnclosedStrErr(start));
        default:
            encode_unicode(c, s);
        }

    }
    return Token(TokenID::STRING, std::move(s));
}

// Reads the next token in the cursor, putting any data associated with the
// token in its `data` field.
Res<Token> next_token(Cursor& cursor) {
    Pos start = cursor.pos;
    std::int32_t c = cursor.getc();
    switch (c) {
    case EOF:
        return Token(TokenID::END_OF_FILE);
    case ' ': {
        std::uint32_t space_count = 1;
        while ((c = cursor.getc()) == ' ')
            space_count++;
        // Unget the last character we read, which was not a space.
        cursor.ungetc();
        if (c == '\n' || c == EOF) {
            // If the last character read was a newline or EOF, we found space
            // at the end of a line.
            return ErrPtr(new TrailingSpaceErr(start));
        }
        return Token(TokenID::SPACE, space_count);
    }
    case '\n':
        return Token(TokenID::NEWLINE);
    case '#':
        // Comment, skip rest of line.
        while (c != '\n' && c != EOF)
            c = cursor.getc();
        cursor.ungetc();
        return Token(TokenID::HASH);
    case ':':
        return Token(
            check_next(cursor, '=') ? TokenID::COLON_EQUALS: TokenID::COLON
        );
    case ',':
        return Token(TokenID::COMMA);
    case '.':
        return Token(TokenID::DOT);
    case '=':
        return Token(
            check_next(cursor, '=') ? TokenID::DOUBLE_EQUALS: TokenID::EQUALS
        );
    case '!':
        if (!check_next(cursor, '='))
            return ErrPtr(new UnexpectedCharErr(start, c));
        return Token(TokenID::BANG_EQUALS);
    case '<':
        c = cursor.getc();
        switch(c) {
        case '=':
            return Token(TokenID::LEFT_ANGLE_EQUALS);
        case '<':
            return Token(
                check_next(cursor, '=') ?
                TokenID::DOUBLE_LEFT_ANGLE_EQUALS: TokenID::DOUBLE_LEFT_ANGLE
            );
        default:
            cursor.ungetc();
            return Token(TokenID::LEFT_ANGLE);
        }
    case '>':
        c = cursor.getc();
        switch(c) {
        case '=':
            return Token(TokenID::RIGHT_ANGLE_EQUALS);
        case '>':
            return Token(
                check_next(cursor, '=') ?
                TokenID::DOUBLE_RIGHT_ANGLE_EQUALS: TokenID::DOUBLE_RIGHT_ANGLE
            );
        default:
            cursor.ungetc();
            return Token(TokenID::RIGHT_ANGLE);
        }
    case '+':
        return Token(
            check_next(cursor, '=') ? TokenID::PLUS_EQUALS: TokenID::PLUS
        );
    case '-':
        c = cursor.getc();
        switch(c) {
        case '=':
            return Token(TokenID::MINUS_EQUALS);
        case '>':
            return Token(TokenID::MINUS_RIGHT_ANGLE);
        default:
            cursor.ungetc();
            return Token(TokenID::MINUS);
        }
    case '*':
        c = cursor.getc();
        switch(c) {
        case '=':
            return Token(TokenID::STAR_EQUALS);
        case '*':
            return Token(
                check_next(cursor, '=') ?
                TokenID::DOUBLE_STAR_EQUALS: TokenID::DOUBLE_STAR
            );
        default:
            cursor.ungetc();
            return Token(TokenID::STAR);
        }
        return Token(
            check_next(cursor, '=') ? TokenID::STAR_EQUALS: TokenID::STAR
        );
    case '/':
        return Token(
            check_next(cursor, '=') ? TokenID::SLASH_EQUALS: TokenID::SLASH
        );
    case '%':
        return Token(
            check_next(cursor, '=') ?
            TokenID::PERCENT_EQUALS: TokenID::PERCENT
        );
    case '~':
        return Token(TokenID::TILDE);
    case '&':
        return Token(
            check_next(cursor, '=') ?
            TokenID::AMPERSAND_EQUALS: TokenID::AMPERSAND
        );
    case '|':
        return Token(
            check_next(cursor, '=') ? TokenID::PIPE_EQUALS: TokenID::PIPE
        );
    case '^':
        return Token(
            check_next(cursor, '=') ? TokenID::CAROT_EQUALS: TokenID::CAROT
        );
    case '(':
        return Token(TokenID::LEFT_CURVED);
    case ')':
        return Token(TokenID::RIGHT_CURVED);
    case '[':
        return Token(TokenID::LEFT_SQUARE);
    case ']':
        return Token(TokenID::RIGHT_SQUARE);
    case '"':
        return next_str(cursor);
    case '\'':
        return next_char(cursor);
    default:
        if (is_digit(c))
            return next_number(c, cursor);
        if (is_alnum(c))
            return next_id(c, cursor);
        return ErrPtr(new UnexpectedCharErr(start, c));
    }
}

}

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
#include "dl/span.hpp"

namespace dl {

// Mapping from keywords to pointers to their corresponding token objects.
std::unordered_map<std::string_view, TokenID> KEYWORDS {
    {"and", TokenID::AND},
    {"as", TokenID::AS},
    {"break", TokenID::BREAK},
    {"case", TokenID::CASE},
    {"continue", TokenID::CONTINUE},
    {"def", TokenID::DEF},
    {"elif", TokenID::ELIF},
    {"else", TokenID::ELSE},
    {"except", TokenID::EXCEPT},
    {"false", TokenID::FALSE},
    {"finally", TokenID::FINALLY},
    {"for", TokenID::FOR},
    {"if", TokenID::IF},
    {"in", TokenID::IN},
    {"match", TokenID::MATCH},
    {"not", TokenID::NOT},
    {"none", TokenID::NONE},
    {"null", TokenID::NULL_},
    {"or", TokenID::OR},
    {"_", TokenID::PLACEHOLDER},
    {"raise", TokenID::RAISE},
    {"return", TokenID::RETURN},
    {"this", TokenID::THIS},
    {"true", TokenID::TRUE},
    {"try", TokenID::TRY},
    {"while", TokenID::WHILE}
};

// Locale to use (C default).
const std::locale LOCALE("C");

struct InvalidCharErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new InvalidCharErr(*this));
    }

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "InvalidCharErr";
    }
};

// Indicates that an invalid escape sequence was found.
struct InvalidEscapeErr final: SourcedErr {
    InvalidEscapeErr(Span src) noexcept: SourcedErr(src) {}

    virtual ErrPtr copy() const override {
        return ErrPtr(new InvalidEscapeErr(*this));
    }

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "InvalidEscapeErr";
    }
};

struct InvalidFloatTailErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new InvalidFloatTailErr(*this));
    }

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "InvalidFloatTailErr";
    }
};

struct InvalidHexDigitErr final: SourcedErr {
    InvalidHexDigitErr(Span src) noexcept: SourcedErr(src) {}

    virtual ErrPtr copy() const override {
        return ErrPtr(new InvalidHexDigitErr(*this));
    }

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "InvalidHexDigitErr";
    }
};

struct InvalidUnicodeCodePointErr final: SourcedErr {
    InvalidUnicodeCodePointErr(Span src) noexcept: SourcedErr(src) {}

    virtual ErrPtr copy() const override {
        return ErrPtr(new InvalidUnicodeCodePointErr(*this));
    }

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "InvalidUnicodeCodePointErr";
    }
};

struct LeadingZeroesErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new LeadingZeroesErr(*this));
    }

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "LeadingZeroesErr";
    }
};

// Indicates that a line ended with space which was unassociated with a string.
struct TrailingSpaceErr final: SourcedErr {
    TrailingSpaceErr(Span src): SourcedErr(src) {}

    virtual ErrPtr copy() const override {
        return ErrPtr(new TrailingSpaceErr(*this));
    }

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "TrailingSpaceErr";
    }
};

// Indicates that a char was started but was unclosed: the statement ended
// before the closing ' was found.
struct UnclosedCharErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new UnclosedCharErr(*this));
    }

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "UnclosedCharErr";
    }
};

// Indicates that a string was started but was unclosed: the statement ended
// before the closing " was found.
struct UnclosedStrErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new UnclosedStrErr(*this));
    }

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "UnclosedStrErr";
    }
};

// Indicates that an unexpected character was read.
struct UnexpectedCharErr final: SourcedErr {
    std::int32_t c;

    UnexpectedCharErr(std::int32_t c, Span src)
    noexcept: SourcedErr(src), c(c) {}

    virtual ErrPtr copy() const override {
        return ErrPtr(new UnexpectedCharErr(*this));
    }

    virtual bool equals(const Err& that) const noexcept override {
        return SourcedErr::equals(that) &&
        c == dynamic_cast<const UnexpectedCharErr&>(that).c;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << c;
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

// Read 1 to 8 hexadecimal digits into an std::int32_t.
Res<std::int32_t> read_hex_digits(Cursor& cursor, int n) {
    std::int32_t res = 0;
    for (int j = 0; j < n; j++) {
        std::int32_t x = cursor.getc();
        if (!is_hex_digit(x))
            return ErrPtr(new InvalidHexDigitErr(Span(
                Pos(cursor.pos.line, cursor.pos.col - 1)
            )));
        res <<= 4;
        res |= hexvalue(x);
    }
    return res;
}

Res<std::int32_t> escaped_char(Cursor& cursor) {
    std::int32_t c = cursor.getc();
    switch(c) {
    case '0':
        return std::int32_t(0);
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
                Span(cursor.pos.line, cursor.pos.col - 6, 6)
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
                Span(cursor.pos.line, cursor.pos.col - 10, 10)
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
        return ErrPtr(new InvalidEscapeErr(Span(
            cursor.pos.line, cursor.pos.col - 2, 2
        )));
    }
}

void read_alnum(std::int32_t c, Cursor& cursor, std::string& res) {
    while (is_alnum(c) || c == '_') {
        res += char(c);
        c = cursor.getc();
    }
    cursor.ungetc();
}

Token read_float_tail(Cursor& cursor, std::string&& res) noexcept {
    std::int32_t c = cursor.getc();
    if (c == '-') {
        res += char(c);
        c = cursor.getc();
    }

    if (!is_digit(c)) {
        // Invalid: floating point literal can't end with e or -.
        read_alnum(c, cursor, res);
        return Token(TokenID::ERROR, ErrPtr(new InvalidFloatTailErr()));
    }

    while(is_digit(c)) {
        res += char(c);
        c = cursor.getc();
    }

    if (is_alnum(c) || c == '_') {
        std::uint32_t alnum_start = res.size();
        read_alnum(c, cursor, res);
        LiteralSuffix suffix = lit_suffix(res);
        if (res.size() - alnum_start != lit_suffix_len(suffix)) 
            return Token(TokenID::ERROR, ErrPtr(new InvalidFloatTailErr()));
    } else
        // Got one extra character, put it back.
        cursor.ungetc();

    return Token(TokenID::FLOAT_TAIL, std::move(res));
}

template<typename T>
Token read_number_token(
    const char* begin, const char* expected_end, int base
) {
    Res<T> res = read_number<T>(begin, expected_end, base);
    if (res.is_err)
        return Token(TokenID::ERROR, std::move(res.err));
    return Token(TokenID::NUMBER, res.res);
}

bool seek_quote(Cursor& cursor, std::int32_t quote) {
    while (true) {
        std::int32_t c = cursor.getc();
        if (c == quote)
            return true;
        if (c == '\n' || c == EOF) {
            cursor.ungetc();
            return false;
        }
    }
}

Token next_char(Cursor& cursor) {
    std::int32_t c = cursor.getc();

    switch(c) {
    case '\'':
        return Token(TokenID::ERROR, ErrPtr(new InvalidCharErr()));
    case '\\': {
        Res<std::int32_t> res = escaped_char(cursor);
        if (res.is_err) {
            // Prioritize UnclosedCharErr if it applies
            if (seek_quote(cursor, '\''))
                return Token(TokenID::ERROR, std::move(res.err));
            return Token(TokenID::ERROR, ErrPtr(new UnclosedCharErr()));
        }
        c = res.res;
        break;
    }
    case '\n':
    case EOF:
        cursor.ungetc();
        return Token(TokenID::ERROR, ErrPtr(new UnclosedCharErr()));
    }
    
    switch(cursor.getc()) {
    case '\'':
        return Token(TokenID::CHAR, c);
    case '\n':
    case EOF:
        cursor.ungetc();
        return Token(TokenID::ERROR, ErrPtr(new UnclosedCharErr()));
    default:
        if (seek_quote(cursor, '\''))
            return Token(TokenID::ERROR, ErrPtr(new InvalidCharErr()));
        return Token(TokenID::ERROR, ErrPtr(new UnclosedCharErr()));
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
// characters thus far indiciate a valid floating point literal.
// Additionally, any concatenated alphanumeric or underscore characters are
// read, not just digits, even if the result is invalid.
Token next_number(std::int32_t c, Cursor& cursor) {
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
            // Something like 123e45
            // Note that, because tokens are always split on dots, we might be
            // missing the integral part, like 89.123e45. These parts are
            // combined into a floating-point number later by the interpreter.
            res += char(c);
            return read_float_tail(cursor, std::move(res));
        }
    }

    bool has_alnum = false;
    if (is_alnum(c)) {
        has_alnum = true;
        // Eat the remaining alphanumeric characters.
        read_alnum(c, cursor, res);
    } else
        // Put back the character we read.
        cursor.ungetc();
    if (res.size() > 1 && res[0] == '0' && is_digit(res[1]))
        return Token(TokenID::ERROR, ErrPtr(new LeadingZeroesErr()));

    LiteralSuffix suffix = lit_suffix(res);
    const char* begin = &res[0];
    const char* expected_end = begin + res.size() - lit_suffix_len(suffix);

    switch(suffix) {
    case LiteralSuffix::NONE:
        if (base == 10 && !has_alnum) {
            // Plain integers stay as strings for now until they are interpreted
            // by the interpreter.
            return Token(TokenID::PLAIN_INT, std::move(res));
        }
        return read_number_token<std::int32_t>(
            begin, expected_end, base
        );
    case LiteralSuffix::S:
    case LiteralSuffix::S32:
        return read_number_token<std::int32_t>(
            begin, expected_end, base
        );
    case LiteralSuffix::S8:
        return read_number_token<std::int8_t>(
            begin, expected_end, base
        );
    case LiteralSuffix::S16:
        return read_number_token<std::int16_t>(
            begin, expected_end, base
        );
    case LiteralSuffix::S64:
        return read_number_token<std::int64_t>(
            begin, expected_end, base
        );
    case LiteralSuffix::U:
    case LiteralSuffix::U32:
        return read_number_token<std::uint32_t>(
            begin, expected_end, base
        );
    case LiteralSuffix::U8:
        return read_number_token<std::uint8_t>(
            begin, expected_end, base
        );
    case LiteralSuffix::U16:
        return read_number_token<std::uint16_t>(
            begin, expected_end, base
        );
    case LiteralSuffix::U64:
        return read_number_token<std::uint64_t>(
            begin, expected_end, base
        );
    case LiteralSuffix::F:
    case LiteralSuffix::F64:
        if (base == 16) {
            // Suffix is valid hex, let it be read as such.
            expected_end += lit_suffix_len(suffix);
            return read_number_token<std::int32_t>(
                begin, expected_end, base
            );
        }
        else if (base == 8)
            // Octal floating point not allowed.
            return Token(
                TokenID::ERROR, ErrPtr(new InvalidNumericLiteralErr())
            );
        // May not be the entire float, since the lexer splits on . always.
        return Token(TokenID::FLOAT_TAIL, std::move(res));
    case LiteralSuffix::F32:
        if (base == 16) {
            expected_end += 3;
            return read_number_token<std::int32_t>(
                begin, expected_end, base
            );
        }
        else if (base == 8)
            return Token(
                TokenID::ERROR, ErrPtr(new InvalidNumericLiteralErr())
            );
        return Token(TokenID::FLOAT_TAIL, std::move(res));
    }
}

// Read a string.
Token next_str(Cursor& cursor) {
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
                    return Token(TokenID::ERROR, std::move(code_point.err));
                return Token(TokenID::ERROR, ErrPtr(new UnclosedStrErr()));
            }
            encode_unicode(code_point.res, s);
            break;
        }
        case '"':
            return Token(TokenID::STRING, s);
        case '\n':
        case EOF:
            cursor.ungetc();
            return Token(TokenID::ERROR, ErrPtr(new UnclosedStrErr()));
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
            return ErrPtr(new TrailingSpaceErr(Span(start, space_count)));
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
        return Token(TokenID::COLON);
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
            return ErrPtr(new UnexpectedCharErr(c, Span(start)));
        return Token(TokenID::BANG_EQUALS);
    case '@':
        return Token(TokenID::AT);
    case '<':
        switch(cursor.getc()) {
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
        c = cursor.getc();
        switch(c) {
        case '=':
            return Token(TokenID::PERCENT_EQUALS);
        case '*':
            return Token(
                check_next(cursor, '*') ?
                    TokenID::PERCENT_DOUBLE_STAR: TokenID::PERCENT_STAR
            );
        default:
            cursor.ungetc();
            return Token(TokenID::PERCENT);
        }
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
        c = cursor.getc();
        switch(c) {
        case '|':
            return Token(TokenID::DOUBLE_PIPE);
        case '=':
            return Token(TokenID::PIPE_EQUALS);
        default:
            cursor.ungetc();
            return Token(TokenID::PIPE);
        }
    case '^':
        return Token(
            check_next(cursor, '=') ? TokenID::CAROT_EQUALS: TokenID::CAROT
        );
    case '`':
        return Token(TokenID::BACKTICK);
    case '(':
        return Token(TokenID::LEFT_CURVED);
    case ')':
        return Token(TokenID::RIGHT_CURVED);
    case '[':
        return Token(TokenID::LEFT_SQUARE);
    case ']':
        return Token(TokenID::RIGHT_SQUARE);
    case '{':
        return Token(TokenID::LEFT_CURLY);
    case '}':
        return Token(TokenID::RIGHT_CURLY);
    case '"':
        return next_str(cursor);
    case '\'':
        return next_char(cursor);
    default:
        if (is_digit(c))
            return next_number(c, cursor);
        if (is_alnum(c) || c == '_')
            return next_id(c, cursor);
        return ErrPtr(new UnexpectedCharErr(c, Span(start)));
    }
}

}

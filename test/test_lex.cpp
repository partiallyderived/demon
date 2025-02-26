#include <catch2/catch_test_macros.hpp>

#include <cstdint>

#include "dl/lex/cursor.hpp"
#include "dl/lex/lex.hpp"
#include "dl/lex/token.hpp"
#include "dl/lex/tokenid.hpp"
#include "dl/pos.hpp"
#include "dl/res.hpp"

#include "streamfile.hpp"

using namespace dl;

void reset(Cursor& cursor) {
    dynamic_cast<StreamFile*>(cursor.file)->ignore();
    cursor.pos = Pos();
    cursor.put_back = 0;
}

Res<Token> result(Cursor& cursor, const char* str="", bool do_reset=true) {
    if (do_reset)
        reset(cursor);

    auto file = dynamic_cast<StreamFile*>(cursor.file);
    file->clear(); // Get rid of any EOF flags.
    file->put(str);
    return next_token(cursor);
}

TEST_CASE("lex", "[lex]") {
    auto file = StreamFile();
    auto c = Cursor(&file);

    SECTION("Init Cursor") {
        REQUIRE(Pos() == Pos(1, 1));
        REQUIRE(c.pos == Pos());
    }

    SECTION("Cursor") {
        REQUIRE(c.pos == Pos());

        file.put("asdf\nfdsa\n\nsadf");
        REQUIRE(c.getc() == 'a');
        REQUIRE(c.pos == Pos(1, 2));
        c.ungetc();
        REQUIRE(c.pos == Pos(1, 1));
        REQUIRE(c.getc() == 'a');

        REQUIRE(c.getc() == 's');
        REQUIRE(c.pos.col == 3);
        REQUIRE(c.getc() == 'd');
        REQUIRE(c.pos.col == 4);
        REQUIRE(c.getc() == 'f');
        REQUIRE(c.pos.col == 5);

        REQUIRE(c.getc() == '\n');
        REQUIRE(c.pos == Pos(2, 1));
        c.ungetc();
        REQUIRE(c.pos == Pos(1, 5));

        REQUIRE(c.getc() == '\n');
        REQUIRE(c.getc() == 'f');
        REQUIRE(c.getc() == 'd');

        c.ungetc();
        c.ungetc();
        REQUIRE(c.pos == Pos(2, 1));
        REQUIRE(c.getc() == 'f');
        REQUIRE(c.getc() == 'd');
        REQUIRE(c.getc() == 's');
        REQUIRE(c.getc() == 'a');
        REQUIRE(c.pos == Pos(2, 5));

        REQUIRE(c.getc() == '\n');
        REQUIRE(c.getc() == '\n');
        REQUIRE(c.pos == Pos(4, 1));
        
        c.ungetc();
        REQUIRE(c.pos == Pos(3, 1));
        c.ungetc();
        REQUIRE(c.pos == Pos(2, 5));

        REQUIRE(c.getc() == '\n');
        REQUIRE(c.getc() == '\n');
        REQUIRE(c.getc() == 's');
        REQUIRE(c.getc() == 'a');
        REQUIRE(c.getc() == 'd');
        REQUIRE(c.getc() == 'f');
        REQUIRE(c.pos == Pos(4, 5));
    }

    SECTION("Space") {
        SECTION("Trailing Space Error") {
            REQUIRE(*result(c, "    \n") == TrailingSpaceErr(Pos()));

            REQUIRE(*result(c, "    ") == TrailingSpaceErr(Pos()));
        }

        SECTION("Space") {
            REQUIRE(
                result(c, "    a") == Token(TokenID::SPACE, std::uint32_t(4))
            );
            result(c, "a  b");
            REQUIRE(
                result(c, "", false) == Token(TokenID::SPACE, std::uint32_t(2))
            );
        }
    }

    SECTION("Newline") {
        REQUIRE(result(c, "\n") == Token(TokenID::NEWLINE));
    }

    SECTION("EOF") {
        REQUIRE(result(c, "") == Token(TokenID::END_OF_FILE));
    }

    SECTION("Comment") {
        REQUIRE(result(c, "# Ignored") == Token(TokenID::HASH));

        // Check that everything after the hash was consumed.
        REQUIRE(result(c) == Token(TokenID::END_OF_FILE));
    }

    SECTION("Colon") {
        REQUIRE(result(c, ":") == Token(TokenID::COLON));
    }

    SECTION("Double Colon") {
        REQUIRE(result(c, "::") == Token(TokenID::DOUBLE_COLON));
    }

    SECTION("Comma") {
        REQUIRE(result(c, ",") == Token(TokenID::COMMA));
    }

    SECTION("Dot") {
        REQUIRE(result(c, ".") == Token(TokenID::DOT));
    }

    SECTION("Bang Equals") {
        REQUIRE(result(c, "!=") == Token(TokenID::BANG_EQUALS));
    }

    SECTION("Left Angle") {
        REQUIRE(result(c, "<") == Token(TokenID::LEFT_ANGLE));
    }

    SECTION("Left Angle Equals") {
        REQUIRE(result(c, "<=") == Token(TokenID::LEFT_ANGLE_EQUALS));
    }

    SECTION("Double Left Angle") {
        REQUIRE(result(c, "<<") == Token(TokenID::DOUBLE_LEFT_ANGLE));
    }

    SECTION("Double Left Angle Equals") {
        REQUIRE(result(c, "<<=") == Token(TokenID::DOUBLE_LEFT_ANGLE_EQUALS));
    }

    SECTION("Right Angle") {
        REQUIRE(result(c, ">") == Token(TokenID::RIGHT_ANGLE));
    }

    SECTION("Right Angle Equals") {
        REQUIRE(result(c, ">=") == Token(TokenID::RIGHT_ANGLE_EQUALS));
    }

    SECTION("Double Right Angle") {
        REQUIRE(result(c, ">>") == Token(TokenID::DOUBLE_RIGHT_ANGLE));
    }

    SECTION("Double Right Angle Equals") {
        REQUIRE(result(c, ">>=") == Token(TokenID::DOUBLE_RIGHT_ANGLE_EQUALS));
    }

    SECTION("Plus") {
        REQUIRE(result(c, "+") == Token(TokenID::PLUS));
    }

    SECTION("Plus Equals") {
        REQUIRE(result(c, "+=") == Token(TokenID::PLUS_EQUALS));
    }

    SECTION("Minus") {
        REQUIRE(result(c, "-") == Token(TokenID::MINUS));
    }

    SECTION("Minus Equals") {
        REQUIRE(result(c, "-=") == Token(TokenID::MINUS_EQUALS));
    }

    SECTION("Minus Right Angle") {
        REQUIRE(result(c, "->") == Token(TokenID::MINUS_RIGHT_ANGLE));
    }

    SECTION("Star") {
        REQUIRE(result(c, "*") == Token(TokenID::STAR));
    }

    SECTION("Star Equals") {
        REQUIRE(result(c, "*=") == Token(TokenID::STAR_EQUALS));
    }

    SECTION("Double Star") {
        REQUIRE(result(c, "**") == Token(TokenID::DOUBLE_STAR));
    }

    SECTION("Double Star Equals") {
        REQUIRE(result(c, "**=") == Token(TokenID::DOUBLE_STAR_EQUALS));
    }

    SECTION("Slash") {
        REQUIRE(result(c, "/") == Token(TokenID::SLASH));
    }

    SECTION("Slash Equals") {
        REQUIRE(result(c, "/=") == Token(TokenID::SLASH_EQUALS));
    }

    SECTION("Percent") {
        REQUIRE(result(c, "%") == Token(TokenID::PERCENT));
    }

    SECTION("Percent") {
        REQUIRE(result(c, "%=") == Token(TokenID::PERCENT_EQUALS));
    }

    SECTION("Tilde") {
        REQUIRE(result(c, "~") == Token(TokenID::TILDE));
    }

    SECTION("Ampersand") {
        REQUIRE(result(c, "&") == Token(TokenID::AMPERSAND));
    }

    SECTION("Ampersand Equals") {
        REQUIRE(result(c, "&=") == Token(TokenID::AMPERSAND_EQUALS));
    }

    SECTION("Pipe") {
        REQUIRE(result(c, "|") == Token(TokenID::PIPE));
    }

    SECTION("Pipe Equals") {
        REQUIRE(result(c, "|=") == Token(TokenID::PIPE_EQUALS));
    }

    SECTION("Carot") {
        REQUIRE(result(c, "^") == Token(TokenID::CAROT));
    }

    SECTION("Left Curved") {
        REQUIRE(result(c, "(") == Token(TokenID::LEFT_CURVED));
    }

    SECTION("Right Curved") {
        REQUIRE(result(c, ")") == Token(TokenID::RIGHT_CURVED));
    }

    SECTION("Left Square") {
        REQUIRE(result(c, "[") == Token(TokenID::LEFT_SQUARE));
    }

    SECTION("Right Square") {
        REQUIRE(result(c, "]") == Token(TokenID::RIGHT_SQUARE));
    }

    SECTION("Left Curly") {
        REQUIRE(result(c, "{") == Token(TokenID::LEFT_CURLY));
    }

    SECTION("Right Curly") {
        REQUIRE(result(c, "}") == Token(TokenID::RIGHT_CURLY));
    }

    SECTION("Char") {
        SECTION("Simple") {
            REQUIRE(
                result(c, "'a'") == Token(TokenID::CHAR, std::int32_t('a'))
            );
            REQUIRE(
                result(c, "'.'") == Token(TokenID::CHAR, std::int32_t('.'))
            );
        }

        SECTION("Escaped") {
            REQUIRE(
                result(c, "'\\n'") == Token(TokenID::CHAR, std::int32_t('\n'))
            );
            REQUIRE(
                result(c, "'\\''") == Token(TokenID::CHAR, std::int32_t('\''))
            );
            // I omit tests for other escaped characters since they are tested
            // in the string tests below, and the logic comes from the same
            // code.
        }

        SECTION("Invalid Escape Error") {
            REQUIRE(*result(c, "'\\c'") == InvalidEscapeErr(Pos(1, 2)));
        }

        SECTION("Unclosed Char Error") {
            REQUIRE(*result(c, "'a") == UnclosedCharErr(Pos()));

            REQUIRE(*result(c, "'a\n'") == UnclosedCharErr(Pos()));

            // Should take precedence over any other error.
            REQUIRE(*result(c, "'\\c") == UnclosedCharErr(Pos()));
        }

        SECTION("Empty") {
            REQUIRE(*result(c, "''") == InvalidCharErr(Pos()));
        }

        SECTION("More Than One Character") {
            REQUIRE(*result(c, "'ab'") == InvalidCharErr(Pos()));

            REQUIRE(*result(c, "'\\ab'") == InvalidCharErr(Pos()));

            // UTF-8 2 byte character, then a.
            REQUIRE(*result(c, "'\xDF\xBF\a'") == InvalidCharErr(Pos()));
        }

        SECTION("Byte Escape") {
            REQUIRE(
                result(c, "'\\xa9'") ==
                Token(TokenID::CHAR, std::int32_t(0xa9))
            );
        }

        SECTION("Bad Byte Escape") {
            REQUIRE(*result(c, "'\\xk9'") == InvalidHexDigitErr(Pos(1, 4)));
        }

        SECTION("Unicode Escape") {
            REQUIRE(
                result(c, "'\\u0055'") ==
                Token(TokenID::CHAR, std::int32_t(0x0055))
            );

            REQUIRE(
                result(c, "'\\u03B1'") ==
                Token(TokenID::CHAR, std::int32_t(0x03b1))
            );

            REQUIRE(
                result(c, "'\\ufeDC'") ==
                Token(TokenID::CHAR, std::int32_t(0xfedc))
            );
        }

        SECTION("Bad Unicode Escape") {
            REQUIRE(
                *result(c, "'\\u0x1y'") == InvalidHexDigitErr(Pos(1, 5))
            );

            // Invalid UTF-8 (Surrogate)
            REQUIRE(
                *result(c, "'\\uDEAD'") == InvalidUnicodeCodePointErr(Pos(1, 4))
            );
        }

        SECTION("Long Unicode Escape") {
            REQUIRE(
                result(c, "'\\U0001d400'") ==
                Token(TokenID::CHAR, std::int32_t(0x0001d400))
            );
        }

        SECTION("Bad Long Unicode Escape") {
            REQUIRE(
                *result(c, "'\\U0001x2yz'") == InvalidHexDigitErr(Pos(1, 8))
            );

            // Invalid UTF-8 (Surrogate)
            REQUIRE(
                *result(c, "'\\U0000DEAD'") ==
                InvalidUnicodeCodePointErr(Pos(1, 4))
            );

            // Invalid UTF-8 (Code point exceeds 0x0010FFFF)
            REQUIRE(
                *result(c, "'\\U00110000'") ==
                InvalidUnicodeCodePointErr(Pos(1, 4))
            );
        }

        SECTION("Decoding UTF-8") {
            REQUIRE(
                result(c, "'\xDF\xBF'") ==
                Token(TokenID::CHAR, std::int32_t(0x07FF))
            );

            REQUIRE(
                result(c, "'\xEF\xBF\xBF'") ==
                Token(TokenID::CHAR, std::int32_t(0xFFFF))
            );

            REQUIRE(
                result(c, "'\xF0\xBF\xBF\xBF'") ==
                Token(TokenID::CHAR, std::int32_t(0x3FFFF))
            );
        }

        SECTION("Overlong Encoding") {
            // Encodes code point 0x01 in two bytes.
            REQUIRE(
                result(c, "'\xC0\x81'") ==
                Token(TokenID::CHAR, std::int32_t(0xFFFD))
            );

            // Encodes code point 0x80 in three bytes.
            REQUIRE(
                result(c, "'\xE0\x82\x80'") ==
                Token(TokenID::CHAR, std::int32_t(0xFFFD))
            );

            // Encodes code point 0x0800 in four bytes.
            REQUIRE(
                result(c, "'\xF0\x80\xA0\x80'") ==
                Token(TokenID::CHAR, std::int32_t(0xFFFD))
            );    
        }

        SECTION("Decoded Surrogate") {
            // Decodes to 0xDEAD.
            REQUIRE(
                result(c, "'\xED\xBA\xAD'") ==
                Token(TokenID::CHAR, std::int32_t(0xFFFD))
            );
        }

        SECTION("Decoded Code Point Too Large") {
            // Code Point: 0x1FFFFF
            REQUIRE(
                result(c, "'\xF7\xBF\xBF\xBF'") ==
                Token(TokenID::CHAR, std::int32_t(0xFFFD))
            );
        }

        SECTION("Decoding Invalid UTF-8") {
            // 1 Byte with all bits on.
            REQUIRE(
                result(c, "'\xFF'") ==
                Token(TokenID::CHAR, std::int32_t(0xFFFD))
            );

            // 2 Bytes, second byte has all bits on.
            // Results in two errors so invalid character due to length because
            // it consists of two replacement characters.
            REQUIRE(
                *result(c, "'\xDF\xFF'") == InvalidCharErr(Pos())
            );

            // 3 Bytes, third byte has all bits on.
            REQUIRE(
                *result(c, "'\xEF\x80\xFF'") == InvalidCharErr(Pos())
            );

            // 4 Bytes, fourth byte has all bits on.
            REQUIRE(
                *result(c, "'\xF7\x80\x80\xFF'") == InvalidCharErr(Pos())
            );
        }
    }

    SECTION("String") {
        SECTION("Simple") {
            REQUIRE(result(c, "\"\"") == Token(TokenID::STRING, ""));
            REQUIRE(result(c, "\"asdf\"") == Token(TokenID::STRING, "asdf"));
        }

        SECTION("Escaped") {
            REQUIRE(
                result(c, "\"asdf\\a\\b\\e\\f\\n\\r\\t\\v\\\"\\'\\\\fdsa\"") ==
                Token(TokenID::STRING, "asdf\a\b\e\f\n\r\t\v\"'\\fdsa")
            );
        }

        SECTION("Invalid Escape Error") {
            REQUIRE(*result(c, "\"asdf\\c\"") == InvalidEscapeErr(Pos(1, 6)));
        }

        SECTION("Unclosed String Error") {
            REQUIRE(*result(c, "\"asdf") == UnclosedStrErr(Pos()));

            REQUIRE(*result(c, "\"asdf\n\"") == UnclosedStrErr(Pos()));

            // Should take precedence over any other error.
            REQUIRE(*result(c, "\"asdf\\c") == UnclosedStrErr(Pos()));
        }

        SECTION("Byte Escape") {
            REQUIRE(
                result(c, "\"asdf\\x2asa\"") ==
                Token(TokenID::STRING, "asdf\x2asa")
            );
        }

        SECTION("Bad Byte Escape") {
            REQUIRE(
                *result(c, "\"asdf\\xyz\"") == InvalidHexDigitErr(Pos(1, 8))
            );
        }

        SECTION("Unicode Escape") {
            REQUIRE(
                result(c, "\"asdf\\u0055\"") ==
                Token(TokenID::STRING, "asdf\x55")
            );

            REQUIRE(
                result(c, "\"asdf\\u03b1\"") ==
                Token(TokenID::STRING, "asdf\xCE\xB1")
            );

            REQUIRE(
                result(c, "\"asdf\\ufedc\"") ==
                Token(TokenID::STRING, "asdf\xEF\xBB\x9C")
            );
        }

        SECTION("Bad Unicode Escape") {
            REQUIRE(
                *result(c, "\"asdf\\u0x1y\"") == InvalidHexDigitErr(Pos(1, 9))
            );

            // Invalid UTF-8 (surrogate)
            REQUIRE(
                *result(c, "\"asdf\\uDEAD\"") ==
                InvalidUnicodeCodePointErr(Pos(1, 8))
            );
        }

        SECTION("Long Unicode Escape") {
            REQUIRE(
                result(c, "\"asdf\\U0001d400\"") ==
                Token(TokenID::STRING, "asdf\xF0\x9D\x90\x80")
            );
        }

        SECTION("Bad Long Unicode Escape") {
            // Contains non-hex characters.
            REQUIRE(
                *result(c, "\"asdf\\U000axbyz\"") ==
                InvalidHexDigitErr(Pos(1, 12))
            );

            // Invalid UTF-8 (surrogate)
            REQUIRE(
                *result(c, "\"asdf\\U0000DEAD\"") ==
                InvalidUnicodeCodePointErr(Pos(1, 8))
            );

            // Code point is too big
            REQUIRE(
                *result(c, "\"asdf\\U00110000\"") ==
                InvalidUnicodeCodePointErr(Pos(1, 8))
            );
        }

        SECTION("Direct Unicode") {
            REQUIRE(
                result(c, "\"\xDF\xBF\xEF\xBF\xBF\xF0\xBF\xBF\xBF\"") ==
                Token(TokenID::STRING, "\xDF\xBF\xEF\xBF\xBF\xF0\xBF\xBF\xBF")
            );
        }

        SECTION("Overlong Encoding") {
            // Encodes code point 0x01 in two bytes.
            REQUIRE(
                result(c, "\"\xC0\x81\"") ==
                // Should contain UTF-8 encoding for replacement character
                // U+FFFD
                Token(TokenID::STRING, "\xEF\xBF\xBD")
            );

            // Encodes code point 0x80 in three bytes.
            REQUIRE(
                result(c, "\"\xE0\x82\x80\"") ==
                Token(TokenID::STRING, "\xEF\xBF\xBD")
            );

            // Encodes code point 0x0800 in four bytes.
            REQUIRE(
                result(c, "\"\xF0\x80\xA0\x80\"") ==
                Token(TokenID::STRING, "\xEF\xBF\xBD")
            );    
        }

        SECTION("Decoded Surrogate") {
            // Decodes to 0xDEAD.
            REQUIRE(
                result(c, "\"\xED\xBA\xAD\"") ==
                Token(TokenID::STRING, "\xEF\xBF\xBD")
            );
        }

        SECTION("Decoded Code Point Too Large") {
            // Code Point: 0x1FFFFF
            REQUIRE(
                result(c, "\"\xF7\xBF\xBF\xBF\"") ==
                Token(TokenID::STRING, "\xEF\xBF\xBD")
            );
        }

        SECTION("Decoding Invalid UTF-8") {
            // Per the UTF-8 Standard, each error is either only 1 byte long or
            // ends just before the first disallowed byte.
            // # errors = # replacement characters

            // 1 Byte, 1 error
            REQUIRE(
                result(c, "\"\xFF\"") == Token(TokenID::STRING, "\xEF\xBF\xBD")
            );

            // 2 Bytes, 1 error
            REQUIRE(
                result(c, "\"\xDFs\"") ==
                Token(TokenID::STRING, "\xEF\xBF\xBDs")
            );

            // 2 Bytes, 2 errors
            REQUIRE(
                result(c, "\"\xFF\x80\"") ==
                Token(TokenID::STRING, "\xEF\xBF\xBD\xEF\xBF\xBD")
            );

            // 3 Bytes, 1 error
            REQUIRE(
                result(c, "\"\xEF\x80s\"") ==
                Token(TokenID::STRING, "\xEF\xBF\xBDs")
            );

            // 3 Bytes, 2 errors
            REQUIRE(
                result(c, "\"\xEF\xFFs\"") ==
                Token(TokenID::STRING, "\xEF\xBF\xBD\xEF\xBF\xBDs")
            );

            // 3 Bytes, 3 errors
            REQUIRE(
                result(c, "\"\xFF\x80\x80\"") ==
                Token(TokenID::STRING, "\xEF\xBF\xBD\xEF\xBF\xBD\xEF\xBF\xBD")
            );

            // 4 Bytes, 1 error
            REQUIRE(
                result(c, "\"\xF0\xBF\xBFs\"") ==
                Token(TokenID::STRING, "\xEF\xBF\xBDs")
            );

            // 4 Bytes, 2 errors
            REQUIRE(
                result(c, "\"\xF0\xBF\xBF\xFF\"") ==
                Token(TokenID::STRING, "\xEF\xBF\xBD\xEF\xBF\xBD")
            );

            // 4 Bytes, 3 errors
            REQUIRE(
                result(c, "\"\xF0\xBF\xFF\xFF\"") ==
                Token(TokenID::STRING, "\xEF\xBF\xBD\xEF\xBF\xBD\xEF\xBF\xBD")
            );

            // 4 Bytes, 4 errors
            REQUIRE(
                result(c, "\"\xFF\x80\x80\x80\"") ==
                Token(
                    TokenID::STRING,
                    "\xEF\xBF\xBD\xEF\xBF\xBD\xEF\xBF\xBD\xEF\xBF\xBD"
                )
            );
        }
    }

    SECTION("Number") {
        SECTION("Integers") {
            SECTION("Decimal") {
                REQUIRE(
                    result(c, "0") == Token(TokenID::PLAIN_INT, std::int32_t(0))
                );
                REQUIRE(
                    result(c, "1234") ==
                    Token(TokenID::PLAIN_INT, std::int32_t(1234))
                );
            }

            SECTION("Hexadecimal") {
                REQUIRE(
                    result(c, "0x0") == Token(TokenID::NUMBER, std::int32_t(0))
                );

                REQUIRE(
                    result(c, "0X89aB") ==
                    Token(TokenID::NUMBER, std::int32_t(0x89ab))
                );
            }

            SECTION("Octal") {
                REQUIRE(
                    result(c, "0o0") == Token(TokenID::NUMBER, std::int32_t(0))
                );

                REQUIRE(
                    result(c, "0O755") ==
                    Token(TokenID::NUMBER, std::int32_t(0755))
                );
            }

            SECTION("Suffixed") {
                REQUIRE(
                    result(c, "0s") == Token(TokenID::NUMBER, std::int32_t(0))
                );

                REQUIRE(
                    result(c, "0s8") == Token(TokenID::NUMBER, std::int8_t(0))
                );

                REQUIRE(
                    result(c, "0s16") == Token(TokenID::NUMBER, std::int16_t(0))
                );

                REQUIRE(
                    result(c, "0s32") == Token(TokenID::NUMBER, std::int32_t(0))
                );

                REQUIRE(
                    result(c, "0s64") == Token(TokenID::NUMBER, std::int64_t(0))
                );

                REQUIRE(
                    result(c, "0u") == Token(TokenID::NUMBER, std::uint32_t(0))
                );

                REQUIRE(
                    result(c, "0u8") == Token(TokenID::NUMBER, std::uint8_t(0))
                );

                REQUIRE(
                    result(c, "0u16") ==
                    Token(TokenID::NUMBER, std::uint16_t(0))
                );

                REQUIRE(
                    result(c, "0u32") ==
                    Token(TokenID::NUMBER, std::uint32_t(0))
                );

                REQUIRE(
                    result(c, "0u64") ==
                    Token(TokenID::NUMBER, std::uint64_t(0))
                );

                REQUIRE(
                    result(c, "1234S") ==
                    Token(TokenID::NUMBER, std::int32_t(1234))
                );

                REQUIRE(
                    result(c, "100S8") ==
                    Token(TokenID::NUMBER, std::int8_t(100))
                );

                REQUIRE(
                    result(c, "12345S16") ==
                    Token(TokenID::NUMBER, std::int16_t(12345))
                );

                REQUIRE(
                    result(c, "123456789S32") ==
                    Token(TokenID::NUMBER, std::int32_t(123456789L))
                );

                REQUIRE(
                    result(c, "12345678910S64") ==
                    Token(TokenID::NUMBER, std::int64_t(12345678910LL))
                );

                REQUIRE(
                    result(c, "1234U") ==
                    Token(TokenID::NUMBER, std::uint32_t(1234))
                );

                REQUIRE(
                    result(c, "200U8") ==
                    Token(TokenID::NUMBER, std::uint8_t(200))
                );

                REQUIRE(
                    result(c, "12345U16") ==
                    Token(TokenID::NUMBER, std::uint16_t(12345))
                );

                REQUIRE(
                    result(c, "123456789U32") ==
                    Token(TokenID::NUMBER, std::uint32_t(123456789L))
                );

                REQUIRE(
                    result(c, "12345678910U64") ==
                    Token(TokenID::NUMBER, std::uint64_t(12345678910LL))
                );
            }

            SECTION("Mixed") {
                REQUIRE(
                    result(c, "0x0s64") ==
                    Token(TokenID::NUMBER, std::int64_t(0))
                );

                REQUIRE(
                    result(c, "0x89aBu32") ==
                    Token(TokenID::NUMBER, std::uint32_t(0x89ab))
                );

                REQUIRE(
                    result(c, "0o0u8") ==
                    Token(TokenID::NUMBER, std::uint8_t(0))
                );

                REQUIRE(
                    result(c, "0o5676s16") ==
                    Token(TokenID::NUMBER, std::int16_t(05676))
                );
            }

            SECTION("Hex Ending With Float Suffix") {
                // These tests check that hexadecimals ending with a valid float
                // suffix (f, f32, or f64) are all correctly interpreted as a
                // hex int rather than a float.

                REQUIRE(
                    result(c, "0xf") ==
                    Token(TokenID::NUMBER, std::int32_t(0xf))
                );

                REQUIRE(
                    result(c, "0xf32") ==
                    Token(TokenID::NUMBER, std::int32_t(0xf32))
                );

                REQUIRE(
                    result(c, "0xf64") ==
                    Token(TokenID::NUMBER, std::int32_t(0xf64))
                );

                REQUIRE(
                    result(c, "0x89aBf") ==
                    Token(TokenID::NUMBER, std::int32_t(0x89abf))
                );

                REQUIRE(
                    result(c, "0x89aBf32") ==
                    Token(TokenID::NUMBER, std::int32_t(0x89abf32))
                );

                REQUIRE(
                    result(c, "0x89aBf64") ==
                    Token(TokenID::NUMBER, std::int32_t(0x89abf64))
                );
            }

            SECTION("Leading Zeroes Error") {
                REQUIRE(*result(c, "00") == LeadingZeroesErr(Pos()));

                REQUIRE(*result(c, "01") == LeadingZeroesErr(Pos()));

                REQUIRE(*result(c, "001234") == LeadingZeroesErr(Pos()));
            }

            SECTION("Invalid Numeric Literal Error") {
                REQUIRE(*result(c, "0h") == InvalidNumericLiteralErr(Pos()));

                REQUIRE(*result(c, "123g") == InvalidNumericLiteralErr(Pos()));
            }

            SECTION("Out Of Range Error") {
                REQUIRE(*result(c, "3000000000") == OutOfRangeErr(Pos()));

                REQUIRE(*result(c, "200s8") == OutOfRangeErr(Pos()));

                REQUIRE(*result(c, "33000s16") == OutOfRangeErr(Pos()));

                REQUIRE(
                    *result(c, "9300000000000000000s64") == OutOfRangeErr(Pos())
                );

                REQUIRE(*result(c, "5000000000u") == OutOfRangeErr(Pos()));

                REQUIRE(*result(c, "300u8") == OutOfRangeErr(Pos()));

                REQUIRE(*result(c, "70000u16") == OutOfRangeErr(Pos()));

                REQUIRE(
                    *result(c, "19000000000000000000u64") ==
                    OutOfRangeErr(Pos())
                );
            }
        }

        SECTION("Floating-point") {
            SECTION("Simple") {
                REQUIRE(result(c, "0.0") == Token(TokenID::NUMBER, 0.0));

                REQUIRE(
                    result(c, "1234.5678") == Token(TokenID::NUMBER, 1234.5678)
                );

                REQUIRE(result(c, "1e7") == Token(TokenID::NUMBER, 1e7));

                REQUIRE(result(c, "1e-7") == Token(TokenID::NUMBER, 1e-7));

                REQUIRE(
                    result(c, "1234.56e-78") ==
                    Token(TokenID::NUMBER, 1234.56e-78)
                );
            }

            SECTION("Suffixed") {
                REQUIRE(result(c, "0f") == Token(TokenID::NUMBER, 0.0));

                REQUIRE(result(c, "0f32") == Token(TokenID::NUMBER, 0.0f));

                REQUIRE(result(c, "0f64") == Token(TokenID::NUMBER, 0.0));

                REQUIRE(
                    result(c, "12.34e-30f") ==
                    Token(TokenID::NUMBER, 12.34e-30)
                );

                REQUIRE(
                    result(c, "12.34e-30f32") ==
                    Token(TokenID::NUMBER, 12.34e-30f)
                );

                REQUIRE(
                    result(c, "12.34e-30f64") ==
                    Token(TokenID::NUMBER, 12.34e-30)
                );
            }

            SECTION("Invalid Numeric Literal Error") {
                REQUIRE(*result(c, "0f31") == InvalidNumericLiteralErr(Pos()));

                REQUIRE(*result(c, "78.9a") == InvalidNumericLiteralErr(Pos()));

                REQUIRE(*result(c, "1234e") == InvalidNumericLiteralErr(Pos()));
            }

            SECTION("Out Of Range Error") {
                REQUIRE(*result(c, "1e46f32") == OutOfRangeErr(Pos()));

                REQUIRE(*result(c, "1e309") == OutOfRangeErr(Pos()));
            }
        }

        SECTION("Identifier") {
            REQUIRE(result(c, "asdf_fdsa") == Token(TokenID::ID, "asdf_fdsa"));
        }

        SECTION("Keywords") {
            REQUIRE(result(c, "and") == Token(TokenID::AND));
            REQUIRE(result(c, "break") == Token(TokenID::BREAK));
            REQUIRE(result(c, "case") == Token(TokenID::CASE));
            REQUIRE(result(c, "continue") == Token(TokenID::CONTINUE));
            REQUIRE(result(c, "def") == Token(TokenID::DEF));
            REQUIRE(result(c, "elif") == Token(TokenID::ELIF));
            REQUIRE(result(c, "else") == Token(TokenID::ELSE));
            REQUIRE(result(c, "except") == Token(TokenID::EXCEPT));
            REQUIRE(result(c, "finally") == Token(TokenID::FINALLY));
            REQUIRE(result(c, "for") == Token(TokenID::FOR));
            REQUIRE(result(c, "if") == Token(TokenID::IF));
            REQUIRE(result(c, "in") == Token(TokenID::IN));
            REQUIRE(result(c, "match") == Token(TokenID::MATCH));
            REQUIRE(result(c, "not") == Token(TokenID::NOT));
            REQUIRE(result(c, "none") == Token(TokenID::NONE));
            REQUIRE(result(c, "null") == Token(TokenID::NULL_));
            REQUIRE(result(c, "or") == Token(TokenID::OR));
            REQUIRE(result(c, "raise") == Token(TokenID::RAISE));
            REQUIRE(result(c, "return") == Token(TokenID::RETURN));
            REQUIRE(result(c, "this") == Token(TokenID::THIS));
            REQUIRE(result(c, "true") == Token(TokenID::TRUE));
            REQUIRE(result(c, "try") == Token(TokenID::TRY));
            REQUIRE(result(c, "type") == Token(TokenID::TYPE));
            REQUIRE(result(c, "vars") == Token(TokenID::VARS));
            REQUIRE(result(c, "while") == Token(TokenID::WHILE));
        }

        SECTION("Several Lines") {
            REQUIRE(
                result(c, "a = 3\na += 5\nreturn a") == Token(TokenID::ID, "a")
            );
            REQUIRE(c.pos == Pos(1, 2));

            REQUIRE(
                result(c, "", false) == Token(TokenID::SPACE, std::uint32_t(1))
            );
            REQUIRE(c.pos == Pos(1, 3));

            REQUIRE(result(c, "", false) == Token(TokenID::EQUALS));
            REQUIRE(c.pos == Pos(1, 4));

            REQUIRE(
                result(c, "", false) == Token(TokenID::SPACE, std::uint32_t(1))
            );
            REQUIRE(c.pos == Pos(1, 5));

            REQUIRE(
                result(c, "", false) ==
                Token(TokenID::PLAIN_INT, std::int32_t(3))
            );
            REQUIRE(c.pos == Pos(1, 6));

            REQUIRE(result(c, "", false) == Token(TokenID::NEWLINE));
            REQUIRE(c.pos == Pos(2, 1));

            REQUIRE(result(c, "", false) == Token(TokenID::ID, "a"));
            REQUIRE(c.pos == Pos(2, 2));

            REQUIRE(
                result(c, "", false) == Token(TokenID::SPACE, std::uint32_t(1))
            );
            REQUIRE(c.pos == Pos(2, 3));

            REQUIRE(result(c, "", false) == Token(TokenID::PLUS_EQUALS));
            REQUIRE(c.pos == Pos(2, 5));

            REQUIRE(
                result(c, "", false) == Token(TokenID::SPACE, std::uint32_t(1))
            );
            REQUIRE(c.pos == Pos(2, 6));

            REQUIRE(
                result(c, "", false) ==
                Token(TokenID::PLAIN_INT, std::int32_t(5))
            );
            REQUIRE(c.pos == Pos(2, 7));

            REQUIRE(result(c, "", false) == Token(TokenID::NEWLINE));
            REQUIRE(c.pos == Pos(3, 1));

            REQUIRE(result(c, "", false) == Token(TokenID::RETURN));
            REQUIRE(c.pos == Pos(3, 7));

            REQUIRE(
                result(c, "", false) == Token(TokenID::SPACE, std::uint32_t(1))
            );
            REQUIRE(c.pos == Pos(3, 8));

            REQUIRE(result(c, "", false) == Token(TokenID::ID, "a"));
            REQUIRE(c.pos == Pos(3, 9));

            REQUIRE(result(c, "", false) == Token(TokenID::END_OF_FILE));
            REQUIRE(c.pos == Pos(3, 9));
        }

        SECTION("Unexpected Char Error") {
            REQUIRE(*result(c, "!") == UnexpectedCharErr(Pos(), '!'));
            REQUIRE(*result(c, "$") == UnexpectedCharErr(Pos(), '$'));
        }
    }
}

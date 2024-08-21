#!/usr/bin/env bash

build() ({
    cd build && cmake --build .
})

test_lex() ({
    build && bin/test-lex
})

test_tokens() ({
    build && bin/test-tokens
})

test() ({
    test_lex && test_tokens
})

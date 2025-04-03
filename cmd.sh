#!/usr/bin/env bash

build() ({
    cd build && cmake --build .
})

test_compose() ({
    build && bin/test-compose
})

test_interpret() ({
    build && bin/test-interpret
})

test_lex() ({
    build && bin/test-lex
})

test_parse() ({
    build && bin/test-parse
})


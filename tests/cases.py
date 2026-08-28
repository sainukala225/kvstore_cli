#!/usr/bin/env python3

import sys

from test_helpers import Expect, run_suite

tests = [
    (
        "positive decimal integer",
        "put pos_dec_int 123\n get pos_dec_int\n",
        Expect(stdout='"pos_dec_int" : 123\n'),
    ),
    (
        "negative decimal integer",
        "put neg_dec_int -45\n get neg_dec_int\n",
        Expect(stdout='"neg_dec_int" : -45\n'),
    ),
    (
        "positive octal integer",
        "put pos_oct_int 012\n get pos_oct_int\n",
        Expect(stdout='"pos_oct_int" : 10\n'),
    ),
    (
        "negative octal integer",
        "put neg_oct_int -017\n get neg_oct_int\n",
        Expect(stdout='"neg_oct_int" : -15\n'),
    ),
    (
        "positive hex integer",
        "put pos_hex_int 0x1E\n get pos_hex_int\n",
        Expect(stdout='"pos_hex_int" : 30\n'),
    ),
    (
        "negitive hex integer",
        "put neg_hex_int -0x10\n get neg_hex_int\n",
        Expect(stdout='"neg_hex_int" : -16\n'),
    ),
    (
        "decimal double 1",
        "put double 0.5\nget double\n",
        Expect(stdout='"double" : 0.5\n'),
    ),
    (
        "decimal double 2",
        "put double 1.5\nget double\n",
        Expect(stdout='"double" : 1.5\n'),
    ),
    (
        "exp double",
        "put double 1e5\nget double\n",
        Expect(stdout='"double" : 100000.0\n'),
    ),
    (
        "hex double 1",
        "put double 0x1E.2\nget double\n",
        Expect(stdout='"double" : 30.125\n'),
    ),
    (
        "hex double 2",
        "put double 0x1p4\nget double\n",
        Expect(stdout='"double" : 16.0\n'),
    ),
    (
        "binary integer",
        "put binary 0b11\nget binary\n",
        Expect(stdout='"binary" : 3\n'),
    ),
    (
        "string 1",
        "put string_val1 abc\n get string_val1\n",
        Expect(stdout='"string_val1" : "abc"\n'),
    ),
    (
        "string 2",
        "put string_val1 1e999\n get string_val1\n",
        Expect(stdout='"string_val1" : "1e999"\n'),
    ),
    (
        "infinity 1",
        "put infinity inf\n get infinity\n",
        Expect(stdout='"infinity" : inf\n'),
    ),
    (
        "infinity 2",
        "put infinity Infinity\n get infinity\n",
        Expect(stdout='"infinity" : inf\n'),
    ),
    (
        "not a number",
        "put nan NaN\n get nan\n",
        Expect(stdout='"nan" : nan\n'),
    ),
    (
        "max integer",
        "put int_max 9223372036854775807\n get int_max\n",
        Expect(stdout='"int_max" : 9223372036854775807\n'),
    ),
    (
        "above max integer",
        "put above_int_max 9223372036854775808\n get above_int_max\n",
        Expect(stdout='"above_int_max" : 9.22337e+18\n'),
    ),
    (
        "min integer",
        "put int_min -9223372036854775808\n get int_min\n",
        Expect(stdout='"int_min" : -9223372036854775808\n'),
    ),
    (
        "below min integer",
        "put below_int_min -9223372036854775809\n get below_int_min\n",
        Expect(stdout='"below_int_min" : -9.22337e+18\n'),
    ),
    (
        "high number",
        "put highnum 99999999999999999999999999\n get highnum\n",
        Expect(stdout='"highnum" : 1e+26\n'),
    ),
    (
        "integer not via double",
        "put int_not_via_double 9007199254740993\n get int_not_via_double\n",
        Expect(stdout='"int_not_via_double" : 9007199254740993\n'),
    ),
    (
        "override int key with string",
        "put key 123\n put key 'hello'\n get key\n",
        Expect(stdout='"key" : "hello"\n'),
    ),
]

exit_code = run_suite("cases.py", tests)
sys.exit(exit_code)

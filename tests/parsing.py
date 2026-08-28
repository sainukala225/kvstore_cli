#!/usr/bin/env python3

import sys

from test_helpers import Expect, run_suite

tests = [
    (
        "using PUT",
        "PUT key value\n get key\n",
        Expect(stdout='"key" : "value"\n'),
    ),
    (
        "using Put",
        "Put key value\n get key\n",
        Expect(stdout='"key" : "value"\n'),
    ),
    (
        "using put",
        "put key value\n get key\n",
        Expect(stdout='"key" : "value"\n'),
    ),
    (
        "key with quotes",
        "put 'a b' 1\n get 'a b'\n",
        Expect(stdout='"a b" : 1\n'),
    ),
    (
        "key contains colon",
        "put 'a : b' 1\n get 'a : b'\n",
        Expect(stdout='"a : b" : 1\n'),
    ),
    (
        "key with length 100",
        f"put {'a' * 100} '100 length key'\n get {'a' * 100}\n",
        Expect(stdout=f'"{"a" * 100}" : "100 length key"\n'),
    ),
    (
        "key with length 101",
        f"put {'a' * 101} '101 length key'\n get {'a' * 101}\n",
        Expect(
            stderr="Error : Key is too long (should be at most 100)\nError : Key is too long (should be at most 100)\n",
        ),
    ),
    (
        "line limit exceeded",
        f"get {'a' * 1000}\n",
        Expect(stderr="Error : The command size should be under 1000\n"),
    ),
    (
        "unterminated quote in key",
        "put 'key value\n",
        Expect(stderr="Error : Unterminated quote in the Key\n"),
    ),
    (
        "unterminated quote in value",
        "put 'key' 'value\n",
        Expect(stderr="Error : Unterminated quote in the Value\n"),
    ),
]

exit_code = run_suite("parsing.py", tests)
sys.exit(exit_code)

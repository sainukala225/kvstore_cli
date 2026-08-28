#!/usr/bin/env python3

import sys

from test_helpers import Expect, run_suite

tests = [
    (
        "put without key",
        "put\n",
        Expect(stderr="Error : 'put' requires key argument. Type 'help' for usage.\n"),
    ),
    (
        "put without value",
        "put key\n",
        Expect(stderr="Error : 'put' requires value argument. Type 'help' for usage.\n"),
    ),
    (
        "put with extra argument",
        "put key value extra\n",
        Expect(stderr="Error : 'put' takes only two argument. Type 'help' for usage.\n"),
    ),
    (
        "get without key",
        "get\n",
        Expect(stderr="Error : 'get' requires key argument. Type 'help' for usage.\n"),
    ),
    (
        "get with extra argument",
        "get key value\n",
        Expect(stderr="Error : 'get' takes only one argument. Type 'help' for usage.\n"),
    ),
    (
        "delete without key",
        "delete\n",
        Expect(stderr="Error : 'delete' requires key argument. Type 'help' for usage.\n"),
    ),
    (
        "delete with extra argument",
        "delete key value\n",
        Expect(stderr="Error : 'delete' takes only one argument. Type 'help' for usage.\n"),
    ),
    (
        "save without filepath",
        "save\n",
        Expect(stderr="Error : 'save' requires filepath argument. Type 'help' for usage.\n"),
    ),
    (
        "save with extra argument",
        "save file extra\n",
        Expect(stderr="Error : 'save' takes only one argument. Type 'help' for usage.\n"),
    ),
    (
        "load without filepath",
        "load\n",
        Expect(stderr="Error : 'load' requires filepath argument. Type 'help' for usage.\n"),
    ),
    (
        "load with extra argument",
        "load file extra\n",
        Expect(stderr="Error : 'load' takes only one argument. Type 'help' for usage.\n"),
    ),
    (
        "exit with argument",
        "exit extra\n",
        Expect(stderr="Error : 'exit' takes no arguments. Type 'help' for usage.\n"),
    ),
    (
        "free with argument",
        "free extra\n",
        Expect(stderr="Error : 'free' takes no arguments. Type 'help' for usage.\n"),
    ),
    (
        "help with argument",
        "help extra\n",
        Expect(stderr="Error : 'help' takes no arguments. Type 'help' for usage.\n"),
    ),
    (
        "stats with argument",
        "stats extra\n",
        Expect(stderr="Error : 'stats' takes no arguments. Type 'help' for usage.\n"),
    ),
    (
        "invalid command",
        "puut key value\n",
        Expect(stderr="Error : Invalid command. Type 'help' for a list of commands.\n"),
    ),
    (
        "get key not in store",
        "get key_not_in_store\n",
        Expect(stderr="Error : Item doesn't exist\n"),
    ),
    (
        "delete key not in store",
        "delete key_not_in_store\n",
        Expect(stderr="Error : key 'key_not_in_store' doesn't exist in store\n"),
    ),
    (
        "get deleted key",
        "put bx 2\ndelete bx\nget bx\n",
        Expect(stderr="Error : Item doesn't exist\n"),
    ),
    (
        "get key after free",
        "put a 1\nfree\nget a\n",
        Expect(stderr="Error : Item doesn't exist\n"),
    ),
]

exit_code = run_suite("errors.py", tests)
sys.exit(exit_code)

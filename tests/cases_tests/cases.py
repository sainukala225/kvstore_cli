#!/usr/bin/env python3

import os
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
os.chdir(HERE)
BIN = HERE.parent.parent / "build" / "bin" / "kvstore"


def run(session):
    return subprocess.run(
        [BIN, "-T"],
        check=False,
        input=session,
        capture_output=True,
        text=True,
        errors="replace",
        timeout=10,
    )


tests = [
    (
        "positive decimal integer",
        "put pos_dec_int 123\n get pos_dec_int\n",
        '"pos_dec_int" : 123\n',
    ),
    (
        "negative decimal integer",
        "put neg_dec_int -45\n get neg_dec_int\n",
        '"neg_dec_int" : -45\n',
    ),
    (
        "positive octal integer",
        "put pos_oct_int 012\n get pos_oct_int\n",
        '"pos_oct_int" : 10\n',
    ),
    (
        "negative octal integer",
        "put neg_oct_int -017\n get neg_oct_int\n",
        '"neg_oct_int" : -15\n',
    ),
    (
        "positive hex integer",
        "put pos_hex_int 0x1E\n get pos_hex_int\n",
        '"pos_hex_int" : 30\n',
    ),
    (
        "negitive hex integer",
        "put neg_hex_int -0x10\n get neg_hex_int\n",
        '"neg_hex_int" : -16\n',
    ),
    (
        "decimal double 1",
        "put double 0.5\nget double\n",
        '"double" : 0.5\n',
    ),
    (
        "decimal double 2",
        "put double 1.5\nget double\n",
        '"double" : 1.5\n',
    ),
    (
        "exp double",
        "put double 1e5\nget double\n",
        '"double" : 100000.0\n',
    ),
    (
        "hex double 1",
        "put double 0x1E.2\nget double\n",
        '"double" : 30.125\n',
    ),
    (
        "hex double 2",
        "put double 0x1p4\nget double\n",
        '"double" : 16.0\n',
    ),
    (
        "binary integer",
        "put binary 0b11\nget binary\n",
        '"binary" : 3\n',
    ),
    (
        "string 1",
        "put string_val1 abc\n get string_val1\n",
        '"string_val1" : "abc"\n',
    ),
    (
        "string 2",
        "put string_val1 1e999\n get string_val1\n",
        '"string_val1" : "1e999"\n',
    ),
    (
        "infinity 1",
        "put infinity inf\n get infinity\n",
        '"infinity" : inf\n',
    ),
    (
        "infinity 2",
        "put infinity Infinity\n get infinity\n",
        '"infinity" : inf\n',
    ),
    (
        "not a number",
        "put nan NaN\n get nan\n",
        '"nan" : nan\n',
    ),
    (
        "max integer",
        "put int_max 9223372036854775807\n get int_max\n",
        '"int_max" : 9223372036854775807\n',
    ),
    (
        "above max integer",
        "put above_int_max 9223372036854775808\n get above_int_max\n",
        '"above_int_max" : 9.22337e+18\n',
    ),
    (
        "min integer",
        "put int_min -9223372036854775808\n get int_min\n",
        '"int_min" : -9223372036854775808\n',
    ),
    (
        "below min integer",
        "put below_int_min -9223372036854775809\n get below_int_min\n",
        '"below_int_min" : -9.22337e+18\n',
    ),
    (
        "high number",
        "put highnum 99999999999999999999999999\n get highnum\n",
        '"highnum" : 1e+26\n',
    ),
    (
        "integer not via double",
        "put int_not_via_double 9007199254740993\n get int_not_via_double\n",
        '"int_not_via_double" : 9007199254740993\n',
    ),
    (
        "override int key with string",
        "put key 123\n put key 'hello'\n get key\n",
        '"key" : "hello"\n',
    ),
]


failed_tests = 0
passed_tests = 0
total_tests = len(tests)

failures = []

for test_name, test, expected in tests:
    passed = True
    process_out = run(test)
    if process_out.stdout != expected:
        passed = False
        failures.append([test_name, "stdout", expected, process_out.stdout])
    if process_out.stderr != "":
        passed = False
        failures.append([test_name, "stderr", "", process_out.stderr])
    if process_out.returncode != 0:
        passed = False
        failures.append([test_name, "returncode", 0, process_out.returncode])

    if passed:
        passed_tests += 1
    else:
        failed_tests += 1

for test_name, stream, expected, actual in failures:
    print(
        f"""Error in Test {test_name} {stream} :
        Expected : {expected!r}
        got : {actual!r}""",
        file=sys.stderr,
    )
print(f"cases.py : {passed_tests}/{total_tests} passed")
if failed_tests != 0:
    print(f"cases.py : {failed_tests}/{total_tests} failed")
    sys.exit(1)

sys.exit(0)

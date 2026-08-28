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
        "using PUT",
        "PUT key value\n get key\n",
        {"stdout": '"key" : "value"\n', "stderr": ""},
    ),
    (
        "using Put",
        "Put key value\n get key\n",
        {"stdout": '"key" : "value"\n', "stderr": ""},
    ),
    (
        "using put",
        "put key value\n get key\n",
        {"stdout": '"key" : "value"\n', "stderr": ""},
    ),
    (
        "key with quotes",
        "put 'a b' 1\n get 'a b'\n",
        {"stdout": '"a b" : 1\n', "stderr": ""},
    ),
    (
        "key contains colon",
        "put 'a : b' 1\n get 'a : b'\n",
        {"stdout": '"a : b" : 1\n', "stderr": ""},
    ),
    (
        "key with length 100",
        f"put {'a' * 100} '100 length key'\n get {'a' * 100}\n",
        {"stdout": f'"{"a" * 100}" : "100 length key"\n', "stderr": ""},
    ),
    (
        "key with length 101",
        f"put {'a' * 101} '101 length key'\n get {'a' * 101}\n",
        {
            "stdout": "",
            "stderr": "Error : Key is too long (should be at most 100)\nError : Key is too long (should be at most 100)\n",
        },
    ),
    (
        "line limit exceeded",
        f"get {'a' * 1000}\n",
        {"stdout": "", "stderr": "Error : The command size should be under 1000\n"},
    ),
    (
        "unterminated quote in key",
        "put 'key value\n",
        {"stdout": "", "stderr": "Error : Unterminated quote in the Key\n"},
    ),
    (
        "unterminated quote in value",
        "put 'key' 'value\n",
        {"stdout": "", "stderr": "Error : Unterminated quote in the Value\n"},
    ),
]

failed_tests = 0
passed_tests = 0
total_tests = len(tests)

failures = []

for test_name, test, streams in tests:
    passed = True
    process_out = run(test)
    if process_out.stdout != streams["stdout"]:
        passed = False
        failures.append([test_name, "stdout", streams["stdout"], process_out.stdout])
    if process_out.stderr != streams["stderr"]:
        passed = False
        failures.append([test_name, "stderr", streams["stderr"], process_out.stderr])
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

print(f"parsing.py : {passed_tests}/{total_tests} passed")
if failed_tests != 0:
    print(f"parsing.py : {failed_tests}/{total_tests} failed")
    sys.exit(1)

sys.exit(0)

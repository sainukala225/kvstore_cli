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
        "put without key",
        "put\n",
        "Error : 'put' requires key argument. Type 'help' for usage.\n",
    ),
    (
        "put without value",
        "put key\n",
        "Error : 'put' requires value argument. Type 'help' for usage.\n",
    ),
    (
        "put with extra argument",
        "put key value extra\n",
        "Error : 'put' takes only two argument. Type 'help' for usage.\n",
    ),
    (
        "get without key",
        "get\n",
        "Error : 'get' requires key argument. Type 'help' for usage.\n",
    ),
    (
        "get with extra argument",
        "get key value\n",
        "Error : 'get' takes only one argument. Type 'help' for usage.\n",
    ),
    (
        "delete without key",
        "delete\n",
        "Error : 'delete' requires key argument. Type 'help' for usage.\n",
    ),
    (
        "delete with extra argument",
        "delete key value\n",
        "Error : 'delete' takes only one argument. Type 'help' for usage.\n",
    ),
    (
        "save without filepath",
        "save\n",
        "Error : 'save' requires filepath argument. Type 'help' for usage.\n",
    ),
    (
        "save with extra argument",
        "save file extra\n",
        "Error : 'save' takes only one argument. Type 'help' for usage.\n",
    ),
    (
        "load without filepath",
        "load\n",
        "Error : 'load' requires filepath argument. Type 'help' for usage.\n",
    ),
    (
        "load with extra argument",
        "load file extra\n",
        "Error : 'load' takes only one argument. Type 'help' for usage.\n",
    ),
    (
        "exit with argument",
        "exit extra\n",
        "Error : 'exit' takes no arguments. Type 'help' for usage.\n",
    ),
    (
        "free with argument",
        "free extra\n",
        "Error : 'free' takes no arguments. Type 'help' for usage.\n",
    ),
    (
        "help with argument",
        "help extra\n",
        "Error : 'help' takes no arguments. Type 'help' for usage.\n",
    ),
    (
        "stats with argument",
        "stats extra\n",
        "Error : 'stats' takes no arguments. Type 'help' for usage.\n",
    ),
    (
        "invalid command",
        "puut key value\n",
        "Error : Invalid command. Type 'help' for a list of commands.\n",
    ),
]

failed_tests = 0
passed_tests = 0
total_tests = len(tests)

failures = []

for test_name, test, error in tests:
    passed = True
    process_out = run(test)
    if process_out.stdout != "":
        passed = False
        failures.append([test_name, "stdout", "", process_out.stdout])
    if process_out.stderr != error:
        passed = False
        failures.append([test_name, "stderr", error, process_out.stderr])
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
print(f"errors.py : {passed_tests}/{total_tests} passed")
if failed_tests != 0:
    print(f"errors.py : {failed_tests}/{total_tests} failed")
    sys.exit(1)

sys.exit(0)

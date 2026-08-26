#!/usr/bin/env python3
import os
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
os.chdir(HERE)
BIN = HERE.parent.parent / "build" / "bin" / "kvstore"
FIX = HERE / "fixtures"
BANNER = "********************* stats of kvstore ***********************"

failures = []


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


failed_tests = 0
passed_tests = 0


tests = [
    (
        "file_doesnt_exist",
        "load file_doesnt_exist",
        "",
        """Error : Failed to open the file file_doesnt_exist
Error : Failed to load the store
""",
    )
]

total_tests = len(tests)

for test_name, test_case, result, error in tests:
    passed = True

    process_out = run(test_case)

    if process_out.stdout != result:
        passed = False
        failures.append([test_name, "stdout", result, process_out.stdout])
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
print(f"load_failure_tests.py : {passed_tests}/{total_tests} passed")
if failed_tests != 0:
    print(f"load_failure_tests.py : {failed_tests}/{total_tests} failed")
    sys.exit(1)

sys.exit(0)

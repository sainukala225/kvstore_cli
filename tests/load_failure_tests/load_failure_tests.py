#!/usr/bin/env python3
import os
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
os.chdir(HERE)
BIN = HERE.parent.parent / "build" / "bin" / "kvstore"
FIX = HERE / "fixtures"
FIX.mkdir(exist_ok=True)
CASE = FIX / "case.db"
CASE_ARG = "fixtures/case.db"
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

load_db = f"put a 1\nput b 2\nstats\nload {CASE_ARG}\nstats"
tests = [
    (
        "file_doesnt_exist",
        "",
        "put a 1\nput b 2\nstats\nload file_doesnt_exist\nstats",
        """Error : Failed to open the file file_doesnt_exist
Error : Failed to load the store
""",
    ),
    (
        "reading_directory",
        "",
        f"put a 1\nput b 2\nstats\nload .\nstats",
        f"""Error in . on line 1 : Failed to read the line, aborting the load
Error : Failed to load the store
""",
    ),
    (
        "line_limit_exceeded",
        "a" * 1000,
        load_db,
        f"""Error in {CASE_ARG} on line 3 : The command size should be under 1000
Error : Failed to load the store
""",
    ),
    (
        "command_length_exceeded",
        "a" * 101,
        load_db,
        f"""Error in {CASE_ARG} on line 3 : Command is too long (should be at most 100)
Error : Failed to load the store
""",
    ),
    (
        "unterminated_quote_in_key",
        "put 'the quote in key is unterminated",
        load_db,
        f"""Error in {CASE_ARG} on line 3 : Unterminated quote in the Key
Error : Failed to load the store
""",
    ),
    (
        "unterminated_quote_in_value",
        "put key 'the quote in value is unterminated",
        load_db,
        f"""Error in {CASE_ARG} on line 3 : Unterminated quote in the Value
Error : Failed to load the store
""",
    ),
    (
        "invalid_escape_in_key",
        r"put 'k\ey' value",
        load_db,
        f"""Error in {CASE_ARG} on line 3 : Invalid escape in the Key
Error : Failed to load the store
""",
    ),
    (
        "invalid_escape_in_value",
        r"put k 'a\zcd'",
        load_db,
        f"""Error in {CASE_ARG} on line 3 : Invalid escape in the Value
Error : Failed to load the store
""",
    ),
    (
        "empty_key",
        "put",
        load_db,
        f"""Error in {CASE_ARG} on line 3 : 'put' requires key argument.
Error : Failed to load the store
""",
    ),
    (
        "empty_value",
        "put key",
        load_db,
        f"""Error in {CASE_ARG} on line 3 : 'put' requires value argument.
Error : Failed to load the store
""",
    ),
    (
        "extra_arg",
        "put key value extra",
        load_db,
        f"""Error in {CASE_ARG} on line 3 : 'put' takes only two argument.
Error : Failed to load the store
""",
    ),
    (
        "not_put_cmd",
        "get key",
        load_db,
        f"""Error in {CASE_ARG} on line 3 : Unexpected command get aborting the load. The file should only contain 'put' commands
Error : Failed to load the store
""",
    ),
    (
        "extra_long",
        "put key value " + "a" * 101,
        load_db,
        f"""Error in {CASE_ARG} on line 3 : Extra argument is too long (should be at most 100)
Error : Failed to load the store
""",
    ),
]

total_tests = len(tests)

for test_name, file_content, test_case, error in tests:
    passed = True
    CASE.write_text("put c 3\nput d 4\n" + file_content + "\n")
    process_out = run(test_case)

    blocks = process_out.stdout.split(BANNER)

    if len(blocks) != 3:
        passed = False
        failures.append([test_name, "stats", "3 blocks", f"{len(blocks)} blocks"])
    elif blocks[1] != blocks[2]:
        passed = False
        failures.append([test_name, "stats", blocks[1], blocks[2]])
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

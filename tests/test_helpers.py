import subprocess
import sys
from pathlib import Path
from typing import NamedTuple

HERE = Path(__file__).resolve().parent
BIN = HERE.parent / "build" / "bin" / "kvstore"


class Expect(NamedTuple):
    stdout: str = ""
    stderr: str = ""
    returncode: int = 0


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


def report(suite_name, passed_tests, failed_tests, total_tests, failures):
    for test_name, stream, expected, actual in failures:
        print(
            f"""Error in Test {test_name} {stream} :
        Expected : {expected!r}
        got : {actual!r}""",
            file=sys.stderr,
        )
    print(f"{suite_name} : {passed_tests}/{total_tests} passed")
    if failed_tests != 0:
        print(f"{suite_name} : {failed_tests}/{total_tests} failed")
        return 1
    return 0


def run_suite(suite_name, tests):
    failures = []
    passed_tests = 0
    failed_tests = 0
    for test_name, session, expected in tests:
        process_out = run(session)
        passed = True
        if process_out.stdout != expected.stdout:
            failures.append([test_name, "stdout", expected.stdout, process_out.stdout])
            passed = False
        if process_out.stderr != expected.stderr:
            failures.append([test_name, "stderr", expected.stderr, process_out.stderr])
            passed = False
        if process_out.returncode != expected.returncode:
            failures.append(
                [test_name, "returncode", expected.returncode, process_out.returncode]
            )
            passed = False
        if passed:
            passed_tests += 1
        else:
            failed_tests += 1
    return report(suite_name, passed_tests, failed_tests, len(tests), failures)

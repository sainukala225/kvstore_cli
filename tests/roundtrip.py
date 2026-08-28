#!/usr/bin/env python3

import difflib
import os
import sys
from pathlib import Path

from test_helpers import HERE, run

os.chdir(HERE)

gets_opr = r"""
stats
get integer1
get double_1
get double_2
get string
get 'hell\'0'
get 'escape\\'
get hex
get hex_double
get a
get b
get precise
get x
get y
get big
get '100 char'
get s
get unq100
"""

puts_opr = r"""
put integer1 1345
put double_1 14.99
put double_2 1e10
put string 'hello world'
put 'hell\'0' 'worl\'d'
put 'escape\\' 'this escapes'
put hex 0x25
put hex_double 0x2.3
put a 0x10
put b 0.5
put s '42'
put precise 0.30000000000000004
put x inf
put y nan
put big 1e400
put '100 char' 'abababababababababababababababababababababababababababababababababababababababababababababababababab'
put unq100 abababababababababababababababababababababababababababababababababababababababababababababababababab
"""

state_db_expected = r"""PUT 'integer1' 1345
PUT 'a' 16
PUT 's' 42
PUT 'hex' 37
PUT 'b' 0.5
PUT 'double_1' 14.99
PUT 'double_2' 10000000000.0
PUT 'hex_double' 2.1875
PUT 'precise' 0.30000000000000004
PUT 'x' inf
PUT 'y' nan
PUT 'big' '1e400'
PUT 'hell\'0' 'worl\'d'
PUT 'escape\\' 'this escapes'
PUT '100 char' 'abababababababababababababababababababababababababababababababababababababababababababababababababab'
PUT 'unq100' 'abababababababababababababababababababababababababababababababababababababababababababababababababab'
PUT 'string' 'hello world'
"""

save_opr = f"{puts_opr}{gets_opr}\nsave state.db"
save_process_out = run(save_opr)
if save_process_out.returncode != 0:
    print("Error : Failed to save the store", file=sys.stderr)
    sys.exit(1)

state_db = Path("state.db").read_text()
state_db_sorted = sorted(state_db.splitlines())
state_db_expected_sorted = sorted(state_db_expected.splitlines())

if state_db_sorted != state_db_expected_sorted:
    diff = "\n".join(
        difflib.unified_diff(
            state_db_expected_sorted, state_db_sorted, "expected", "got", lineterm=""
        )
    )

    print(
        f"""Error : state.db doesn't match expected state
{diff}""",
        file=sys.stderr,
    )

    sys.exit(1)


load_opr = f"load state.db\n{gets_opr}"
load_process_out = run(load_opr)
if load_process_out.returncode != 0:
    print("Error : Failed to load the store", file=sys.stderr)
    sys.exit(1)

passed = True
exitcode = 0

if save_process_out.stdout != load_process_out.stdout:
    print(
        f"""Error : output of save and load doesn't match
        save output : {save_process_out.stdout!r}
        load output : {load_process_out.stdout!r}""",
        file=sys.stderr,
    )
    passed = False
    exitcode = 1

if save_process_out.stderr != "":
    print(
        f"""Error : There are errors during save operation
        save errors : {save_process_out.stderr!r}""",
        file=sys.stderr,
    )
    passed = False
    exitcode = 1

if load_process_out.stderr != "":
    print(
        f"""Error : There are errors during load operation
        load errors : {load_process_out.stderr!r}""",
        file=sys.stderr,
    )
    passed = False
    exitcode = 1


if not passed:
    print("roundtrip.py : roundtrip failed")
else:
    print("roundtrip.py : roundtrip passed")
    os.remove(Path("state.db"))

sys.exit(exitcode)

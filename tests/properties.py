#!/usr/bin/env python3

import math
import random
import sys

from test_helpers import Expect, run_suite

NUM_OPS = 200

INTMAX_MIN = -(2**63)
INTMAX_MAX = 2**63 - 1

seed = random.randrange(2**32)
random.seed(seed)


def decode_word(word):
    if not word:
        return False, ""

    quoted = word[0] in "\"'"
    delimiter = word[0] if quoted else " "
    pos = 1 if quoted else 0

    out = []
    terminated = False
    n = len(word)

    while pos < n:
        ch = word[pos]
        if ch == "\\":
            if pos + 1 >= n:
                return False, ""
            nxt = word[pos + 1]
            if nxt == "\\" or nxt == delimiter:
                out.append(nxt)
                pos += 2
                continue
            return False, ""
        if ch == delimiter:
            terminated = True
            pos += 1
            break
        out.append(ch)
        pos += 1

    if quoted and not terminated:
        return False, ""
    if pos != n:
        return False, ""
    return True, "".join(out)


def try_int(text):
    if "_" in text:
        return False, None

    body = text[1:] if text[:1] in ("+", "-") else text
    if not body:
        return False, None

    prefix = body[:2].lower()
    if prefix == "0x":
        base = 16
    elif prefix == "0b":
        base = 2
    elif body[0] == "0" and len(body) > 1:
        base = 8
    else:
        base = 10

    try:
        result = int(text, base)
    except ValueError:
        return False, None

    if result < INTMAX_MIN or result > INTMAX_MAX:
        return False, None
    return True, result


def try_float(text):
    if "_" in text:
        return False, None

    body = text[1:] if text[:1] in ("+", "-") else text
    try:
        if body[:2].lower() == "0x":
            result = float.fromhex(text)
        else:
            result = float(text)
    except ValueError:
        return False, None

    if math.isinf(result) and text.lstrip("+-").lower() not in ("inf", "infinity"):
        return False, None
    return True, result


def render(text):
    ok, decoded = decode_word(text)
    if not ok:
        raise ValueError(f"undecodable entry in VALUES: {text!r}")

    ok, int_val = try_int(decoded)
    if ok:
        return str(int_val)

    ok, float_val = try_float(decoded)
    if ok:
        out = f"{float_val:g}"
        if not any(m in out for m in (".", "e", "E", "inf", "nan")):
            out += ".0"
        return out

    return f'"{decoded}"'


KEYS = [
    "a",
    "b",
    "k",
    "key",
    "x",
    "y",
    "s",
    "zz",
    "n1",
    "n2",
    "'z'",
    "'a b'",
    r"a\ b",
    r"'a\'b'",
    r"'a\\b'",
    "a" * 100,
]

VALUES = [
    "123",
    "-45",
    "0",
    "012",
    "-012",
    "0x1E",
    "-0x10",
    "0b11",
    "0b11111111",
    "4.11",
    "16.0",
    "1e5",
    "1.5e300",
    "0x1p4",
    "0.30000000000000004",
    "9007199254740993",
    "9223372036854775807",
    "9223372036854775808",
    "-9223372036854775809",
    "1e400",
    "inf",
    "-inf",
    "Infinity",
    "NaN",
    "hello",
    "'hello world'",
    "'42'",
    "'  spaced  '",
    r"'a\\b'",
]

session = ""
session_stdout = ""
session_stderr = ""
store = {}

for _ in range(NUM_OPS):
    op = random.choices(["put", "get", "delete", "free"], weights=[50, 30, 15, 5])[0]
    key_text = random.choice(KEYS)
    value = random.choice(VALUES)

    ok, key = decode_word(key_text)
    if not ok:
        raise ValueError(f"undecodable entry in KEYS: {key_text!r}")

    if op == "put":
        session += f"put {key_text} {value}\n"
        store[key] = value
    elif op == "get":
        session += f"get {key_text}\n"
        if key in store:
            session_stdout += f'"{key}" : {render(store[key])}\n'
        else:
            session_stderr += "Error : Item doesn't exist\n"
    elif op == "delete":
        session += f"delete {key_text}\n"
        if key in store:
            del store[key]
        else:
            session_stderr += f"Error : key '{key}' doesn't exist in store\n"
    elif op == "free":
        session += "free\n"
        store = {}

tests = [
    (
        "properties test",
        session,
        Expect(stdout=session_stdout, stderr=session_stderr),
    )
]

exit_code = run_suite("properties.py", tests)
if exit_code != 0:
    print(f"properties.py : seed {seed}", file=sys.stderr)
sys.exit(exit_code)

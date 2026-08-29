# kvstore_cli

An interactive key/value store in C, with typed values and crash-safe persistence.

[![tests](https://github.com/sainukala225/kvstore_cli/actions/workflows/ci.yml/badge.svg)](https://github.com/sainukala225/kvstore_cli/actions/workflows/ci.yml)

## Introduction

The basic operations are:

| command | effect |
| --- | --- |
| `put <key> <value>` | store a value under a key |
| `get <key>` | print the value |
| `delete <key>` | remove a key |

`save <file>` writes the current state to a file and `load <file>` merges it back into the store atomically: either the whole file is applied or none of it is.

## Installation

1. Clone this repo:

   ```bash
   git clone https://github.com/sainukala225/kvstore_cli
   ```
2. Change to the kvstore_cli directory and build:

   ```bash
   cd kvstore_cli
   make
   ```
3. Run it:

   ```bash
   make run
   ```

Note: this needs GCC 13 or newer.

## Usage

A session showing the three value types, persistence and a missing key. The startup banner is omitted.

```text
sai@fedora:~/kvstore_cli$ make run

enter the command : put name 'sai'
enter the command : put count 42
enter the command : put ratio 4.11
enter the command : put hex 0x1E
enter the command : get name
"name" : "sai"
enter the command : get count
"count" : 42
enter the command : get ratio
"ratio" : 4.11
enter the command : get hex
"hex" : 30
enter the command : stats
********************* stats of kvstore ***********************
* The bucket size (capacity)     : 101                       *
* The number of occupied_buckets :   4                       *
* The total items in the store   :   4                       *
* The load factor                : 0.04                      *
* The number of int items        :   2                       *
* The number of double items     :   1                       *
* The number of string items     :   1                       *
**************************************************************
enter the command : save state.db
store saved to file state.db successfully
enter the command : free
enter the command : get name
Error : Item doesn't exist
enter the command : load state.db
store loaded from file state.db successfully
enter the command : get name
"name" : "sai"
enter the command : get hex
"hex" : 30
enter the command : delete count
enter the command : get count
Error : Item doesn't exist
enter the command : exit
exited program successfully.
```


## Commands

| command | arguments | effect |
| --- | --- | --- |
| `put` | key, value | store a value under a key, replacing any existing one |
| `get` | key | print the key and its value |
| `delete` | key | remove a key |
| `save` | filepath | write the store to a file |
| `load` | filepath | merge a file into the store |
| `free` | none | empty the store |
| `stats` | none | print bucket and item counts |
| `help` | none | list the commands |
| `exit` | none | leave the shell |

Command words are matched case-insensitively, so `put`, `Put` and `PUT` are the same. The `-T` flag suppresses the banner, the prompt and the exit message, for use in a pipe:

```bash
printf 'put a 1\nget a\n' | ./build/bin/kvstore -T
```

## Values and parsing

A value is stored as one of three types: **integer**, **double** or **string**. The type is inferred from the text you type. There is no way to force one.

Parsing is tried in order: integer first, then double, then string if neither consumed the whole text. The integer parse uses base 0, so C numeric-literal rules apply:

| typed | stored as |
| --- | --- |
| `123` | integer 123 |
| `0x1E` | integer 30 |
| `0b11` | integer 3 |
| `01234` | integer **668** (a leading zero means octal) |
| `4.11` | double 4.11 |
| `1e400` | string, since it exceeds double range |
| `hello` | string |

A zero-padded identifier does not survive as typed. `inf`, `infinity`, `nan` and `nan(...)` are accepted as doubles, case-insensitively, so `INF` and `Infinity` are numbers rather than strings. An integer beyond the platform's range becomes a double; a double beyond its range becomes a string. Nothing is clamped to a boundary value.

Binary `0b` literals need glibc 2.38 or newer.

**Quoting.** A word may be wrapped in `'` or `"`, which lets it contain spaces. An unquoted word ends at the first space, unless that space is escaped with a backslash:

```text
put 'a b' 1
put "a b" 1
put a\ b 1
```

All three store the key `a b`.

**Escaping.** Exactly two escapes are accepted: a backslash before another backslash, and a backslash before the character that would otherwise end the word. That character is the closing quote for a quoted word, a space for an unquoted one. Anything else after a backslash is rejected and the whole line is abandoned. The set is this small to keep the encoding reversible: two different keys can never collapse into one.

**Quotes are stripped before the value is typed**, so quoting is not a type hint: `put s '42'` stores the **integer** 42.

**Limits.** A word may be at most 100 characters and a line at most 999. Both are reported rather than truncated, and the line is abandoned.

**Output.** `get` prints the key in double quotes, then the value. String values are double-quoted; integers and doubles are printed bare. Doubles use `%g`, with a `.0` suffix when the result would otherwise read as an integer, so a double `16.0` prints as `16.0` rather than `16`.

## The save file

The file is human-readable text, one `PUT` line per entry. Keys and string values are single-quoted; numbers are bare, doubles at full precision so they reload exactly.

```text
PUT 'name' 'sai nukala'
PUT 'count' 42
PUT 'ratio' 4.11
```

The file is a **snapshot of current contents, not a log of operations**, so `PUT` is the only verb in it. `save` writes a temporary file, flushes it, syncs it to disk, checks the close status, then renames it over the target. A reader never sees a partial file.

`load` is all-or-nothing. A corrupt line leaves the store exactly as it was, and every rejection names the file and the line number. Blank lines are skipped, and the verb is matched case-insensitively.

## Testing

```bash
make test
```

builds both binaries and runs seven suites, six in Python and one compiled C program:

| suite | covers |
| --- | --- |
| `cases.py` | value typing across every classification branch |
| `errors.py` | CLI error messages and argument validation |
| `parsing.py` | tokenizer behaviour, quoting and length boundaries |
| `state_test` | internal counters, compiled from `tests/state.c` and calling the library API directly rather than driving the shell |
| `roundtrip.py` | save, reload and compare, including the on-disk format |
| `load_failure_tests.py` | every way a load can fail, and that the store is left untouched |
| `properties.py` | a random operation sequence compared against an independent model |

CI runs the same command on every push.

## Limitations

- **Not thread-safe.** The store has no locking, so it is single-threaded only.
- **Fixed bucket count, no rehashing.** The table is 101 buckets and does not grow, so lookups degrade as the load factor climbs.
- **A numeric-looking string cannot be stored as a string**, because quotes are stripped before the value is typed.
- **`get` output does not escape quotes inside keys.** `put 'a"b' 1` then `get 'a"b'` prints `"a"b" : 1`, which cannot be parsed unambiguously. The on-disk format escapes correctly, so a round trip is unaffected. Only the display is.

#!/usr/bin/env bash

set -u
cd "$(dirname "$0")"

fail=0
for t in cases_tests/cases.py errors_tests/errors.py parsing_tests/parsing.py \
  state_tests/state.sh roundtrip_tests/roundtrip.py \
  load_failure_tests/load_failure_tests.py; do
  "$t" || fail=1
done

exit $fail

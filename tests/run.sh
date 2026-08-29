#!/usr/bin/env bash

set -u
cd "$(dirname "$0")"

fail=0
for t in ./cases.py ./errors.py ./parsing.py \
  ../build/bin/state_test ./roundtrip.py \
  ./load_failure_tests.py ./properties.py; do
  "$t" || fail=1
done

exit $fail

#!/usr/bin/env bash

set -u
cd "$(dirname "$0")"

fail=0
for t in cases_tests/cases.sh errors_tests/errors.sh parsing_tests/parsing.sh state_tests/state.sh; do
  "$t" || fail=1
done

exit $fail

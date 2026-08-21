#!/usr/bin/env bash

set -u
cd "$(dirname "$0")"

cat <<'EOF' | ../../build/bin/kvstore -T 1>state.results 2>state.errors

put k abc
stats
put k 123
stats

put k 345
stats

delete k
stats

put integer 2
put float 4.11
put string hello
stats
free
stats

free
put a 1
put bx 2
stats
get a
get bx

get key_not_in_store

delete key_not_in_store

delete bx
get bx

get a

free
get a
EOF

total_tests=10

diff state.results state.results.expected
results_ok=$?
diff state.errors state.errors.expected
errors_ok=$?

if [[ $results_ok -eq 0 && $errors_ok -eq 0 ]]; then
  echo "state.sh: $total_tests/$total_tests test cases passed"
  exit 0
else
  different_lines=$(($(diff state.results state.results.expected | grep -c '^[><]') + $(diff state.errors state.errors.expected | grep -c '^[><]')))
  echo "state.sh: ${different_lines} lines differ between expected and actual results"
  exit 1
fi

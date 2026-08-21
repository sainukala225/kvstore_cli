#!/usr/bin/env bash

set -u
cd "$(dirname "$0")"

cat <<'EOF' | ../../build/bin/kvstore -T 1>errors.results 2>errors.errors

put
put key
put key value extra

get
get key value

delete
delete key value

save
save file extra

load
load file extra

exit extra
free extra
help extra
stats extra

puut key value
EOF

total_result_tests=$(wc -l <errors.results.expected)
total_result_tests_failed=$(diff errors.results errors.results.expected | grep -c '^>')

total_error_tests=$(wc -l <errors.errors.expected)
total_error_tests_failed=$(diff errors.errors errors.errors.expected | grep -c '^>')

total_tests=$((total_result_tests + total_error_tests))
total_tests_failed=$((total_result_tests_failed + total_error_tests_failed))

if [[ $total_tests_failed -eq 0 ]]; then
  echo "error.sh: ${total_tests}/${total_tests} Test cases passed"
  exit 0
else
  echo "error.sh: ${total_result_tests_failed}/${total_result_tests} result tests failed"
  echo "error.sh: ${total_error_tests_failed}/${total_error_tests} error tests failed"
  exit 1
fi

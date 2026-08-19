#!/usr/bin/env bash

cd "$(dirname "$0")"

cat <<'EOF' | ../build/bin/kvstore -T 1>cases.results 2>cases.errors

put dec_int1 123
get dec_int1

put dec_int2 -45
get dec_int2

put oct_int1 012
get oct_int1

put hex_int1 0x1E
get hex_int1

put hex_int2 -0x10
get hex_int2

put int_double1 0.5
get int_double1

put int_double2 1.5
get int_double2

put exp_double 1e5
get exp_double

put hex_double1 0x1E.2
get hex_double1

put hex_double2 0x1p4
get hex_double2

put bin_val1 0b11
get bin_val1

put string_val1 abc
get string_val1

put string_val2 1e999
get string_val2

put inf1 inf
get inf1

put inf2 Infinity
get inf2

put nan NaN
get nan

put int_max 9223372036854775807
get int_max

put above_int_max 9223372036854775808
get above_int_max

put below_int_min -9223372036854775809
get below_int_min

put highnum 99999999999999999999999999
get highnum

put int_not_via_double 9007199254740993
get int_not_via_double

exit
EOF

diff cases.results cases.results.expected

results=$?

diff cases.errors cases.errors.expected

errors=$?

if [[ $results -eq 0 && $errors -eq 0 ]]; then
  echo "Test cases passed"
  exit 0
else
  echo "Test cases output don't match expected results"
  exit 1
fi

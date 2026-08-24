#!/usr/bin/env bash

set -u

cd "$(dirname "$0")"

{
  cat puts.txt gets.txt
  echo "save state.db"
} | ../../build/bin/kvstore -T 1>roundtrip_save.results 2>roundtrip_save.errors
save_status=$?

if [[ $save_status -ne 0 ]]; then
  echo "roundtrip.sh : save operation failed"
  exit 1
fi

{
  echo "load state.db"
  cat gets.txt
} | ../../build/bin/kvstore -T 1>roundtrip_load.results 2>roundtrip_load.errors
load_status=$?

if [[ $load_status -ne 0 ]]; then
  echo "roundtrip.sh : load operation failed"
  exit 1
fi

if [[ -s roundtrip_save.errors || -s roundtrip_load.errors ]]; then
  echo "roundtrip.sh : There are errors during roundtrip."
  cat roundtrip_save.errors roundtrip_load.errors
  exit 1
fi

diff <(sort state.db) <(sort state.db.expected)
state_status=$?

if [[ $state_status -ne 0 ]]; then
  echo "roundtrip.sh : The stored state doesn't match expected state."
  exit 1
fi

rm state.db

diff roundtrip_save.results roundtrip_load.results
result_status=$?

total_keys=$(wc -l <state.db.expected)

if [[ $result_status -eq 0 ]]; then
  echo "roundtrip.sh : $total_keys/$total_keys keys loaded successfully"
  exit 0
else
  echo "roundtrip.sh : roundtrip test failed"
  exit 1
fi

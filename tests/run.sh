#!/usr/bin/env bash

cd "$(dirname "$0")"

./cases_tests/cases.sh
./errors_tests/errors.sh
./parsing_tests/parsing.sh

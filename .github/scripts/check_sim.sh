#!/bin/bash
# Copyright (c) 2025 ETH Zurich and University of Bologna.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0

# Author:  Philippe Sauter <phsauter@iis.ee.ethz.ch>

LOG_FILE=$1

expected_lines=(
  "\[CORE\] Start fetching instructions"
  "\[JTAG\] Halting hart 0"
  "\[JTAG\] Resumed hart 0"
)

for line in "${expected_lines[@]}"; do
  if ! grep -q "$line" "$LOG_FILE"; then
    echo "Error: Expected line not found in the log: '$line'"
    exit 1
  fi
done

echo "Simulation passed."
exit 0

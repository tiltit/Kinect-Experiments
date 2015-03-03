#!/bin/bash
find . -name "*.pgm" -type f -print | xargs -n 1 permutate-pgm-endianness

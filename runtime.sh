#!/bin/bash
{ time ./$1 < $2 > test.out ; } 2>&1 | grep user | awk '{print $2}'

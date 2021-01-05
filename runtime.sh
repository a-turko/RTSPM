#!/bin/bash
{ time ./$1 < $2 > /dev/null ; } 2>&1 | grep user | awk '{print $2}'

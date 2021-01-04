#!/bin/bash
./$1 < $2 2>&1 | grep VmPeak

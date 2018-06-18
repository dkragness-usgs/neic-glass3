# ========================
# DK REVIEW 20180618
# This file should be separated out into two or more files, similar to what we do in Hydra
# Where project wide stuff should show up in one common file instead of having redundant
# copies, while sub-project stuff shows up in it's own file, pulling commone pieces as
# necessary.  The goal of course being not having redundant copies of the same "processing info",
# so that it doesn't have to be tracked/changed in multiple places.
# ========================

#!/bin/sh
currentpath=`pwd`

echo "Current Path: ${currentpath}"
echo "\ncapture coverage info"
lcov --directory . --capture --output-file coverage.info
echo "\nfilter out system libraries"
lcov --remove coverage.info '/usr/*' '/Applications/*' --output-file coverage.info
echo "\nfilter out external libraries"
lcov --remove coverage.info '*distribution/*' --output-file coverage.info
echo "\nfilter out tests"
lcov --remove coverage.info '*tests/*' --output-file coverage.info
echo "\nprint coverage report"
lcov --list coverage.info

#!/bin/bash

if [ -z "$NESTING" ] || [ -z "$INET" ]
then
    echo "The \$NESTING and \$INET environment variables must contain the paths to the NESTING and INET directories."
    exit 1 
fi

if [ "$MODE" = "debug" ]
then
    D="_dbg"
else
    D=""
fi

echo
echo "=== Preprocessing tests ==="
echo
python3 $NESTING/scripts/preprocess_tests.py

echo
echo "=== Generating tests ==="
echo
mkdir -p $NESTING/work
opp_test gen -v -w $NESTING/work $NESTING/tests/*.test || exit 1

echo
echo "=== Building tests ==="
echo
(cd $NESTING/work; opp_makemake -f --deep -lnesting$D -L$NESTING/src -lINET$D -L$INET/src -I$NESTING/src -I$INET/src -o nesting; make) || exit 1

echo
echo "=== Running tests ==="
echo
opp_test run -v -w $NESTING/work -p $NESTING/work/nesting$D $NESTING/tests/*.test \
    -a "-n $NESTING/work:$NESTING/src:$INET/src"

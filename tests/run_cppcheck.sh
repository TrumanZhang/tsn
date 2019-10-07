if [ -z "$OMNETPP" ] || [ -z "$NESTING" ] || [ -z "$INET" ]
then
    echo "The \$OMNETPP, \$NESTING and \$INET environment variables must contain the paths to the OMNET++, NESTING and INET directories."
    exit 1 
fi

if [ -z "$1" ]
then
    CPPCHECK_REPORT="cppcheck_report.json"
else
    CPPCHECK_REPORT=$1
fi

cppcheck \
    --std=c++11 \
    --suppress="*:*omnetpp\*" \
    --suppress="*:*inet\*" \
    --suppress="*:*_m.cc" \
    --suppress="*:*_m.h" \
    --enable=all \
    -I $OMNETPP/include \
    -I $INET/src \
    -I $NESTING/ \
    $NESTING/src \
    --template='{"type": "issue","description": "{message}","check_name": "{id}","location": {"path": "{file}","lines": {"begin": "{line}"}}, "severity": "{severity}", "fingerprint": ""}' \
    2> $CPPCHECK_REPORT.tmp

(echo "["; sed -e '$s/\({.*}\),/\1/;/"severity": "information"/d' $CPPCHECK_REPORT.tmp; echo "]") > $CPPCHECK_REPORT

#rm $CPPCHECK_REPORT.tmp
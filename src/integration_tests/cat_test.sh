#!/bin/bash

SUCCESS=0
FAIL=0
COUNTER=0
DIFF_RES=""
FILE=$(find $(pwd) -name test.txt)

cd ../C2_SimpleBashUtils/src/cat/

make

./s21_cat $FILE > test_s21_cat.log
cat $FILE > test_sys_cat.log
DIFF_RES="$(diff -s test_s21_cat.log test_sys_cat.log)"
(( COUNTER++ ))
if [ "$DIFF_RES" == "Files test_s21_cat.log and test_sys_cat.log are identical" ]
then
    (( SUCCESS++ ))
    echo "SUCCESS"
else
    (( FAIL++ ))
    echo "FAIL"
fi
    rm test_s21_cat.log test_sys_cat.log

rm s21_cat

echo "FAIL: $FAIL"
echo "SUCCESS: $SUCCESS"
echo "ALL: $COUNTER"
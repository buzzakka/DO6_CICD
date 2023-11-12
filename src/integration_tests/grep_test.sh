SUCCESS=0
FAIL=0
COUNTER=0
DIFF_RES=""
FILE=$(find $(pwd) -name test.txt)

cd ../C2_SimpleBashUtils/src/grep/

make

./s21_grep 12 $FILE > test_s21_grep.log
grep 12 $FILE > test_sys_grep.log
DIFF_RES="$(diff -s test_s21_grep.log test_sys_grep.log)"
(( COUNTER++ ))
if [ "$DIFF_RES" == "Files test_s21_grep.log and test_sys_grep.log are identical" ]
then
    (( SUCCESS++ ))
    echo "SUCCESS"
else
    (( FAIL++ ))
    echo "FAIL"
fi
    rm test_s21_grep.log test_sys_grep.log

rm s21_grep

echo "FAIL: $FAIL"
echo "SUCCESS: $SUCCESS"
echo "ALL: $COUNTER"
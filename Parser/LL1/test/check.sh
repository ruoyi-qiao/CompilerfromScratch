# write a script to check the correctness of your parser
# read from [%d].in and write to [%d].out
# then compare [%d].out with _[%d].out
# if they are the same, print "Test [%d] passed"
# otherwise, print "Test [%d] failed"

cd test

for i in {1..3}
do
    ../ll1Parser < $i.in > $i.out
    diff $i.out _$i.out
    if [ $? -eq 0 ]
    then
        echo "Test $i passed"
    else
        echo "Test $i failed"
    fi
done

cd ..
g++ -std=c++11 -o lr1Parser LR1Parser.cpp

 
cd test

for i in {1..3}
do 
    ../lr1Parser < $i.in > _$i.out
    diff $i.out _$i.out > /dev/null
    if [ $? -eq 0 ]
    then
        echo "Test $i passed"
    else
        echo "Test $i failed"
    fi
done

cd ..

rm lr1Parser
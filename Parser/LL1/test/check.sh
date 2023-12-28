g++ -std=c++11 -o ll1Parser ll1Parser.cpp

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

rm ll1Parser   
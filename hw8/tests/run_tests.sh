
rm ./tests/user*;
for i in `seq 1 6`;
do 
	./tests/test$i.sh | ./debugger tests/prog1 | grep -v "^$" | grep -v "the program terminated" > /tmp/user$i.out;

	#last minute hack to avoid users that did added features
	if [ $i -eq 3 ]
	then
		cat /tmp/user$i.out | grep -v "int sum=0;" > ./tests/user$i.out;
	else
		cat /tmp/user$i.out > ./tests/user$i.out
	fi

	out=`diff -u ./tests/user$i.out ./tests/test$i.out | wc -l;`
	if [ ${#out} -lt 1 ]
	then
		echo "Test "$i" failed"
	else
		if [ $((out)) -eq 0 ] 
		then
			echo "Test "$i" passed"
		else
			echo "Test "$i" failed"
		fi
	fi	
done

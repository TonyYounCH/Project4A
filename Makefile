# NAME: Changhui Youn
# EMAIL: tonyyoun2@gmail.com
# ID: 304207830

default:
# 	gcc -g -lm -lmraa -Wall -Wextra -DDUMMY lab4b.c -o lab4b
	chmod +x ./compile.sh
	./compile.sh

check: default
	echo | ./lab4b --badArg &> /dev/null
	if [[ $? -ne 1 ]]
	then
		echo "test for bad option failed"
	else
		echo "test for bad option passed"
	fi

	echo | ./lab4b --scale=badArg &> /dev/null
	if [[ $? -ne 1 ]]
	then	
		echo "test for bad argument failed"
	else
		echo "test for bad argument passed"
	fi

	./lab4b --period=3 --scale=F --log=LOG <<-EOF
	SCALE=C
	PERIOD=5
	STOP
	START
	LOG 
	OFF
	EOF
	if [[ $? -ne 0 ]]
	then
		echo "Test failed: Invalid exit code returned for correct input"
	else
		echo "Test passed: Valid exit code for correct input"
	fi

	if [ ! -s LOG ]
	then
		echo "Test failed: Logfile was not created"
	else
		echo "Test passed: Logfile created"
	fi

	grep "PERIOD=5" LOG &> /dev/null; \
	if [[ $? -ne 0 ]]
	then
	        echo "Test failed: PERIOD was not logged in Logfile"
	else
	        echo "Test passed: PERIOD was logged in logfile"
	fi


	grep "SCALE=C" LOG &> /dev/null; \
	if [[ $? -ne 0 ]]
	then
		echo "Test failed: SCALE was not logged in Logfile"
	else
		echo "Test passed: SCALE was logged in logfile"
	fi

	grep "LOG" LOG &> /dev/null; \
	if [[ $? -ne 0 ]]
	then
	        echo "Test failed: LOG was not logged in Logfile"
	else
		echo "Test passed: LOG was logged in logfile"
	fi

	grep "SHUTDOWN" LOG &> /dev/null; \
	if [[ $? -ne 0 ]]
	then
	        echo "Test failed: SHUTDOWN was not logged in Logfile"
	else
		echo "Test passed: SHUTDOWN was logged in logfile"
	fi

	rm -f LOG




dist: check
	tar -czf lab4b-304207830.tar.gz lab4b.c Makefile README compile.sh

clean:
	rm -f *.o lab4b *.txt *.tar.gz

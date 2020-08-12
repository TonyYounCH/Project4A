# NAME: Changhui Youn
# EMAIL: tonyyoun2@gmail.com
# ID: 304207830

default:
	gcc -g -lm -lmraa -Wall -Wextra lab4b.c -o lab4b

check: default
	{ echo "START"; sleep 2; echo "STOP"; sleep 2; echo "OFF"; } | ./lab4b --log=log.txt

	if [ $? -ne 0 ]
	then
		echo "Error: program should have exited with 0"
	else
		echo "good return value!"
	fi

	for c in START STOP OFF SHUTDOWN
		do
			grep "$c" log.txt > /dev/null
			if [ $? -ne 0 ]
			then
				echo "failed to log $c command"
			else
				echo "$c was logged successfully!"
			fi
		done

	rm -f log.txt


dist: check
	tar -czf lab4b-304207830.tar.gz lab4b.c Makefile README

clean:
	rm -f *.o lab4b *.txt *.tar.gz

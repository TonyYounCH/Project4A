# NAME: Changhui Youn
# EMAIL: tonyyoun2@gmail.com
# ID: 304207830

default:
	chmod +x ./compile.sh
	./compile.sh

check: default
	chmod +x tests.sh
	./tests.sh


dist: check
	tar -czf lab4b-304207830.tar.gz lab4b.c Makefile README compile.sh tests.sh

clean:
	rm -f *.o lab4b *.txt *.tar.gz

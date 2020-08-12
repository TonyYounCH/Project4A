#!/bin/bash

result=$(uname -a)
if [[ $result == *"beaglebone"* ]]; then
	gcc -g -lm -lmraa -Wall -Wextra lab4b.c -o lab4b
else
	gcc -g -lm -lmraa -Wall -Wextra -DDUMMY lab4b.c -o lab4b
fi
CURRENT = $(shell uname -r)

all: user module
module:		
	./module/make.sh
user:		
	gcc -Wall -o ./client/tufilter ./client/tufilter.c

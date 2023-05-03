.PHONY: all
all: task stdinExample coder

task:	codec.h basic_main.c
	gcc basic_main.c ./libCodec.so -L. -l Codec -o encoder 

stdinExample:	stdin_main.c
		gcc stdin_main.c ./libCodec.so -L. -l Codec -o tester

coder: codec.h our_codec.cpp our_codec.hpp
	g++ our_codec.cpp ./libCodec.so -L. -l Codec -o coder -lpthread

.PHONY: clean
clean:
	-rm encoder tester coder 2>/dev/null

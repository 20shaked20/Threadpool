.PHONY: all
all: stdinExample coder

stdinExample:	stdin_main.c
		gcc stdin_main.c ./libCodec.so -L. -l Codec -o tester

coder: codec.h our_codec.cpp our_codec.hpp
	g++ our_codec.cpp ./libCodec.so -L. -l Codec -o coder -lpthread -ldl

.PHONY: clean
clean:
	-rm encoder tester coder 2>/dev/null

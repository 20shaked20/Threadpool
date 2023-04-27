.PHONY: all
all: task stdinExample

task:	codec.h basic_main.c
	gcc basic_main.c ./libCodec.so -L. -l Codec -o encoder

stdinExample:	stdin_main.c
		gcc stdin_main.c ./libCodec.so -L. -l Codec -o tester

coder: codec.h our_codec.c our_codec.h
	gcc our_codec.c ./libCodec.so -L. -l Codec -o coder

.PHONY: clean
clean:
	-rm encoder tester 2>/dev/null

.PHONY: all
all: task stdinExample coder

task:	codec.h basic_main.c
	gcc basic_main.c ./libCodec.so -L. -l Codec -o encoder 

stdinExample:	stdin_main.c
		gcc stdin_main.c ./libCodec.so -L. -l Codec -o tester

coder: codec.h our_codec.c our_codec.h queue.h queue.c 
	gcc our_codec.c ./libCodec.so -L. -l Codec -o coder -lpthread

#queue: queue.h queue.c 
#	gcc queue.c ./libCodec.so -L. -l Codec -o queue

.PHONY: clean
clean:
	-rm encoder tester coder 2>/dev/null

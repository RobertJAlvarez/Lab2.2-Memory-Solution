CC   = cc
OBJS = my_string.o tokenizer.o process_file.o BST.o main.o

# -Wextra
CFLAGS = -O3 -g3 -Wall -Werror=format-security -Werror=implicit-function-declaration \
         -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wwrite-strings -Wconversion

all: main

%.o: %.c
	${CC} $(CFLAGS) -c -o $@ $<

main: $(OBJS)
	${CC} -o $@ $^

run: main
	./main Inventory.txt

clean:
	rm -f *.o main

BST.o: BST.c BST.h tokenizer.h process_file.h
process_file.o: process_file.c process_file.h my_string.h
my_string.o: my_string.c my_string.h
tokenizer.o: tokenizer.c tokenizer.h my_string.h


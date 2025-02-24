SRC = mymalloc.c
OUTPUT = test test2 test3 test4
CFLAGS = -g -Wall -Werror -fsanitize=address

all: $(OUTPUT)


%: %.c $(SRC)
	gcc $(CFLAGS) $^ -o $@


clean:
	rm -f $(OUTPUT)


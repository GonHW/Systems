OUTPUT = tree find ls
CFLAGS = -g -Wall -Werror -fsanitize=address

all: $(OUTPUT)


%.o: %.c
	gcc $(CFLAGS) $(INCLUDE) -c -o $@ $<


clean:
	rm -f $(OUTPUT)

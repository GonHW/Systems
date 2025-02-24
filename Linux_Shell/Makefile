OUTPUT = shell
CFLAGS = -g -Wall -Werror -fsanitize=address

%.o: %.c
	gcc $(CFLAGS) $(INCLUDE).c -o $@ $<

all: $(OUTPUT)

clean:
	rm -f $(OUTPUT)

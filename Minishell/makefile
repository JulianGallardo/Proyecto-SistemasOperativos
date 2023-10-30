CC = gcc
CFLAGS = -Wall -Wextra
SOURCES = cat.c chmod.c gelp.c ls.c minishell.c mkdir.c rmdir.c touch.c
OBJS = $(SOURCES:.c=.o)
EXECUTABLE = minishell

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

run: $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm -f $(OBJS) $(EXECUTABLE)

.PHONY: run clean all

	
	

	

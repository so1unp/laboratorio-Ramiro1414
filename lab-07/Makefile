CC=gcc
BIN=./bin
CFLAGS=-Wall -g -pthread -Werror

PROG=maps sim

LIST=$(addprefix $(BIN)/, $(PROG))

.PHONY: all
all: $(LIST)

$(BIN)/%: %.c
	$(CC) -o $@ $< $(CFLAGS)

%: %.c
	$(CC) -o $(BIN)/$@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(LIST)

zip:
	git archive --format zip --output ${USER}-lab07.zip HEAD

html:
	pandoc -o README.html -f gfm README.md

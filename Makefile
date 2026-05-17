CC = gcc
CFLAGS = -g -Wall -Wextra -std=gnu11 -Ivector/include -I./include

LDLIBS = -lm

SRC = main.c \
      vector/src/file_vector.c \
      vector/src/file_data.c \
      src/flags/flag_parser.c \
      src/flags/flag_setters.c \
      src/print/print.c \
      src/sorting.c \
      src/file_scan.c \
      src/print/format_info.c \
      src/ls.c \
      src/flags/flags.c \
      src/print/print_utils.c \
	  src/print/print_long.c \
	  src/print/print_columns.c \
	  src/print/print_horizontal.c \
	  src/print/print_one_per_line.c \
	  src/print/print_standard.c \
	  src/print/print_commas.c 

OBJ = $(patsubst %.c,build/%.o,$(SRC))

TARGET = my_ls

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDLIBS)

build/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build $(TARGET)

re: clean all

.PHONY: all clean re

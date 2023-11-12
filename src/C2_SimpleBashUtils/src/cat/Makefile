CC = gcc
CFLAGS = -Wall -Werror -Wextra
C11 = -std=c11

all: rebuild

build: s21_cat

s21_cat:
	$(CC) $(CFLAGS) $(C11) s21_cat.c -o s21_cat

clean:
	rm -rf s21_cat

style:
	cp ../../../materials/linters/.clang-format .
	clang-format -n s21_cat.c
	rm -rf .clang-format

rebuild: clean build
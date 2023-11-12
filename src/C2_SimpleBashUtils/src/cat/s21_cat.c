#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 2056

typedef struct {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
  int error;
} cat_flags;

/*
            -- Парсер --
*/
void parser(int argc, char **argv, cat_flags *input_flags);
void flag_checker(int argc, char **argv, cat_flags *input_flags);

/*
            -- Cat - печать --
    cat: Общая ункция Cat.
    cat_print: Вывод данных из файла на экран.
*/
void cat(char **argv, cat_flags *input_flags);
void cat_print(FILE *file, cat_flags *input_flags);
void n_flag(cat_flags *input_flags, int *str_num_count);
void b_flag(cat_flags *input_flags, int *str_num_count, char str,
            int new_line_flag);
void e_flag(cat_flags *input_flags);
void t_flag();
void v_flag(char str, cat_flags *input_flags);

/*
            -- ERROR --
    no_file_error: Ошибка при отсутствии файла в argv.
    error_exit: Выводит ошибку и закрывает прогу.
    check_memory: Защита малоков.
*/
int no_file_error(char **argv);
void error_exit();
void check_memory(char *string);

int main(int argc, char **argv) {
  cat_flags input_flags = {0};

  parser(argc, argv, &input_flags);
  if (!input_flags.error && !no_file_error(argv)) cat(argv, &input_flags);
}

void cat(char **argv, cat_flags *input_flags) {
  while (argv[optind] != NULL) {
    FILE *file = fopen(argv[optind++], "r");
    if (file != NULL) {
      cat_print(file, input_flags);
    } else {
      perror("No such file or directory.\n");
    }
    if (file != NULL) fclose(file);
  }
}

void cat_print(FILE *file, cat_flags *input_flags) {
  int count = 0;          // Счётчик символов в строке
  int str_num_count = 1;  // Сщётчик строк
  int new_line_flag = 1;  // Флаг: был переход на новую строку
  char str[BUFFER_SIZE] = {0};

  while ((str[count] = fgetc(file)) != EOF) {
    if (new_line_flag) {
      if ((*input_flags).s == 1 &&
          ((count == 1 && str[0] == '\n' && str[count] == '\n') ||
           (str[count] == '\n' && str[count - 1] == '\n' &&
            str[count - 2] == '\n'))) {
        continue;
      }
      n_flag(input_flags, &str_num_count);
      b_flag(input_flags, &str_num_count, str[count], new_line_flag);
      new_line_flag = 0;
    }

    if (str[count] == '\n' || str[count] == EOF) {
      e_flag(input_flags);
      new_line_flag = 1;
    }

    if ((*input_flags).v) {
      v_flag(str[count], input_flags);
    } else {
      if ((*input_flags).t && str[count] == 9)
        t_flag();
      else
        printf("%c", str[count]);
    }
    count++;
  }
}

void n_flag(cat_flags *input_flags, int *str_num_count) {
  if (input_flags->n && !input_flags->b) {
    printf("%6.d\t", (*str_num_count)++);
  }
}

void b_flag(cat_flags *input_flags, int *str_num_count, char str,
            int new_line_flag) {
  if ((*input_flags).b && new_line_flag && (str != '\n')) {
    printf("%6.d\t", (*str_num_count)++);
  }
}

void e_flag(cat_flags *input_flags) {
  if ((*input_flags).e) printf("$");
}

void v_flag(char str, cat_flags *input_flags) {
  if ((str < 32) && (str >= 0) && (str != 10) && (str != 9)) {
    printf("^%c", str + 64);
  } else if (str == 127) {
    printf("^?");
  } else if ((str < 0) && (str < -96)) {
    printf("M-^%c", str + 128 + 64);
  } else if ((str == 9) && ((*input_flags).t == 1)) {  //Флаг t
    t_flag();
  } else {
    printf("%c", str);
  }
}

void t_flag() { printf("^I"); }

void parser(int argc, char **argv, cat_flags *input_flags) {
  flag_checker(argc, argv, input_flags);
  if (!(*input_flags).error) no_file_error(argv);
}

void flag_checker(int argc, char **argv, cat_flags *input_flags) {
  int rezult = 0;
  const struct option long_flags[] = {
      {"number-nonblank", no_argument, &((*input_flags).b), 1},
      {"number", no_argument, &((*input_flags).n), 1},
      {"squeeze-blank", no_argument, &((*input_flags).s), 1},
  };
  while (((rezult = getopt_long(argc, argv, "+benstvET", long_flags, NULL)) !=
          -1) &&
         !(*input_flags).error) {
    switch (rezult) {
      case 'b':
        input_flags->b = 1;
        break;
      case 'e':
        input_flags->e = 1;
        input_flags->v = 1;
        break;
      case 'E':
        input_flags->e = 1;
        break;
      case 'n':
        input_flags->n = 1;
        break;
      case 's':
        input_flags->s = 1;
        break;
      case 'v':
        input_flags->v = 1;
        break;
      case 't':
        input_flags->t = 1;
        input_flags->v = 1;
        break;
      case 'T':
        input_flags->t = 1;
        break;
      case '?':
        error_exit();
        (*input_flags).error = 1;
        break;
    }
  }
}

int no_file_error(char **argv) {
  int res = 0;
  if (argv[optind] == NULL) {
    error_exit();
    res = 1;
  }
  return res;
}

void error_exit() { perror("usage: cat [-benstuv] [file ...]"); }
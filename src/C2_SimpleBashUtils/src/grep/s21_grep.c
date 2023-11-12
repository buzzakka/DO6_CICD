#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 2056
#define ARRAY_SIZE(arr) sizeof((arr)) / sizeof((arr)[0])

typedef struct {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
  int syntax_error;
  int f_flag_error;
} grep_flags;

/*
                -- Парсер --
  parser: Проверка на флаги + составление шаблона.
  flag_checker: Проверка флагов в argv.
  re_pattern: Обновление шаблона.
*/
void parser(int argc, char **argv, char *pattern, grep_flags *input_flags);
void flag_checker(int argc, char **argv, grep_flags *input_flags,
                  char *pattern);
void re_pattern(char *full_pattern, char *new_pattern, int e_mode);

/*
                -- Grep --
  grep_make: отркывает файлы из argv и вызывает grep_print.
  grep_print: Общая функция вывода на экран строк из файла.
  grep_regex_compiller: Компиляция исходных строк для regexec.
  grep_print_file_name: Вывод имени файла.
  grep_print_from_file: Вывод строк из файла.
  c_flag_print, l_flag_print: Реализация флагов -c , -l.
*/
void grep_make(int argc, char **argv, char *pattern, grep_flags input_flags);
void grep_print(char *pattern, FILE *file, char *file_name,
                grep_flags input_flags, int one_file_mode);
void grep_regex_compiller(grep_flags input_flags, regex_t *re, char *pattern);
void grep_print_file_name(int one_file_mode, char *file_name,
                          grep_flags input_flags);
void grep_print_from_file(grep_flags input_flags, char *buffer,
                          int one_file_mode, char *file_name, int *c_count,
                          int n_count, regex_t *re);
void c_flag_print(grep_flags input_flags, int one_file_mode, char *file_name,
                  int c_count);
void l_flag_print(grep_flags input_flags, char *file_name, int c_count);
void f_flag(char *pattern, char *optarg, grep_flags *input_flags);
void o_flag(char *buffer, regex_t *re);

/*
                -- Чекеры --
  check_norm_argv_checker: Проверка argv на корректность.
  check_one_file_mode: Проверка на один файл в argv.
  check_memory: Проверка выделенной памяти.
*/
int check_normal_flag(int argc, char **argv, grep_flags *input_flags);
int check_one_file_mode(char **argv, grep_flags input_flags);

void syntax_error();
void no_file_error();

int main(int argc, char **argv) {
  grep_flags input_flags = {0};

  char pattern[BUFFER_SIZE] = {0};
  parser(argc, argv, pattern, &input_flags);

  if (check_normal_flag(argc, argv, &input_flags)) {
    grep_make(argc, argv, pattern, input_flags);
  }
}

void grep_make(int argc, char **argv, char *pattern, grep_flags input_flags) {
  int one_file_mode = check_one_file_mode(argv, input_flags);
  if (input_flags.e != 1 && input_flags.f != 1) {
    optind++;
  }
  while (optind < argc) {
    FILE *file = fopen(argv[optind], "r");
    if (file == NULL) {
      if (input_flags.s != 1) {
        perror("No such file or directory.\n");
      }
    } else {
      grep_print(pattern, file, argv[optind], input_flags, one_file_mode);
    }
    optind++;
    if (file != NULL) fclose(file);
  }
}

void grep_print(char *pattern, FILE *file, char *file_name,
                grep_flags input_flags, int one_file_mode) {
  regex_t re;
  char buffer[BUFFER_SIZE];

  int c_count = 0;
  int n_count = 0;

  grep_regex_compiller(input_flags, &re, pattern);

  while (fgets(buffer, BUFFER_SIZE, file) != NULL) {
    n_count++;

    char *s = buffer;
    regmatch_t pmatch[1];

    if (input_flags.o && !(input_flags).v &&
        (!regexec(&re, s, ARRAY_SIZE(pmatch), pmatch, 0))) {
      grep_print_from_file(input_flags, buffer, one_file_mode, file_name,
                           &c_count, n_count, &re);
    } else if (regexec(&re, buffer, 0, NULL, 0) == 0 && !(input_flags).v &&
               strcmp(buffer, "\n")) {
      grep_print_from_file(input_flags, buffer, one_file_mode, file_name,
                           &c_count, n_count, &re);
    } else if (regexec(&re, buffer, 0, NULL, 0) != 0 && input_flags.v) {
      grep_print_from_file(input_flags, buffer, one_file_mode, file_name,
                           &c_count, n_count, &re);
    }
  }
  c_flag_print(input_flags, one_file_mode, file_name, c_count);
  l_flag_print(input_flags, file_name, c_count);

  regfree(&re);
}

void grep_print_from_file(grep_flags input_flags, char *buffer,
                          int one_file_mode, char *file_name, int *c_count,
                          int n_count, regex_t *re) {
  (*c_count)++;
  if (!input_flags.c && !input_flags.l) {
    grep_print_file_name(one_file_mode, file_name, input_flags);
    if (input_flags.o && !input_flags.v) {
      if (input_flags.n) {
        printf("%d:", n_count);
      }
      o_flag(buffer, re);
    } else {
      if (input_flags.n) {
        printf("%d:", n_count);
      }
      if (input_flags.o && !input_flags.v) {
        o_flag(buffer, re);
      } else {
        fputs(buffer, stdout);
      }
      if (buffer[strlen(buffer)] == '\0' && buffer[strlen(buffer) - 1] != '\n')
        printf("\n");
    }
  }
}

void grep_print_file_name(int one_file_mode, char *file_name,
                          grep_flags input_flags) {
  if (one_file_mode && !input_flags.h) {
    printf("%s:", file_name);
  }
}

void f_flag(char *pattern, char *optarg, grep_flags *input_flags) {
  char buffer[BUFFER_SIZE];
  FILE *regex_file = fopen(optarg, "r");
  if (regex_file != NULL) {
    while (fgets(buffer, BUFFER_SIZE, regex_file) != NULL) {
      if (buffer[0] == '\n') {
        // re_pattern(pattern, "^\n$", 1);
        continue;
      }
      if (buffer[strlen(buffer) - 1] == '\n') buffer[strlen(buffer) - 1] = '\0';
      re_pattern(pattern, buffer, 1);
    }
  } else {
    (*input_flags).f_flag_error = 1;
  }
  if (regex_file != NULL) {
    fclose(regex_file);
  }
}

void grep_regex_compiller(grep_flags input_flags, regex_t *re, char *pattern) {
  int t;
  if (input_flags.i) {
    t = regcomp(re, pattern, REG_ICASE | REG_EXTENDED | REG_NEWLINE);
  } else {
    t = regcomp(re, pattern, REG_EXTENDED);
  }

  if (t != 0) {
    perror("Compilation error.\n");
    exit(1);
  }
}

void o_flag(char *buffer, regex_t *re) {
  char *s = buffer;
  regmatch_t pmatch[1];
  regoff_t len;

  for (int i = 0;; i++) {
    if (!regexec(re, s, ARRAY_SIZE(pmatch), pmatch, 0)) {
      len = pmatch[0].rm_eo - pmatch[0].rm_so;
      printf("%.*s\n", (int)len, s + pmatch[0].rm_so);
      s += pmatch[0].rm_eo;
    } else {
      break;
    }
  }
}

void c_flag_print(grep_flags input_flags, int one_file_mode, char *file_name,
                  int c_count) {
  if (input_flags.c) {
    grep_print_file_name(one_file_mode, file_name, input_flags);
    if (!input_flags.l) {
      printf("%d\n", c_count);
    } else if (input_flags.l && c_count > 1) {
      printf("%d\n", 1);
    } else {
      printf("%d\n", c_count);
    }
  }
}

void l_flag_print(grep_flags input_flags, char *file_name, int c_count) {
  if (input_flags.l && c_count) {
    printf("%s\n", file_name);
  }
}

int check_one_file_mode(char **argv, grep_flags input_flags) {
  int res;

  if (input_flags.e || input_flags.f) {
    res = (argv[optind + 1] == NULL) ? 0 : 1;
  } else {
    res = (argv[optind + 2] == NULL) ? 0 : 1;
  }
  return res;
}

void parser(int argc, char **argv, char *pattern, grep_flags *input_flags) {
  flag_checker(argc, argv, input_flags, pattern);

  if (!(*input_flags).e && !(*input_flags).f) {
    re_pattern(pattern, argv[optind], 0);
  } else {
    pattern[strlen(pattern) - 1] = '\0';
  }
}

void flag_checker(int argc, char **argv, grep_flags *input_flags,
                  char *pattern) {
  int rez = 0;

  while (!(*input_flags).syntax_error && !(*input_flags).f_flag_error &&
         ((rez = getopt_long(argc, argv, "e:f:ivclnhso", NULL, NULL)) != -1)) {
    switch (rez) {
      case 'e':
        input_flags->e = 1;
        re_pattern(pattern, optarg, 1);
        break;
      case 'i':
        input_flags->i = 1;
        break;
      case 'v':
        input_flags->v = 1;
        break;
      case 'c':
        input_flags->c = 1;
        break;
      case 'l':
        input_flags->l = 1;
        break;
      case 'n':
        input_flags->n = 1;
        break;
      case 'h':
        input_flags->h = 1;
        break;
      case 's':
        input_flags->s = 1;
        break;
      case 'f':
        input_flags->f = 1;
        f_flag(pattern, optarg, input_flags);
        break;
      case 'o':
        input_flags->o = 1;
        break;
      case '?':
        input_flags->syntax_error = 1;
        break;
    }
  }
}

int check_normal_flag(int argc, char **argv, grep_flags *input_flags) {
  int res = 1;

  if ((*input_flags).f_flag_error) {
    no_file_error();
    res = 0;
  } else if (!argv[optind] || (optind == 1 && argc == 2)) {
    syntax_error();
    res = 0;
  }
  return res;
}

void re_pattern(char *full_pattern, char *new_pattern, int e_mode) {
  if (new_pattern) {
    strcat(full_pattern, new_pattern);
    if (e_mode) {
      full_pattern = strcat(full_pattern, "|");
    }
  }
}

void syntax_error() {
  perror("usage: grep [-ivclnh] [-e pattern] [-f file] [pattern] [file ...]");
}

void no_file_error() { perror("No such file or directory."); }
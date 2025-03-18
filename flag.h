#ifndef FLAG_H
#define FLAG_H

enum FlagType {
  INT,
  LONG,
  STRING,
  BOOL,
};

typedef struct Flag {
  enum FlagType type;
  void *ptr;
  int len;
  char *name_long;
  char *name_short;
  void *default_value;
  char *description;
} Flag;

#define FLAGS_MAX 64

typedef struct Flags {
  Flag *flags[FLAGS_MAX];
} Flags;

void flag_append(Flags *flags, Flag *flag);
void flag_parse(Flags *flags, int argc, char *argv[]);
void flag_print(Flags *flags);

#define flag_int(int_ptr, name_long, name_short, default_value, description) \
  { INT, int_ptr, 0, name_long, name_short, (void*)default_value, description }

#define flag_long(long_ptr, name_long, name_short, default_value, description) \
  { LONG, long_ptr, 0, name_long, name_short, (void*)default_value, description }

#define flag_string(char_ptr, len, name_long, name_short, default_value, description) \
  { STRING, char_ptr, len, name_long, name_short, (void*)default_value, description }

#define flag_bool(bool_ptr, name_long, name_short, description) \
  { BOOL, bool_ptr, 0, name_long, name_short, (void*)false, description }

#ifdef FLAG_IMPL

#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void flag_append(Flags *flags, Flag *flag) {
  for (int i = 0; i < FLAGS_MAX; i++) {
    if (flags->flags[i] == NULL) {
      flags->flags[i] = flag;
      return;
    }
  }
  fprintf(stderr, "too many flags\n");
  exit(1);
}

void flag_parse(Flags *flags, int argc, char *argv[]) {
  for (int j = 0; j < FLAGS_MAX; j++) {
    Flag *flag = flags->flags[j];
    if (flag == NULL) {
      break;
    }
    if (flag->default_value != NULL) {
      switch (flag->type) {
        case INT: {
          int *ptr = (int*)flag->ptr;
          long default_value = (long)flag->default_value;
          if (default_value < INT_MIN || default_value > INT_MAX) {
            fprintf(stderr, "integer out of range: %ld\n", default_value);
            exit(1);
          }
          *ptr = default_value;
        } break;
        case LONG: {
          long *ptr = (long*)flag->ptr;
          long default_value = (long)flag->default_value;
          *ptr = default_value;
        } break;
        case STRING: {
          char *ptr = (char*)flag->ptr;
          const char *default_value = (const char*)flag->default_value;
          size_t copied = strlcpy(ptr, default_value, flag->len);
          if (copied >= flag->len) {
            fprintf(stderr, "default_value: %s: too long\n", default_value);
            exit(1);
          }
        } break;
        case BOOL: {
          bool *ptr = (bool*)flag->ptr;
          bool default_value = (bool)flag->default_value;
          *ptr = default_value;
        } break;
        default:
          fprintf(stderr, "unhandled flag type: %d\n", flag->type);
          exit(1);
      }
    }
  }
  
  for (int i = 1; i < argc; i++) {
    char* arg = argv[i];
    for (int j = 0; j < FLAGS_MAX; j++) {
      Flag *flag = flags->flags[j];
      if (flag == NULL) {
        break;
      }
      if (strcmp(arg, flag->name_long) == 0 || strcmp(arg, flag->name_short) == 0) {
        switch (flag->type) {
          case INT:
            if((i + 1) < argc) {
              i++;
              char *val = argv[i];
              char *endptr = NULL;
              long l = strtol(val, &endptr, 10);
              if (errno == EINVAL || !(strcmp(endptr, "") == 0)) {
                fprintf(stderr, "%s: invalid argument\n", val);
                exit(1);
              }
              if (errno == ERANGE || l < INT_MIN || l > INT_MAX) {
                fprintf(stderr, "integer out of range: %s\n", val);
                exit(1);
              }
              int *ptr = (int*)flag->ptr;
              *ptr = (int)l;
            } else {
              fprintf(stderr, "missing value after flag: %s\n", arg);
              exit(1);
            }
            break;
          case LONG:
            if((i + 1) < argc) {
              i++;
              char *val = argv[i];
              char *endptr = NULL;
              long long ll = strtoll(val, &endptr, 10);
              if (errno == EINVAL || !(strcmp(endptr, "") == 0)) {
                fprintf(stderr, "%s: invalid argument\n", val);
                exit(1);
              }
              if (errno == ERANGE || ll < LONG_MIN || ll > LONG_MAX) {
                fprintf(stderr, "long out of range: %s\n", val);
                exit(1);
              }
              long *ptr = (long*)flag->ptr;
              *ptr = (long)ll;
            } else {
              fprintf(stderr, "missing value after flag: %s\n", arg);
              exit(1);
            }
            break;
          case STRING:
            if((i + 1) < argc) {
              i++;
              char *val = argv[i];
              size_t copied = strlcpy(flag->ptr, val, flag->len);
              if (copied >= flag->len) {
                fprintf(stderr, "flag %s: value %s: too long\n", arg, val);
                exit(1);
              }
            } else {
              fprintf(stderr, "missing value after flag: %s\n", arg);
              exit(1);
            }
            break;
          case BOOL: {
            bool *ptr = (bool*)flag->ptr;
            *ptr = true;
          } break;
          default:
            fprintf(stderr, "unhandled flag type: %d\n", flag->type);
            exit(1);
        }
        goto next_arg;
      }
    }
    // flags exchausted
    fprintf(stderr, "unknown flag %s\n", arg);
    exit(1);
next_arg:
    continue;
  }
}

void flag_print(Flags *flags) {
  for (int i = 0; i < FLAGS_MAX; i++) {
    Flag *flag = flags->flags[i];
    if (flag == NULL) {
      break;
    }
    printf("%s, %s\t%s", flag->name_long, flag->name_short, flag->description);
    if (flag->default_value != NULL) {
      printf(" (default: ");
      switch(flag->type) {
        case INT:
        case LONG:
          printf("%ld", (long)flag->default_value);
          break;
        case STRING:    
          printf("%s", (char*)flag->default_value);
          break;
        default:
          fprintf(stderr, "unhandled flag type: %d\n", flag->type);
          exit(1);
      }
      printf(")");
    }
    if (flag -> type == BOOL) {
      printf(" (default: false)");
    } else if (flag->type == STRING) {
      printf(" (max length: %d)", flag->len);
    }
    printf("\n");
  }
}

#endif // FLAG_IMPL
#endif // FLAG_H

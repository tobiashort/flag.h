#define FLAG_IMPL
#include "flag.h"

int main(int argc, char *argv[]) {
  long l = {0};
  long l2 = {0};
  #define slen 12
  char s[slen] = {0};
  #define s2len 22
  char s2[s2len] = {0};

  Flag l_flag = flag_long(&l, "--long", "-l", NULL, "long flag");
  Flag l2_flag = flag_long(&l2, "--long2", "-l", 42, "long2 flag");
  Flag s_flag = flag_string(s, slen, "--string", "-s", NULL, "string flag");
  Flag s2_flag = flag_string(s2, s2len, "--string2", "-s2", "Foo Bar Baz", "string2 flag");

  Flags flags = {0};
  flag_append(&flags, &l_flag);
  flag_append(&flags, &l2_flag);
  flag_append(&flags, &s_flag);
  flag_append(&flags, &s2_flag);

  flag_parse(&flags, argc, argv);

  printf("long flag: %ld\n", l);
  printf("long2 flag: %ld\n", l2);
  printf("string flag: %s\n", s);
  printf("string2 flag: %s\n", s2);

  flag_print(&flags);

  printf("Done\n");
  return 0;
}

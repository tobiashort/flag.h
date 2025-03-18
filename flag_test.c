#define FLAG_IMPL
#include "flag.h"

int main(int argc, char *argv[]) {
  int i = {0};
  int i2 = {0};
  long l = {0};
  long l2 = {0};
  #define slen 12
  char s[slen] = {0};
  #define s2len 22
  char s2[s2len] = {0};
  bool b = {0};

  Flag i_flag = flag_int(&i, "--int", "-i", NULL, "int flag");
  Flag i2_flag = flag_int(&i2, "--int", "-i", 3, "int2 flag");
  Flag l_flag = flag_long(&l, "--long", "-l", NULL, "long flag");
  Flag l2_flag = flag_long(&l2, "--long2", "-l", 42, "long2 flag");
  Flag s_flag = flag_string(s, slen, "--string", "-s", NULL, "string flag");
  Flag s2_flag = flag_string(s2, s2len, "--string2", "-s2", "Foo Bar Baz", "string2 flag");
  Flag b_flag = flag_bool(&b, "--bool", "-b", "bool flag");

  Flags flags = {0};
  flag_append(&flags, &i_flag);
  flag_append(&flags, &i2_flag);
  flag_append(&flags, &l_flag);
  flag_append(&flags, &l2_flag);
  flag_append(&flags, &s_flag);
  flag_append(&flags, &s2_flag);
  flag_append(&flags, &b_flag);

  flag_parse(&flags, argc, argv);

  printf("int flag: %d\n", i);
  printf("int2 flag: %d\n", i2);
  printf("long flag: %ld\n", l);
  printf("long2 flag: %ld\n", l2);
  printf("string flag: %s\n", s);
  printf("string2 flag: %s\n", s2);
  printf("bool flag: %s\n", b ? "true" : "false");

  flag_print(&flags);

  printf("Done\n");
  return 0;
}

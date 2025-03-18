#define BUILD_IMPL
#include "build.h/build.h"

#define check_status(s) \
  if (s != 0) {         \
    return s;           \
  }

int main() {
  int status = build_yourself();
  check_status(status);

  Cmd test_build = {0};
  cmd_append(&test_build, "cc", NULL);
  cmd_append(&test_build, "-Wall", "-Werror", NULL);
  cmd_append(&test_build, "-o", "flag_test", NULL);
  cmd_append(&test_build, "-g", NULL);
  cmd_append(&test_build, "flag_test.c", NULL);
  status = cmd_run(&test_build);
  check_status(status);

  Cmd test_run = {0};
  cmd_append(&test_run, "./flag_test", NULL);
  cmd_append(&test_run, "-l", "66", NULL);
  cmd_append(&test_run, "--string", "Hello World", NULL);
  status = cmd_run(&test_run);
  check_status(status);

  return 0;
}

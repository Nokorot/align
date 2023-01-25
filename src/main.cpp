#include "main.h"

#include <iostream>
#include <algorithm>

#include "str_trim.hpp"

#define FLAG_IMPLEMENTATION
#include "flag.h"

#include "align.h"

using namespace std;

void usage(FILE *sink, const char *program)
{
    fprintf(sink, "Usage: %s [OPTIONS] [--] <regex-keys...>\n", program);
    fprintf(sink, "OPTIONS:\n");
    flag_print_options(sink);
}

int main(int argc, char **argv)
{

  char *program = *argv;

  bool *help        = flag_bool("help", false, "Print this help to stdout and exit with 0");
  bool *after       = flag_bool("a", false, "Align after, in stead of before the key");
  bool *last        = flag_bool("l", false, "Match the last ocurrense of the key, in stead of the first");
  bool *ignore_case = flag_bool("i", false, "Ignore case");
  uint64_t *clmn    = flag_uint64("clmn", DEFAULT_CLMN, \
          "Sets the 'column width'. That is the key is aligned at a multiple of this value.");

  // TODO: Check args for ',' before parsing. 
  //  Then parse only the part before it. Use this to apply, -a, -l, -i to each key.

  if (!flag_parse(&argc, argv)) { usage(stderr, program); flag_print_error(stderr); exit(1); }
  if (*help) { usage(stdout, program); exit(0); }

  options op;
  op.prgname     = program;
  op.clmn_width  = *clmn;
  op.after       = *after;
  op.last        = *last;
  op.ignore_case = *ignore_case;
  // argv = flag_rest_argv();

  if (!*argv) {
    fprintf(stderr, "ERROR: Not enough arguments\n\n");
    usage(stderr, program);
    exit(1);
  }

  align(op, stdout, stdin, argv);
}

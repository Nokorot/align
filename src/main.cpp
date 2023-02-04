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
  bool *last        = flag_bool("l", false, "Match the last ocurrense of the key, in stead of the first");
  uint64_t *clmn    = flag_uint64("clmn", DEFAULT_CLMN, \
          "Sets the 'column width'. That is the key is aligned at a multiple of this value.");

  bool *after       = flag_bool("a", false, "Align after, in stead of before the key");
  bool *ignore_case = flag_bool("i", false, "Ignore case");
  bool *es          = flag_bool("x", false, "Do _not_ ignore escape sequences");

  // TODO: Check args for ',' before parsing.
  //  Then parse only the part before it. Use this to apply, -a, -l, -i to each key.
  //
  //      align ':' a, ':second:' ix

  if (!flag_parse(&argc, argv)) { usage(stderr, program); flag_print_error(stderr); exit(1); }
  if (*help) { usage(stdout, program); exit(0); }

  options op;
  op.prgname     = program;
  op.clmn_width  = *clmn;
  op.after       = *after;
  op.last        = *last;
  op.ignore_case = *ignore_case;
  op.ignore_escape_sequenses = !*es;
  argv = flag_rest_argv();

  if (argc < 0) {
    fprintf(stderr, "ERROR: Not enough arguments\n\n");
    usage(stderr, program);
    exit(1);
  }

  align(op, stdout, stdin, argv);
}

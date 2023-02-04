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
    fprintf(sink, "Usage: %s [OPTIONS] [--] +[key-flags] <key-string> ... \n\n", program);
    fprintf(sink, "Descrtiption:\n");
    fprintf(sink, """\
    For each line the program, searches for the key-string, a 'regex expresion'. Then the maximum index of the match, for each line is computed. The program then outputs the lines, with space caracters added before or after the matched index (depending on the after key-flag). \n\
    $ align ':' \n\
    Will insert spaces to each line before the first colon (ie. :) at each line, such in the output the first colon on each line are aligned.\n\
    In comprason\n\
    $ align +a ':' \n\
    will insert spaces after the first colon on each line, so that the first non-space character on each line following this colon will be aligned.\n\
    \n\
    There is also a key-flag 'l', indicating to align at the last match on each line. \n\
    The flag 'i' means ignore case. That is \n\
    $ align +li HERE \n\
    will align at the last match with the sequence of characters 'here' \n\
    By default ascci escape sequences are considerd to have width 0. The key-flag '+x' desables-this behaivior. \n\n""");
    fprintf(sink, "The key-flags: \n");
    fprintf(sink, "OPTIONS:\n");
    flag_print_options(sink);
}

int main(int argc, char **argv)
{

  char *program = *argv;

  bool *help        = flag_bool("help", false, "Print this help to stdout and exit with 0");
  uint64_t *clmn    = flag_uint64("clmn", DEFAULT_CLMN, \
          "Sets the 'column width'. That is the key is aligned at a multiple of this value.");

  if (!flag_parse(&argc, argv)) {
      // usage(stderr, program);
      flag_print_error(stderr);
      fprintf(stderr, " use -help for more info\n");

      exit(1); }
  if (*help) { usage(stdout, program); exit(0); }

  options op;
  op.prgname     = program;
  op.clmn_width  = *clmn;
  argv = flag_rest_argv();

  if (argc < 0) {
    fprintf(stderr, "ERROR: Not enough arguments\n\n");
    usage(stderr, program);
    exit(1);
  }

  vector<key> keys;

  for (char **arg = argv; *arg; ++arg) {
    key k = {0};
    if (**arg == '+') {
        char *flags = *(arg++);
        if (!*arg) {
            fprintf(stderr, "ERROR: Argument missing, you have provided a key-flags '%s', but it is not followed by a string-key.!\n", flags);
            exit(1);
        }
        k.str = *arg;
        for (char *ch=flags+1; *ch; ++ch) {
            switch (*ch){
            case 'a': k.after = true; break;
            case 'l': k.last = true; break;
            case 'A': k.match_all = true; break;
            case 'i': k.ignore_case = true; break;
            case 'x': k.ignore_escape_sequenses = true; break;
            default:
                fprintf(stderr, "ERROR: Unknown flag key '%c'!\n", *ch);
                exit(1);
            }
        }
    } else
        k.str = *arg;

    // fprintf(stdout, "(%s, a: %b, l: %b, i: %b, x: %b)\n", k.str, k.after, k.last, k.ignore_case, k.ignore_escape_sequenses);
    keys.push_back(k);
  }

  align(op, stdout, stdin, keys);
}

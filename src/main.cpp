#include "main.h"

#include <iostream>
#include <algorithm>

#include "str_trim.hpp"

#define FLAG_IMPLEMENTATION
#include "flag.h"

using namespace std;

void usage(FILE *sink, const char *program)
{
    fprintf(sink, "Usage: %s [OPTIONS] [--] <regex-keys...>\n", program);
    fprintf(sink, "OPTIONS:\n");
    flag_print_options(sink);
}

regex mk_regex_after(options &o, char **args) {
  // Construction the regex
  char rx_str[512];
  int rx_len = 0;
  rx_len = sprintf(rx_str, "(.*%s", *args);
  while (*(++args)) {
    rx_len += sprintf(rx_str + rx_len, "|.*%s", *args);
  }
  rx_len += sprintf(rx_str + rx_len, ")[ ]*([^ ].*)");
  return regex(rx_str);
}

regex mk_regex(options &o, char **args) {
  // Construction the regex
  char rx_str[512];
  int rx_len = 0;
  rx_len = sprintf(rx_str, "(.*)[ ]*(%s.*", *args);
  while (*(++args)) {
    rx_len += sprintf(rx_str + rx_len, "|%s.*", *args);
  }
  rx_len += sprintf(rx_str + rx_len, ")");
  return regex(rx_str);
}

int find_key(options &o, regex rx, vector<string> &head, vector<string> &tail) {
  // Searching for the regex in each string
  string line, tmp;
  smatch m;
  int mx = 0;
  while (getline(cin, line)) {
    if (regex_search (line,m,rx)) {
        tmp = string(m[2]); rtrim(tmp);
        head.push_back(m[1]);
        tail.push_back(tmp);

        if (m[1].length() > mx)
            mx = m[1].length();
    } else {
        rtrim(line); // Removing trailing newline character
        head.push_back(line);
        tail.push_back("");
    }
  }
  return ((mx + o.clmn_width) / o.clmn_width) * o.clmn_width;
}

void align(options &o, char **args) {
  vector<string> head, tail;

  regex rx;
  if (o.after) {
    rx = mk_regex_after(o, args);
  } else {
    rx = mk_regex(o, args);
  }
  int mx = find_key(o, rx, head, tail);

  // Printing the lines with alignment
  auto a = head.begin(), b = tail.begin();
  for (; a != head.end() && b != tail.end(); ++a, ++b) {
    cout << *a;
    if (b->length() > 0) 
        for (int i=a->length(); i<mx; ++i) 
            cout << " ";
    cout << *b << "\n";
  }
}

int main(int argc, char **argv)
{
  char *program = *argv;

  bool *help = flag_bool("help", false, "Print this help to stdout and exit with 0");
  bool *after = flag_bool("a", false, "Align after, in stead of before the key");

  uint64_t *clmn_width = flag_uint64("clmn", 8, "Sets the 'column width'. That is the key is aligned at a multiple of this value.");

  if (!flag_parse(argc, argv)) {
     usage(stderr, program);
     flag_print_error(stderr);
     exit(1);
  }

  if (*help) {
     usage(stdout, program);
     exit(0);
  }

  options op;
  op.prgname = program;
  op.clmn_width = *clmn_width;
  op.after = *after;
  argv = flag_rest_argv();

  if (!*argv) {
    fprintf(stderr, "ERROR: Not enough arguments\n\n");
    usage(stderr, program);
    exit(1);
  }

  align(op, argv);
}

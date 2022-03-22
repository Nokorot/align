#ifndef MAIN_H__
#define MAIN_H__

#include <string>
#include <regex>
#include <vector>


// TODO: Evoronment Var
#define DEFAULT_CLMN 4

struct options {
  std::string prgname;
  int clmn_width;       // align at a multipule of clmn_width
  bool after;           // align after the match
  bool last;            // match the last case
  bool ignore_case;     // match the last case
};

int main(int argc, char **argv);

#endif

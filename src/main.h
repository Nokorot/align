#ifndef MAIN_H__
#define MAIN_H__

#include <string>
#include <regex>
#include <vector>


// TODO: Evoronment Var
#define DEFAULT_CLMN 4

struct options {
  std::string prgname;
  int clmn_width;                   // Align at a multipule of clmn_width
};

int main(int argc, char **argv);

#endif

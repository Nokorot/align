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
  bool after;                       // Align after the match
  bool last;                        // Match the last case
  bool ignore_case;                 // When matching the key ignore case
  bool ignore_escape_sequenses = 1; // When aligning the columns.
                                    //    Consider esacpecodes to be of width 0.
};

int main(int argc, char **argv);

#endif

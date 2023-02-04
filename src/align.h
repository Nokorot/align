#ifndef ALIGN_H__
#define ALIGN_H__

#include <string>
#include <regex>
#include <vector>

#include "main.h"

struct key {
  char *str;
  bool match_all;                   // TODO: Align at all matches
  bool after;                       // Align after the match
  bool last;                        // Match the last case
  bool ignore_case;                 // When matching the key ignore case
  bool ignore_escape_sequenses = 1; // When aligning the columns
                                    //    Consider esacpecodes to be of width 0
  std::regex rx;
};

void align(options &o, FILE *sink, FILE *inpt, std::vector<key> keys);


#endif

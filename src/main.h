#ifndef MAIN_H__
#define MAIN_H__

#include <string>
#include <regex>
#include <vector>

struct options {
  std::string prgname;
  int clmn_width;
  bool after;
};

void align(options &o, int argc, char **args);
int main(int argc, char **argv);
int find_key(options &o, std::regex rx, 
        std::vector<std::string> &head, std::vector<std::string> &tail);

#endif

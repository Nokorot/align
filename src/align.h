#ifndef ALIGN_H__
#define ALIGN_H__

#include <string>
#include <regex>
#include <vector>

#include "main.h"

void align(options &o, FILE *sink, FILE *inpt, char **args);
int split_lines(options &o, FILE *inpt, char *key, \
        std::vector<std::string> &head, std::vector<std::string> &tail);

#endif

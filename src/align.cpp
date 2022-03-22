#include "align.h"

#include <iostream>
#include <algorithm>
#include <list>

#include "str_trim.hpp"

using namespace std;


string string_replace(string &str, const string a, const string &b) {
  stringstream ss;

  size_t prev = 0, pos = str.find(a, 0);
  while(pos != string::npos) {
      ss << str.substr(prev, pos);
      ss << b; 
      prev = pos + a.length();
      pos = str.find(a,pos+1);
  }
  ss << str.substr(prev);
  return ss.str();
}

regex mk_regex(options &o, char *key) {
  // Construction the regex
  char rx_str[512];
  int rx_len = 0;

  string s(key);

  s = string_replace(s, "(", "\\(");
  s = string_replace(s, ")", "\\)");
  rx_len = sprintf(rx_str, "(%s)", s.c_str());

  if (o.ignore_case)
    return regex(rx_str, std::regex_constants::icase);
  return regex(rx_str);
}

int match_key(options &o, regex &rx, string line) {
  smatch m;

  auto words_begin = sregex_iterator(line.begin(), line.end(), rx);
  auto words_end = sregex_iterator();

  if (words_begin == words_end) 
      return -1;

  if (o.last) {
    m = *words_begin;
    for (auto i = words_begin; ++i != words_end;)
        m = *i;
  } else {
    m = *words_begin;
  }

  if (o.after) {
    return m[0].second - line.begin(); 
  } else {
    return m[0].first - line.begin();
  }
}

int split_lines(options &o, FILE *inpt, char *key, vector<string> &head, vector<string> &tail) {
  regex rx = mk_regex(o, key);

  string line, tmp;
  char *lineptr; size_t len=0;
  int idx, mx = 0;
  while (getline(&lineptr, &len, inpt) > 0) {
    line = string(lineptr);
    if (0 < (idx = match_key(o, rx, line))) {
        if (o.after) {
            tmp = line.substr(idx); trim(tmp);
            tail.push_back(tmp);
            head.push_back(line.substr(0, idx));
        } else {
            tmp = line.substr(0, idx); rtrim(tmp);
            head.push_back(tmp);
            tmp = line.substr(idx); rtrim(tmp);
            tail.push_back(tmp);
        }

        mx = max(mx, (int) head.back().length());
        // if (head.back().length() > mx)
        //     mx = head.back().length();
    } else {
        rtrim(line); // Removing trailing newline character
        head.push_back(line);
        tail.push_back("");
    }
  }

  return ((mx + o.clmn_width) / o.clmn_width) * o.clmn_width;
}

void align(options &o, FILE *sink, FILE *inpt, char **args) {
  vector<string> head, tail;

  int mx = split_lines(o, inpt, *args, head, tail);

  char *bp;
  size_t size;
  FILE *stream = open_memstream(&bp, &size);

  // Printing the lines with alignment
  auto a = head.begin(), b = tail.begin();
  for (; a != head.end() && b != tail.end(); ++a, ++b) {
    fprintf(stream, "%s", (*a).c_str());
    if (b->length() > 0) 
        for (int i=a->length(); i<mx; ++i) 
            fprintf(stream, " ");
    fprintf(stream, "%s\n", (*b).c_str());
    // Shuld probalby switch to c++ style streams
    // cout << *a;
    // if (b->length() > 0) 
    //     for (int i=a->length(); i<mx; ++i) 
    //         cout << " ";
    // cout << *b << "\n";
  }
  fclose(stream);

    // TODO: This is a cool way of handeling the arguments, but it would be nice if the searche of the second argument started after the first. Which could be done with multiple regex sub matches.
  if (*(++args)) {
    stream = fmemopen(bp, size, "r");
    align(o, sink, stream, args);
    fclose(stream);
  } else {
    fprintf(sink, "%s", bp);
  }
}

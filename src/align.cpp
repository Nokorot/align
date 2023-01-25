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

vector<int> split_lines(options &o, FILE *inpt, char **keys, 
        vector<vector<string>> &rows) {
  vector<regex> rxs;
  for (; *keys; ++keys)
    rxs.push_back(mk_regex(o, *keys));

  vector<int> widths(rxs.size());

  string line, tmp;
  char *lineptr; size_t len=0;
  int idx, mx = 0;

  while (getline(&lineptr, &len, inpt) > 0) {
    line = string(lineptr);
    
    int rowc = 0;
    vector<string> row;
    for (auto rx=rxs.begin(); rx != rxs.end(); ++rx, ++rowc) {
        if( 0 < (idx = match_key(o, *rx, line)) ) {
            // TODO: Test this some more
            if (o.after) {
                row.push_back(line.substr(0, idx)); // Don't know why there is no trimming
                line = line.substr(idx); ltrim(line);
            } else {
                tmp = line.substr(0, idx); rtrim(tmp);
                row.push_back(tmp);
                line = line.substr(idx); rtrim(line); // Note this leaves spaces
            }
            
            // Unicode ??
            widths[row.size()-1] = max(widths[row.size()-1], (int) row.back().length());
            while (row.size() < rowc)
                row.push_back("");
        }
    }

    rtrim(line); // Removing trailing newline character
    row.push_back(line);

    widths[row.size()-1] = max(widths[row.size()-1], (int) row.back().length());

    rows.push_back(row);
  }

  // Align at clmn count
  int charc = 0;
  for (auto w = widths.begin(); w != widths.end(); ++w) {
    *w = ((charc + *w + o.clmn_width) / o.clmn_width) * o.clmn_width - charc;
    charc += *w;
  }

  return widths;
}


void align(options &o, FILE *sink, FILE *inpt, char **args) {
  vector<vector<string>> rows;
  vector<int> widths = split_lines(o, inpt, args, rows);

  char *bp;
  size_t size;
  FILE *stream = open_memstream(&bp, &size);
   
  // Printing the lines with alignment
  auto row = rows.begin();
  for (; row != rows.end(); ++row) {
    // assert(row->lendth() < widths.size);

    auto a = row->begin();
    auto w = widths.begin();
    fprintf(stream, "%s", a->c_str());
    int alen = a->length();
    for (a++; a != row->end(); ++a, ++w) {
        for (int i=alen; i<*w; ++i) 
            fprintf(stream, " ");
        fprintf(stream, "%s", a->c_str());
    }
    fprintf(stream, "\n");
  }
  fclose(stream);
  
  fprintf(sink, "%s", bp);
}

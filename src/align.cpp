#include "align.h"

#include <iostream>
#include <algorithm>
#include <list>

#include "str_trim.hpp"


// #include <wchar.h>
#include <uniwidth.h>

#include <cstdlib>
#include "thirdparty/unicodelib_encodings.h"
#include "thirdparty/unicodelib.h"


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

regex mk_regex(const key key) {
  // Construction the regex
  char rx_str[512];
  int rx_len = 0;

  string s(key.str);

  s = string_replace(s, "(", "\\(");
  s = string_replace(s, ")", "\\)");
  rx_len = sprintf(rx_str, "(%s)", s.c_str());

  if (key.ignore_case)
    return regex(rx_str, std::regex_constants::icase);
  return regex(rx_str);
}




void match_key(const key &key, const string line, vector<int> &matches) {
    auto words_begin = sregex_iterator(line.begin(), line.end(), key.rx);
    auto words_end = sregex_iterator();

    if (words_begin == words_end)
        return;

    auto get_index = [key](smatch m)
    {
        if (key.after) {
          return m.position(0) + m.length(0);
        } else {
          return m.position(0);
        }
    };

    if (key.match_all) {
        for (auto i = words_begin; i != words_end; ++i) {
            matches.push_back(get_index(*i));
        }
    } else if (key.last) {
        smatch last_match = *words_begin;
        for (auto match = words_begin; ++match != words_end;)
            last_match = *match;
        matches.push_back(get_index(last_match));
    } else {
        matches.push_back(get_index(*words_begin));
    }
}

int compute_str_width(string &str, bool ignore_escape_sequenses)
{
    std::u32string out;
    unicode::utf8::decode(str.c_str(), str.length(), out);

    int width = 0;

    for (int k=0; k<out.length(); ++k) {
        // Skip escape sequences
        if (ignore_escape_sequenses && out[k] == '\x1B') {
            if (out[++k] == '[') {
                if (out[++k] == '?') ++k;
                while(unicode::is_number(out[k]) || out[k] == ';') ++k;
            }
            continue;
        }
        ++width;

        // Consider emojis to have width 2.
        if (0x1f600 < out[k] && out[k] < 0x1fffe)
            ++width;
    }
    return width;
}

struct row_entry {
    string str;
    int current_width;
};

// returns the row count
int split_line(string line, const vector<key> &keys, vector<row_entry> &row,
        vector<int> &widths) {
    int i, rowc = 0;

    // TODO: u3string here

    for (auto key=keys.begin(); key != keys.end(); ++key) {
        vector<int> matches;
        match_key(*key, line, matches);
        for (auto index = matches.begin(); index != matches.end(); ++index, ++rowc) {
            row_entry entry;

            entry.str = line.substr(0, *index);
            line = line.substr(*index);

            if (key->after)
                line = ltrim(line, key->ignore_escape_sequenses);
            else
                entry.str = rtrim(entry.str, key->ignore_escape_sequenses);

            entry.current_width = compute_str_width(entry.str, key->ignore_escape_sequenses);
            if (widths.size() <= rowc)
                widths.push_back(entry.current_width);
            else
                widths[rowc] = max(widths[rowc], entry.current_width);

            row.push_back(entry);
            ++rowc;
        }
    }

    if (line.length() > 0) {
        // TODO: This 'ignore_escape_sequenses should probably be the one from the last key.
        row_entry entry;
        entry.str = rtrim(line, true);
        entry.current_width = compute_str_width(entry.str, true);
        if (widths.size() <= rowc)
            widths.push_back(entry.current_width);
        else
            widths[rowc] = max(widths[rowc], entry.current_width);

        row.push_back(entry);
    }

    return rowc+1;
}

// Returns the maximum row widths
vector<int> split_lines(options &o, FILE *inpt, vector<key> keys,
        vector<vector<row_entry>> &rows)
{
  vector<int> widths;

  string line, tmp;
  char *lineptr; size_t len=0;
  int max_row_count = 0;

  for (auto key=keys.begin(); key != keys.end(); ++key)
      (*key).rx = mk_regex(*key);

  while (getline(&lineptr, &len, inpt) > 0) {
      line = string(lineptr);
      vector<row_entry> row;
      int rowc = split_line(line, keys, row, widths);
      max_row_count = max(max_row_count, rowc);

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

void align(options &o, FILE *sink, FILE *inpt, vector<key> keys) {
  vector<vector<row_entry>> rows;
  vector<int> widths = split_lines(o, inpt, keys, rows);

  char *bp;
  size_t size;
  FILE *stream = open_memstream(&bp, &size);

  // Printing the lines with alignment
  auto row = rows.begin();
  for (; row != rows.end(); ++row) {
    auto a = row->begin();
    auto w = widths.begin();

    // int a_width = compute_str_width(*a, );

    fprintf(stream, "%s", a->str.c_str(), a->current_width, *w);
    for (; a + 1 != row->end(); ++w) {
        for (int i=a->current_width; i<*w; ++i)
            fprintf(stream, " ");
        fprintf(stream, "%s", (++a)->str.c_str());
    }
    fprintf(stream, "\n");
  }
  fclose(stream);

  fprintf(sink, "%s", bp);
}

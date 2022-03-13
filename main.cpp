#include <iostream>
#include <algorithm>
#include <string>
#include <regex>

#include <vector>

#include "str_trim.hpp"

using namespace std;

int main(int argc, char **argv)
{
  // TODO: align after, instead of before as it is
  
  //  TODO: column width shuld be a command line option
  int clmn_width = 8;

  if (argc < 2) {
    // TODO: Usage message
    cerr << "Not enough arguments\n";
    exit(1);
  }

  
  // Construction the regex
  char rx_str[512];
  int rx_len = 0;
  rx_len = sprintf(rx_str, "(.*[^ ])[ ]*(%s.*", argv[1]);
  for (int i=2; i<argc; ++i)
      rx_len += sprintf(rx_str + rx_len, "|%s.*", argv[i]);
  rx_len += sprintf(rx_str + rx_len, ")");
  regex rex(rx_str);

  // Searching for the regex in each string
  string line, tmp;
  smatch m;
  vector<string> head, tail;
  int mx = 0;
  while (getline(cin, line)) {
    if (regex_search (line,m,rex)) {
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
  mx = ((mx + clmn_width) / clmn_width) * clmn_width;


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

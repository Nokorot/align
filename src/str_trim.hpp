#include <cctype>
#include <locale>
#include <cstring>

static inline void escapecode(const char **c)
{
     if (**c == '[') {
         ++*c;
         while(std::isdigit(**c) || **c == ';' || **c == '?') {
             ++*c;
         }
     }
     ++*c;
}

static inline void strn_push(char **dst, const char *head, const int len)
{
     std::strncpy(*dst, head, len);
     *dst += len;
}

// trim from start
static inline std::string ltrim(std::string &str, bool ignore_escape_sequenses)
{
    const char *head, *ch;
    char *new_str, *new_str_ch;

    head = ch = str.c_str();
    new_str = new_str_ch = new char[str.length() + 1];

    while (*ch) {
        if (ignore_escape_sequenses && *ch == '\x1B') {
            head = ch;
            escapecode(&(++ch));
            strn_push(&new_str_ch, head, ch-head);
        } else if (std::isspace(*ch))
            ++ch;
        else
            break;
    }

    std::strcpy(new_str_ch, ch);
    return std::string(new_str);
}

// trim from end
static inline std::string rtrim(std::string &str, bool ignore_escape_sequenses) {
    const char *head, *ch, *last_non_space;
    char *new_str, *new_str_ch;

    head = ch = str.c_str();
    new_str = new_str_ch = new char[str.length() + 1];

    last_non_space = ch;
    while (*ch) {
        if (ignore_escape_sequenses && *ch == '\x1B')
            escapecode(&(++ch));
        else if (std::isspace(*ch))
            ++ch;
        else last_non_space = ch++;
    }
    strn_push(&new_str_ch, head, last_non_space-head+1);
    ch = last_non_space+1;
    while (*ch) {
        if (*ch == '\x1B') {
            head = ch;
            escapecode(&(++ch));
            strn_push(&new_str_ch, head, ch-head);
        }
        else if (std::isspace(*ch))
            ++ch;
        else {
            fprintf(stderr, "ERROR: %s!\n", "Unreachable");
            break;
        }
    }

    return std::string(new_str);
    // s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
    //     return !std::isspace(ch);
    // }).base(), s.end());
}

// trim from both ends
static inline std::string trim(std::string &str, bool ignore_escape_sequenses) {
    std::string s = ltrim(str, ignore_escape_sequenses);
    return rtrim(s, ignore_escape_sequenses);
}

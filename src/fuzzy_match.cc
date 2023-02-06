#include "fuzzy_match.hh"

#include <iostream>

#include <algorithm>
#include <ctype.h>
#include <stdio.h>
#include <vector>
#include <string_view>


namespace fuzz{

  namespace{

    enum CharClass { Other, Lower, Upper };
    enum CharRole { None, Tail, Head };



    CharClass getCharClass(int c) {
      if (islower(c))
        return Lower;
      if (isupper(c))
        return Upper;
      return Other;
    }


    void calculateRoles(std::string_view s, int roles[], int *class_set) {
      if (s.empty()) {
        *class_set = 0;
        return;
      }

      CharClass pre = Other, cur = getCharClass(s[0]), suc;
      *class_set = 1 << cur;
      auto fn = [&]() {
        if (cur == Other)
          return None;
          // U(U)L is Head while U(U)U is Tail
        return pre == Other || (cur == Upper && (pre == Lower || suc == Lower))
          ? Head
          : Tail;
      };

      for (size_t i = 0; i < s.size() - 1; i++) {
        suc = getCharClass(s[i + 1]);
        *class_set |= 1 << suc;
        roles[i] = fn();
        pre = cur;
        cur = suc;
      }
      roles[s.size() - 1] = fn();
    }

  }


  int FuzzyMatcher::missScore(int j, bool last) {
    int s = -3;
    if (last)
      s -= 10;
    if (text_role[j] == Head)
      s -= 10;
    return s;
  }

  int FuzzyMatcher::matchScore(int i, int j, bool last) {
    int s = 0;
    // Case matching.
    if (pat[i] == text[j]) {
      s++;
      // pat contains uppercase letters or prefix matching.
      if ((pat_set & 1 << Upper) || i == j)
        s++;
    }
    if (pat_role[i] == Head) {
      if (text_role[j] == Head)
        s += 30;
      else if (text_role[j] == Tail)
        s -= 10;
    }
    // Matching a tail while previous char wasn't matched.
    if (text_role[j] == Tail && i && !last)
      s -= 30;
      // First char of pat matches a tail.
    if (i == 0 && text_role[j] == Tail)
      s -= 40;
    return s;
  }

  FuzzyMatcher::FuzzyMatcher(std::string_view pattern, int sensitivity) {
    calculateRoles(pattern, pat_role, &pat_set);

    if (sensitivity == 1)
      sensitivity = pat_set & 1 << Upper ? 2 : 0;
    case_sensitivity = sensitivity;
    size_t n = 0;

    for (size_t i = 0; i < pattern.size(); i++)
      if (pattern[i] != ' ') {
        pat += pattern[i];
        low_pat[n] = (char)::tolower(pattern[i]);
        pat_role[n] = pat_role[i];
        n++;
    }
}



}

#include "fuzzy_match.hh"

#include <iostream>

#include <algorithm>
#include <ctype.h>
#include <stdio.h>
#include <vector>


namespace fuzz{

  namespace{

    enum CharClass { Other, Lower, Upper };
    enum CharRole { None, Tail, Head };


    int main( int, char** ){

      std::cout << "Hello World!" << std::endl;
      return 0;
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

}

#include "dm_problems/solutions.hpp"
#include <iostream>

namespace {

void display_problems(const dm_problems::ProblemsMap &problems) {
  for (auto pair : problems) {
    std::cout << "- " << pair.first << std::endl;
  }
}

} // namespace

int main() {
  dm_problems::ProblemsMap problems = dm_problems::get_problems();

  display_problems(problems);
}

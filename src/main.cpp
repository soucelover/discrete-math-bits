#include "dm_problems/problem.hpp"
#include "dm_problems/solutions.hpp"
#include <iostream>
#include <limits>

namespace {

void display_problems(const dm_problems::ProblemsMap &problems) {
  for (auto pair : problems) {
    std::cout << "- " << pair.first << std::endl;
  }
}

const dm_problems::Problem *
get_problem(const dm_problems::ProblemsMap &problems) {
  int no;

  while (true) {

    if (!(std::cin >> no)) {
      std::cout << "Invalid input. Coulnd't interpret it as an integer."
                << std::endl;

      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      continue;
    }

    if (!problems.contains(no)) {
      std::cout << "Couldn't find a problem #" << no << ". Try again."
                << std::endl;
      continue;
    }

    break;
  }

  return problems.at(no);
}

} // namespace

int main() {
  dm_problems::ProblemsMap problems = dm_problems::get_problems();

  std::cout << "Lits of problem solutions:" << std::endl << std::endl;

  display_problems(problems);

  std::cout << std::endl << "Which solution would you like to launch? ";

  const dm_problems::Problem *problem = get_problem(problems);

  if (problem == nullptr) {
    std::cout << "UNEXPECTED ERROR: Couldn't find corresponding solution.";
    return 1;
  }

  return problem->run();
}

#pragma once

#include "dm_problems/problem.hpp"
#include <map>

namespace dm_problems {

extern const Problem *const problem_109;
extern const Problem *const problem_110;
extern const Problem *const problem_111;

typedef std::map<int, const Problem *> ProblemsMap;

inline ProblemsMap get_problems() {
  return {{109, problem_109}, {110, problem_110}, {111, problem_111}};
}

} // namespace dm_problems

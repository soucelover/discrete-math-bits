#pragma once

#include <functional>

namespace dm_problems {

class Problem {
public:
  virtual int run() const = 0;
};

class InlineProblem : public Problem {
protected:
  std::function<int()> run_function;

public:
  InlineProblem(std::function<int()> run_function)
      : run_function(run_function) {}

  virtual int run() const override { return run_function(); };
};

} // namespace dm_problems

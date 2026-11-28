#pragma once

#include "dm_algorithms/tree.hpp"
#include <algorithm>
#include <concepts>
#include <vector>

namespace dm_algorithms {

template <typename T> class PrefixCodeTreeBase : public BinaryTreeBase<T> {
private:
  using Base = BinaryTreeBase<T>;

public:
  template <typename Self>
  std::vector<bool> get_code(this const Self &self)
    requires(std::same_as<Self, T>)
  {
    std::vector<bool> code;
    typename Base::pointer current = &self;

    while (current->has_parent()) {
      code.push_back(current->is_right());
      current = current->parent();
    }

    std::reverse(std::begin(code), std::end(code));
    return code;
  }

  template <typename Self>
  void extract_codes(this const Self &self,
                     void(store_code)(const std::vector<bool> &)) {
    for (const auto &node : self) {
      if (node.is_leaf()) {
        store_code(node.get_code());
      }
    }
  }
};

class PrefixCodeTree : public PrefixCodeTreeBase<PrefixCodeTree> {};

} // namespace dm_algorithms

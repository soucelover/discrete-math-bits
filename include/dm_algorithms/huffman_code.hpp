#include <algorithm>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

template <typename T>
concept Numeric = std::is_arithmetic_v<T>;

template <typename KeyT, Numeric FreqT> class Base {

protected:
  Base(FreqT frequency) : frequency(frequency) {}

public:
  const FreqT frequency;

  virtual ~Base() {};

  virtual void extract_codes(std::unordered_map<KeyT, std::vector<bool>> &dst,
                             const std::vector<bool> &prefix) = 0;
};

template <typename KeyT, Numeric FreqT> class Node : public Base<KeyT, FreqT> {
  using Base = Base<KeyT, FreqT>;

  const std::shared_ptr<Base> left;
  const std::shared_ptr<Base> right;

public:
  Node(std::shared_ptr<Base> left, std::shared_ptr<Base> right)
      : left(left), right(right), Base(left->frequency + right->frequency) {}

  void extract_codes(std::unordered_map<KeyT, std::vector<bool>> &dst,
                     const std::vector<bool> &prefix) override {
    std::vector<bool> child_pref = prefix;

    child_pref.push_back(false);
    left->extract_codes(dst, child_pref);

    child_pref.back() = true;
    right->extract_codes(dst, child_pref);
  }
};

template <typename KeyT, Numeric FreqT> class Leaf : public Base<KeyT, FreqT> {
  using Base = Base<KeyT, FreqT>;

  const KeyT key;

public:
  Leaf(KeyT key, double frequency) : key(key), Base(frequency) {}

  void extract_codes(std::unordered_map<KeyT, std::vector<bool>> &dst,
                     const std::vector<bool> &prefix) override {
    if (prefix.empty()) {
      dst[key] = {0};
      return;
    }

    dst[key] = prefix;
  }
};

template <typename KeyT, Numeric FreqT>
inline size_t find_insertion_index(
    const std::vector<std::shared_ptr<Base<KeyT, FreqT>>> &pairs,
    std::shared_ptr<Base<KeyT, FreqT>> node) {
  if (pairs.empty()) {
    return 0;
  }

  long long l = 0, r = pairs.size() - 1;

  while (l <= r) {
    size_t c = l + ((r - l) >> 1);

    if (pairs[c]->frequency == node->frequency) {
      return c;
    } else if (pairs[c]->frequency < node->frequency) {
      l = c + 1;
    } else {
      r = c - 1;
    }
  }
  return l;
}

template <typename KeyT, Numeric FreqT>
void build_huffman_code(const std::unordered_map<KeyT, FreqT> &src,
                        std::unordered_map<KeyT, std::vector<bool>> &dst) {
  using Base = Base<KeyT, FreqT>;
  using Node = Node<KeyT, FreqT>;
  using Leaf = Leaf<KeyT, FreqT>;

  if (!dst.empty()) {
    throw std::invalid_argument("dst must be an empty unordered map.");
  }

  std::vector<std::shared_ptr<Base>> pairs;

  for (auto &pair : src) {
    pairs.push_back(std::make_shared<Leaf>(pair.first, pair.second));
  }

  std::ranges::sort(pairs, [](const std::shared_ptr<Base> &left,
                              const std::shared_ptr<Base> &right) {
    return left->frequency < right->frequency;
  });

  if (pairs.empty()) {
    return;
  }

  while (pairs.size() > 1) {
    std::shared_ptr<Base> left = pairs[0];
    std::shared_ptr<Base> right = pairs[1];

    pairs.erase(pairs.begin(), pairs.begin() + 2);

    std::shared_ptr<Node> node = std::make_shared<Node>(left, right);
    const auto iter =
        pairs.begin() + find_insertion_index<KeyT, FreqT>(pairs, node);

    pairs.insert(iter, node);
  }

  pairs[0]->extract_codes(dst, {});
}

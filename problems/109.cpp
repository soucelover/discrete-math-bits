#include "huffman_code.hpp"
#include <cstddef>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

static const std::unordered_map<const char *, double> english_letters = {
    {"a", 0.082},  {"b", 0.015},  {"c", 0.028}, {"d", 0.043},  {"e", 0.127},
    {"f", 0.022},  {"g", 0.02},   {"h", 0.061}, {"i", 0.07},   {"j", 0.0016},
    {"k", 0.0077}, {"l", 0.04},   {"m", 0.024}, {"n", 0.067},  {"o", 0.075},
    {"p", 0.019},  {"q", 0.0012}, {"r", 0.06},  {"s", 0.063},  {"t", 0.091},
    {"u", 0.028},  {"v", 0.0098}, {"w", 0.024}, {"x", 0.0015}, {"y", 0.02},
    {"z", 0.00074}};

std::string str_from_vector(const std::vector<bool> &vec) {
  std::string out(vec.size(), '0');

  for (size_t i = 0; i < vec.size(); ++i) {
    out[i] = '0' + vec[i];
  }

  return out;
}

int main() {

  std::unordered_map<const char *, std::vector<bool>> dst;

  build_huffman_code(english_letters, dst);

  for (auto pair : dst) {
    std::cout << pair.first << " : " << str_from_vector(pair.second)
              << std::endl;
  }
}

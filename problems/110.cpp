#include "huffman_code.hpp"
#include <cstddef>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

static const std::unordered_map<const char *, double> english_letters = {
    {"а", 40487008}, {"б", 8051767},  {"в", 22930719}, {"г", 8564640},
    {"д", 15052118}, {"е", 42691213}, {"ё", 184928},   {"ж", 4746916},
    {"з", 8329904},  {"и", 37153142}, {"й", 6106262},  {"к", 17653469},
    {"л", 22230174}, {"м", 16203060}, {"н", 33838881}, {"о", 55414481},
    {"п", 14201572}, {"р", 23916825}, {"с", 27627040}, {"т", 31620970},
    {"у", 13245712}, {"ф", 1335747},  {"х", 4904176},  {"ц", 2438807},
    {"ч", 7300193},  {"ш", 3678738},  {"щ", 1822476},  {"ъ", 185452},
    {"ы", 9595941},  {"ь", 8784613},  {"э", 1610107},  {"ю", 3220715},
    {"я", 10139085}};

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

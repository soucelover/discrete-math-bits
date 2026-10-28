#include "huffman_code.hpp"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace std {
template <> struct hash<std::vector<char>> {
  size_t operator()(const std::vector<char> &data) const {
    size_t hash = 0xcbf29ce484222325ULL; // FNV offset basis
    size_t prime = 0x100000001b3ULL;     // FNV prime

    for (size_t i = 0; i < data.size(); ++i) {
      hash ^= static_cast<uint8_t>(data[i]);
      hash *= prime;
    }

    return hash;
  }
};
} // namespace std

std::string str_from_vector(const std::vector<bool> &vec) {
  std::string out(vec.size(), '0');

  for (size_t i = 0; i < vec.size(); ++i) {
    out[i] = '0' + vec[i];
  }

  return out;
}

constexpr size_t CHUNK_SIZE = 256;

bool get_chunks(const std::filesystem::path &file,
                std::unordered_map<std::vector<char>, size_t> &counter) {

  std::ifstream ifile(file, std::ios::binary);

  if (!ifile) {
    return false;
  }

  std::vector<char> chunk(CHUNK_SIZE);

  while (ifile.read(chunk.data(), CHUNK_SIZE) || ifile.gcount() > 0) {
    std::size_t bytes_read = ifile.gcount();

    ++counter[chunk];
  }

  return true;
}

bool output_result(
    const std::filesystem::path &file,
    std::unordered_map<std::vector<char>, size_t> frequencies,
    const std::unordered_map<std::vector<char>, std::vector<bool>> &result) {

  std::ofstream ofile(file, std::ios::binary);

  if (!ofile) {
    return false;
  }

  size_t max_length = 0, compressed_size = 0, real_size = 0;
  size_t i = 0;

  for (auto pair : result) {
    if (pair.second.size() > max_length) {
      max_length = pair.second.size();
    }

    size_t frequency = frequencies[pair.first];
    compressed_size += pair.second.size() * frequency;
    real_size += pair.first.size() * frequency;
    ofile << i++ << " : " << str_from_vector(pair.second) << std::endl;
  }

  ofile << std::endl
        << "max_length : " << max_length << std::endl
        << "real_size : " << real_size << std::endl
        << "compressed_size : " << compressed_size << std::endl;
  return true;
}

int main() {
  std::unordered_map<std::vector<char>, size_t> chunks;

  if (!get_chunks("./problems/zen.bmp", chunks)) {
    std::cerr << "ERROR: Couldn't open the input file.";
    return 1;
  }

  std::unordered_map<std::vector<char>, std::vector<bool>> dst;

  build_huffman_code(chunks, dst);

  if (!output_result("./output.txt", chunks, dst)) {
    std::cerr << "ERROR: Couldn't open the output file.";
    return 1;
  }
}

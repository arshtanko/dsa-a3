#include "HashFuncGen.h"
#include "RandomStreamGen.h"
#include <iomanip>
#include <iostream>
#include <set>

void printHashes(const std::vector<std::string> &stream, HashFuncGen &hashGen,
                 size_t count = 10) {
  std::cout << std::setw(5) << "No" << std::setw(25) << "String"
            << std::setw(15) << "Hash (hex)" << std::setw(15) << "Hash (dec)"
            << std::endl;
  std::cout << std::string(60, '-') << std::endl;

  for (size_t i = 0; i < std::min(count, stream.size()); ++i) {
    uint32_t hash = hashGen.hash(stream[i]);
    std::cout << std::setw(5) << (i + 1) << std::setw(25) << stream[i] << "  0x"
              << std::hex << std::setw(8) << std::setfill('0') << hash
              << std::dec << std::setfill(' ') << std::setw(12) << hash
              << std::endl;
  }
}

void analyzeStream(const std::vector<std::string> &stream,
                   HashFuncGen &hashGen) {
  std::set<std::string> uniqueStrings(stream.begin(), stream.end());
  std::set<uint32_t> uniqueHashes;

  for (const auto &str : stream) {
    uniqueHashes.insert(hashGen.hash(str));
  }

  std::cout << "Stream size:        " << stream.size() << std::endl;
  std::cout << "Unique strings:     " << uniqueStrings.size() << " ("
            << std::fixed << std::setprecision(1)
            << (100.0 * uniqueStrings.size() / stream.size()) << "%)"
            << std::endl;
  std::cout << "Unique hashes:      " << uniqueHashes.size() << " ("
            << (100.0 * uniqueHashes.size() / stream.size()) << "%)"
            << std::endl;
  std::cout << "Hash collisions:    "
            << (uniqueStrings.size() - uniqueHashes.size()) << std::endl;
}

int main() {

  HashFuncGen hashGen(42);

  std::cout << "Testing hash uniformity 10000 strings, 100 buckets:"
            << std::endl;
  RandomStreamGen largeStream(10000);
  largeStream.generateStream();

  auto stats = hashGen.testUniformity(largeStream.getStream(), 100);

  std::cout << "Total items:        " << stats.totalItems << std::endl;
  std::cout << "Buckets:            " << stats.numBuckets << std::endl;
  std::cout << "Expected per bucket: " << std::fixed << std::setprecision(2)
            << stats.expectedPerBucket << std::endl;
  std::cout << "Max deviation:      " << stats.maxDeviation << std::endl;

  return 0;
}

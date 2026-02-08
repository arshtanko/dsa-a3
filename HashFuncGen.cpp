#include "HashFuncGen.h"
#include <algorithm>
#include <cmath>

HashFuncGen::HashFuncGen(uint32_t randomSeed)
    : rng(randomSeed == 0 ? std::random_device{}() : randomSeed) {
  regenerateParameters();
}

void HashFuncGen::regenerateParameters() {
  std::uniform_int_distribution<uint32_t> dist(1e9, 2e9);
  polynomialBase = dist(rng);
  if (polynomialBase % 2 == 0) {
    polynomialBase++;
  }
}

uint32_t HashFuncGen::polynomialHash(const std::string &str) const {
  uint64_t hash = 0;
  uint64_t base = polynomialBase;

  for (char c : str) {
    hash = (hash * base + static_cast<unsigned char>(c)) & 0xFFFFFFFFULL;
  }

  return static_cast<uint32_t>(hash);
}

uint32_t HashFuncGen::hash(const std::string &str) const {
  return polynomialHash(str);
}

HashFuncGen::UniformityStats
HashFuncGen::testUniformity(const std::vector<std::string> &data,
                            size_t numBuckets) const {

  UniformityStats stats;
  stats.numBuckets = numBuckets;
  stats.totalItems = data.size();
  stats.bucketCounts.resize(numBuckets, 0);

  for (const auto &str : data) {
    uint32_t hashValue = hash(str);
    size_t bucket = hashValue % numBuckets;
    stats.bucketCounts[bucket]++;
  }

  stats.expectedPerBucket = static_cast<double>(data.size()) / numBuckets;

  stats.maxDeviation = 0.0;

  for (size_t count : stats.bucketCounts) {
    double diff = count - stats.expectedPerBucket;
    stats.maxDeviation = std::max(stats.maxDeviation, std::abs(diff));
  }

  return stats;
}

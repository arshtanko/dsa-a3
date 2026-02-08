#ifndef HASHFUNCGEN_H
#define HASHFUNCGEN_H

#include <cstdint>
#include <random>
#include <string>
#include <vector>

class HashFuncGen {
private:
  uint32_t polynomialBase;
  std::mt19937 rng;

  uint32_t polynomialHash(const std::string &str) const;

public:
  explicit HashFuncGen(uint32_t randomSeed = 0);

  uint32_t hash(const std::string &str) const;

  void regenerateParameters();

  struct UniformityStats {
    double expectedPerBucket;
    double maxDeviation;
    size_t totalItems;
    size_t numBuckets;
    std::vector<size_t> bucketCounts;
  };

  UniformityStats testUniformity(const std::vector<std::string> &data,
                                 size_t numBuckets = 1000) const;
};

#endif

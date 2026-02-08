#ifndef RANDOMSTREAMGEN_H
#define RANDOMSTREAMGEN_H

#include <memory>
#include <random>
#include <string>
#include <vector>

class RandomStreamGen {
private:
  size_t totalSize;
  std::vector<std::string> stream;
  std::mt19937 rng;

  static const std::string ALLOWED_CHARS;
  static const size_t MAX_STRING_LENGTH = 30;

  std::string generateRandomString();

public:
  explicit RandomStreamGen(size_t size);

  void generateStream();

  const std::vector<std::string> &getStream() const;

  std::vector<std::string> getStreamPortion(double percentage) const;

  std::vector<std::string> getStreamUpToTime(size_t t) const;

  size_t getSize() const;

  void clear();
};

#endif

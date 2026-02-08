#include "RandomStreamGen.h"
#include <iomanip>
#include <iostream>
#include <stdexcept>

const std::string RandomStreamGen::ALLOWED_CHARS =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-";

RandomStreamGen::RandomStreamGen(size_t size)
    : totalSize(size), rng(std::random_device{}()) {
  if (size == 0) {
    throw std::invalid_argument("Stream size must be greater than 0");
  }
  stream.reserve(size);
}

std::string RandomStreamGen::generateRandomString() {
  std::uniform_int_distribution<size_t> lengthDist(1, MAX_STRING_LENGTH);
  size_t length = lengthDist(rng);

  std::uniform_int_distribution<size_t> charDist(0, ALLOWED_CHARS.length() - 1);
  std::string result;
  result.reserve(length);

  for (size_t i = 0; i < length; ++i) {
    result += ALLOWED_CHARS[charDist(rng)];
  }

  return result;
}

void RandomStreamGen::generateStream() {
  stream.clear();
  stream.reserve(totalSize);

  for (size_t i = 0; i < totalSize; ++i) {
    stream.push_back(generateRandomString());
  }
}

const std::vector<std::string> &RandomStreamGen::getStream() const {
  return stream;
}

std::vector<std::string>
RandomStreamGen::getStreamPortion(double percentage) const {
  if (percentage < 0.0 || percentage > 1.0) {
    throw std::invalid_argument("Percentage must be between 0.0 and 1.0");
  }

  if (stream.empty()) {
    throw std::runtime_error("Stream is empty. Call generateStream() first.");
  }

  size_t count = static_cast<size_t>(stream.size() * percentage);
  return std::vector<std::string>(stream.begin(), stream.begin() + count);
}

std::vector<std::string> RandomStreamGen::getStreamUpToTime(size_t t) const {
  if (stream.empty()) {
    throw std::runtime_error("Stream is empty. Call generateStream() first.");
  }

  if (t > stream.size()) {
    throw std::out_of_range("Time point t exceeds stream size");
  }

  return std::vector<std::string>(stream.begin(), stream.begin() + t);
}

size_t RandomStreamGen::getSize() const { return stream.size(); }

void RandomStreamGen::clear() { stream.clear(); }

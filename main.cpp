#include "HashFuncGen.h"
#include "RandomStreamGen.h"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <unordered_set>
#include <vector>

size_t exactUnique(const std::vector<std::string> &stream) {
  std::unordered_set<std::string> s(stream.begin(), stream.end());
  return s.size();
}

double hyperLogLog(const std::vector<std::string> &stream, HashFuncGen &hashGen,
                   int B) {
  int m = 1 << B;
  std::vector<int> M(m, 0);

  for (const auto &str : stream) {
    uint32_t h = hashGen.hash(str);
    int idx = h >> (32 - B);
    uint32_t w = (h << B) | (1u << (B - 1));
    int rho = __builtin_clz(w) + 1;
    if (rho > M[idx])
      M[idx] = rho;
  }

  double alpha;
  if (m == 16)
    alpha = 0.673;
  else if (m == 32)
    alpha = 0.697;
  else if (m == 64)
    alpha = 0.709;
  else
    alpha = 0.7213 / (1.0 + 1.079 / m);

  double Z = 0.0;
  for (int j = 0; j < m; j++)
    Z += std::pow(2.0, -M[j]);
  double E = alpha * m * m / Z;

  if (E <= 2.5 * m) {
    int V = 0;
    for (int j = 0; j < m; j++)
      if (M[j] == 0)
        V++;
    if (V > 0)
      E = m * std::log(static_cast<double>(m) / V);
  }
  double pow32 = std::pow(2.0, 32);
  if (E > pow32 / 30.0)
    E = -pow32 * std::log(1.0 - E / pow32);

  return E;
}

int main() {
  HashFuncGen hashGen(42);

  std::cout << std::fixed << std::setprecision(2);

  std::cout << "Подбор B\n";

  RandomStreamGen testGen(50000, 42);
  testGen.generateStream();
  const auto &testStream = testGen.getStream();
  size_t exact = exactUnique(testStream);

  std::cout << std::setw(5) << "B" << std::setw(8) << "m" << std::setw(8)
            << "L-B" << std::setw(12) << "N_t" << std::setw(10) << "F_t^0"
            << std::setw(10) << "Err%\n";
  std::cout << std::string(53, '-') << "\n";

  int bestB = 4;
  double bestErr = 1e18;

  for (int b = 4; b <= 16; b += 2) {
    double nt = hyperLogLog(testStream, hashGen, b);
    double err = std::abs(nt - (double)exact) / exact * 100.0;
    std::cout << std::setw(5) << b << std::setw(8) << (1 << b) << std::setw(8)
              << (32 - b) << std::setw(12) << nt << std::setw(10) << exact
              << std::setw(9) << err << "%\n";
    if (err < bestErr) {
      bestErr = err;
      bestB = b;
    }
  }
  std::cout << "\nВыбран B=" << bestB << " (ошибка " << bestErr << "%)\n";

  std::cout << "\nОсновной этап" << "\n\n";

  std::vector<size_t> sizes = {1000, 10000, 100000};
  std::vector<double> portions = {0.25, 0.5, 0.75, 1.0};
  const int RUNS = 10;

  for (size_t sz : sizes) {
    std::cout << "--- Stream " << sz << ", " << RUNS << " runs ---\n";
    std::cout << std::setw(8) << "Part" << std::setw(10) << "F_t^0"
              << std::setw(12) << "E(N_t)" << std::setw(12) << "sigma\n";
    std::cout << std::string(42, '-') << "\n";

    for (double p : portions) {
      std::vector<double> ests;
      size_t ft0 = 0;

      for (int r = 0; r < RUNS; r++) {
        RandomStreamGen gen(sz, 300 + r);
        gen.generateStream();
        auto sub = gen.getStreamPortion(p);
        ft0 = exactUnique(sub);
        ests.push_back(hyperLogLog(sub, hashGen, bestB));
      }

      double mean = std::accumulate(ests.begin(), ests.end(), 0.0) / RUNS;
      double sq = 0;
      for (double v : ests)
        sq += (v - mean) * (v - mean);
      double sigma = std::sqrt(sq / RUNS);

      std::cout << std::setw(6) << (int)(p * 100) << "%" << std::setw(10) << ft0
                << std::setw(12) << mean << std::setw(12) << sigma << "\n";
    }
    std::cout << "\n";
  }

  const size_t N = 10000;
  const int STEPS = 20;
  const int PLOT_RUNS = 10;

  {
    RandomStreamGen gen(N, 42);
    gen.generateStream();
    const auto &s = gen.getStream();
    std::ofstream f("graph1.csv");
    f << "t,Ft0,Nt\n";
    for (int i = 1; i <= STEPS; i++) {
      size_t t = N * i / STEPS;
      auto sub = std::vector<std::string>(s.begin(), s.begin() + t);
      f << t << "," << exactUnique(sub) << ","
        << hyperLogLog(sub, hashGen, bestB) << "\n";
    }
  }

  {
    std::vector<RandomStreamGen> gens;
    for (int r = 0; r < PLOT_RUNS; r++) {
      gens.emplace_back(N, 500 + r);
      gens.back().generateStream();
    }
    std::ofstream f("graph2.csv");
    f << "t,Ft0,E_Nt,lower,upper\n";
    for (int i = 1; i <= STEPS; i++) {
      size_t t = N * i / STEPS;
      std::vector<double> ests;
      for (int r = 0; r < PLOT_RUNS; r++) {
        auto sub = std::vector<std::string>(gens[r].getStream().begin(),
                                            gens[r].getStream().begin() + t);
        ests.push_back(hyperLogLog(sub, hashGen, bestB));
      }
      auto sub0 = std::vector<std::string>(gens[0].getStream().begin(),
                                           gens[0].getStream().begin() + t);
      size_t ft0 = exactUnique(sub0);
      double mean = std::accumulate(ests.begin(), ests.end(), 0.0) / PLOT_RUNS;
      double sq = 0;
      for (double v : ests)
        sq += (v - mean) * (v - mean);
      double sigma = std::sqrt(sq / PLOT_RUNS);
      f << t << "," << ft0 << "," << mean << "," << (mean - sigma) << ","
        << (mean + sigma) << "\n";
    }
  }

  return 0;
}

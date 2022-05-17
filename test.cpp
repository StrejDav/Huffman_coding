#include <cmath>
#include <vector>
#include <numeric>

class Decomp
{
  public:
  static std::vector<long long> decompose(long long n);
};

std::vector<long long> Decomp::decompose(long long n) {
  int next;
  int sum = 0;
  int rem;
  int indToFix = 1;
  std::vector<long long> retVec = {n - 1};
  
  while (sum != n) {
    sum = std::inner_product(retVec.begin(), retVec.end(), retVec.begin(), 0);
    rem = pow(n, 2) - sum;
    while (sum < pow(n, 2)) {
      next = std::floor(sqrt(rem));
      sum += pow(next, 2);
      rem -= pow(next, 2);
      retVec.push_back(next);
      if (sum == pow(n, 2)) break;
    }
    indToFix = retVec.size() - 2;
    if (retVec[indToFix] <= 1) {
      retVec.erase(retVec.begin() + indToFix + 1, retVec.end());
      indToFix -= 1;
    }
    retVec.pop_back();
    retVec[indToFix] -= 1;
  }
  
  return retVec;
}

int main() {
  std::vector<long long> ans = Decomp::decompose(50);
  int c = 0;
}
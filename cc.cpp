#include <map>
#include <utility>
#include <vector>

std::map<char, std::pair<int, int>> directions = {
  {'n', std::pair<int, int>{0, 1}},
  {'s', std::pair<int, int>{0, -1}},
  {'w', std::pair<int, int>{1, 0}},
  {'e', std::pair<int, int>{-1, 0}}
};

bool isValidWalk(std::vector<char> walk) {
  if (walk.size() != 10) return false;
  int path = 0;
  for (auto x: walk) {
    path += directions[x].first + directions[x].second;
  }
  return path == 0;
}

int main () {
    bool c = isValidWalk(std::vector<char>{'n'});
    int p = 0;
}
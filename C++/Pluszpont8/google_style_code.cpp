#include <algorithm>
#include <iostream>
#include <vector>

struct Item
{
  int first_val;
  int second_val;
};

void ProcessAndPrint(const std::vector<Item> &items)
{
  for (const auto &item : items)
  {
    int r = item.first_val;
    int u = item.second_val;
    int v = 0;

    if (r % 2 == 0)
    {
      if (u % 2 == 0)
      {
        v = r * u;
      }
      else
      {
        v = r + u;
      }
    }
    else
    {
      if (u % 2 == 0)
      {
        v = r - u;
      }
      else
      {
        v = r;
      }
    }

    std::cout << v << '\n';
  }
}

void SortAndProcess(std::vector<Item> &items)
{
  std::sort(items.begin(), items.end(),
            [](const Item &lhs, const Item &rhs)
            {
              return lhs.first_val < rhs.first_val;
            });

  std::cout << "===\n";
  std::cout << "Output:\n";
  std::cout << "---\n";

  ProcessAndPrint(items);
}

int main()
{
  std::vector<Item> items = {
      {5, 1},
      {2, 4},
      {3, 7},
      {1, 6},
      {4, 5}};

  SortAndProcess(items);

  return 0;
}
#include <vector>
#include <algorithm>
#include <memory>

using namespace std;

struct ManacherPalindrome
{
  typedef size_t size_type;
  template<typename elem_type>
  vector<size_type> GetRadius(vector<elem_type> S) const
  {
    size_type n = S.size();
    vector<size_type> A(n, 0);
    size_type covered = 0, center = 0;
    for (size_type i = 0; i < n; i++)
    {
      size_type radius = 0;
      size_type limit = min(n-1-i, i);
      if (i < covered)
      {
        radius = min(covered - 1 - i, A[center - (i - center)]);
      }
      while (radius < limit && S[i - radius - 1] == S[i + radius + 1])
      {
        radius++;
      }
      A[i] = radius;
      if (i + radius + 1 >= covered)
      {
        covered = i + radius + 1;
        center = i;
      }
    }
    return A;
  }
};

#include <vector>
#include <string>

using namespace std;

vector<int> buildZ(const string &s) {
    int n = s.length();
    vector<int> Z(n); // Z[i] := length of the longest common prefix of s[0..] and s[i..]
    Z[0] = n;
    int l = 1, r = 0; // [l, r)
    for (int i = 1; i < n; i++) {
        if (Z[i - l] < r - i) {
            Z[i] = Z[i - l];
            continue;
        }
        l = i;
        r = max(i, r);
        while (r < n && s[r - l] == s[r]) r++;
        Z[i] = r - l;
    }
    return Z;
}

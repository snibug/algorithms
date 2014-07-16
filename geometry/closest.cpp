// Tests
// http://codeforces.com/contest/429/problem/D (CF#245 Tricky Function)

#include <algorithm>
#include <vector>

typedef pair<long long, long long> point;

point points[100003];

long long dist(const point &a, const point &b) {
	return (a.first - b.first) * (a.first - b.first) + (a.second - b.second) * (a.second - b.second);
}

bool bySecond(const point &a, const point &b) {
	return a.second < b.second;
}

// returns the closest squared distance between two points
long long getClosest(int s, int e) {
	long long res = LLONG_MAX;
	if (e - s < 5) {
		for(int i = s; i < e; i++) {
			for(int j = i+1; j < e; j++) {
				res = min(res, dist(points[i],points[j]));
			}
		}
		return res;
	}
	int m = (s+e)/2;
	long long lres = getClosest(s, m);
	long long rres = getClosest(m, e);
	res = min(res, lres);
	res = min(res, rres);

	vector<point> strip;
	for(int i = s; i < e; i ++) {
		if ((points[m].first - points[i].first) * (points[m].first - points[i].first) < res) {
			strip.emplace_back(points[i]);
		}
	}
	sort(strip.begin(), strip.end(), bySecond);
	for(int i = 0; i < strip.size(); i++) {
		for(int j = i + 1; j < strip.size() &&
			(strip[j].second - strip[i].second)*(strip[j].second - strip[i].second) < res; j++) {
			res = min(res, dist(strip[i], strip[j]));
		}
	}
	return res;
}


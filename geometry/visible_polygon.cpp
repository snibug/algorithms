#include <cstdio>
#include <set>
#include <map>
#include <unordered_map>
#include <cstdint>
#include <algorithm>
#include <vector>

using namespace std;

/* max_heap */
template<typename Ty, typename Pr = less<Ty>>
struct removable_heap
{
	typedef int id_t;
	typedef ptrdiff_t pos_t;

	Pr comparator;
	vector<pair<Ty, id_t>> heap;
	id_t lastid;
	unordered_map<id_t, pos_t> pos;
	removable_heap():lastid(0){}
	/* returns id */
	id_t push(Ty val)
	{
		id_t id = lastid++;
		pos_t cur = heap.size();
		heap.emplace_back(val, id);
		while(cur > 0)
		{
			pos_t par = (cur-1)>>1;
			/* satisfies heap */
			if (!comparator(heap[par].first, heap[cur].first))
				break;
			swap(heap[par], heap[cur]);
			pos[heap[cur].second] = cur;
			cur = par;
		}
		pos[id] = cur;
		return id;
	}
	bool empty() { return heap.empty(); }
	Ty &top() { return heap.front().first; }
	void adjust(pos_t hole)
	{
		for(;;)
		{
			pos_t par = (hole-1)>>1;
			pos_t left = hole*2 + 1;
			pos_t right = hole*2 + 2;
			pos_t swpind = hole;
			if (hole > 0 && comparator(heap[par].first, heap[hole].first)){
				swpind = par;
			} else if (right < heap.size()) {
				if (comparator(heap[left].first, heap[right].first)) {
					if (comparator(heap[hole].first, heap[right].first)) {
						swpind = right;
					}
				} else {
					if (comparator(heap[hole].first, heap[left].first)) {
						swpind = left;
					}
				}
			} else if (left < heap.size()) {
				if (comparator(heap[hole].first, heap[left].first)) {
					swpind = left;
				}
			}
			if (swpind == hole) break;
			swap(heap[swpind], heap[hole]);
			pos[heap[hole].second] = hole;
			hole = swpind;
		}
		pos[heap[hole].second] = hole;
	}
	void pop()
	{
		if (empty()) throw runtime_error("pop empty");
		if (heap.size() == 1) {
			heap.clear();
			pos.clear();
			return;
		}
		pos.erase(heap[0].second);
		swap(heap[0], heap.back());
		heap.pop_back();
		adjust(0);
	}
	bool remove(id_t id)
	{
		if (pos.count(id) == 0) return false;
		auto cur = pos[id];
		pos.erase(id);
		if (cur + 1 == heap.size()) {
			heap.pop_back();
			return true;
		}
		swap(heap[cur], heap.back());
		heap.pop_back();
		adjust(cur);
	}
};

struct point
{
	long long x,y,z;
	point():x(0),y(0),z(1){}
	point(long long x, long long y, long long z = 1) : x(x),y(y),z(z){}

	int quadrant() const {
		auto x = this->x, y = this->y;
		if (z < 0) x = -x, y = -y;
		if (x == 0 && y == 0) return 0;
		if (y >= 0 && x > 0 ) return 1;
		if (y > 0) return 2;
		if (x < 0) return 3;
		return 4;
	}
};

point lineeq(const point &a, const point &b)
{
	point x(
		a.y*b.z - b.y*a.z,
		a.z*b.x - b.z*a.x,
		a.x*b.y - b.x*a.y
		);
	if (x.z < 0) x.x = -x.x, x.y = -x.y, x.z = -x.z;
	return x;
}

struct segment
{
	point a, b;
	segment():a(),b(){}
	segment(point a, point b):a(a),b(b){}
};

int polar_cmp_strict(const point &a, const point &b)
{
	int pa = a.quadrant(), pb = b.quadrant();
	if (pa != pb)
		return pa - pb;
	auto ccwres = a.x * b.y - a.y * b.x;
	return (ccwres < 0) - (ccwres > 0);
}

int polar_cmp_wraparound(const point &a, const point &b)
{
	auto ccwres = a.x * b.y - a.y * b.x;
	return (ccwres < 0) - (ccwres > 0);
}

point project_point(const point &a, const segment &b)
{
	point l = lineeq(b.a, b.b);
	point ret(
		l.z*(a.x),
		l.z*(a.y),
		-(a.x * l.x + a.y * l.y));
	return ret;
}

/* assumptions: no intersections between segments -> if there are, split them before use */
vector<pair<segment, int>> visiblePolygon(const vector<segment> &segments)
{
	struct ev
	{
		point p;
		int evtype;
		int ind;
	};
	int n = segments.size();
	vector<ev> sorted_events;
	struct heap_comparator
	{
		// near -> bigger
		point dir;
		point ndir;
		heap_comparator():dir(1,0),ndir(20001,1){}
		heap_comparator(point dir) : dir(dir){}
		bool operator ()(const pair<segment, int> &a, const pair<segment, int> &b) const
		{
			point la = lineeq(a.first.a, a.first.b), lb = lineeq(b.first.a, b.first.b);
			auto left = -la.z * (dir.x * lb.x + dir.y * lb.y);
			auto right = -lb.z * (dir.x * la.x + dir.y * la.y);
			if (left == right) {
				auto left2 = -la.z * (ndir.x * lb.x + ndir.y * lb.y);
				auto right2 = -lb.z * (ndir.x * la.x + ndir.y * la.y);
				return left2 > right2;
			}
			return left > right;
		}
	};
	removable_heap<pair<segment,int>, heap_comparator> dist_heap;
	vector<int> opened(n, -1);
	for (int i = 0; i < segments.size(); i++)
	{
		const auto &seg = segments[i];
		point a = seg.a, b = seg.b;
		if (polar_cmp_strict(a,b) == 0) continue; /* don't use */
		if (polar_cmp_wraparound(a,b) > 0) swap(a,b);
		// a is starting point, b is ending point
		ev e;
		e.p = a;
		e.evtype = 0;
		e.ind = i;
		sorted_events.emplace_back(e);
		e.p = b;
		e.evtype = 1;
		e.ind = i;
		sorted_events.emplace_back(e);
		// a comes later in the sorted events
		if (polar_cmp_strict(a,b) > 0) {
			opened[i] = dist_heap.push(make_pair(seg, i));
		}
	}
	{
		ev e;
		e.evtype = -1;
		e.ind = -1;
		e.p = point(1,0);
		sorted_events.emplace_back(e);
	}
	sort(sorted_events.begin(), sorted_events.end(), [](const ev &a, const ev &b) -> bool {
		auto cmp_res = polar_cmp_strict(a.p, b.p);
		if (cmp_res == 0) return a.evtype < b.evtype;
		return cmp_res < 0;
	});

	vector<pair<segment, int>> ans;
	int lastind = -1;
	point lastp;
	int j = 0;
	for (const auto &e : sorted_events)
	{
		while(j < sorted_events.size() && polar_cmp_strict(e.p, sorted_events[j].p) >= 0) j++;
		dist_heap.comparator.dir = e.p;
		if (j < sorted_events.size()) {
			dist_heap.comparator.ndir = sorted_events[j].p;
		} else{
			dist_heap.comparator.ndir = point(1,0);
		}
		if (e.ind >= 0) {
			if (e.evtype == 0) {
				opened[e.ind] = dist_heap.push(make_pair(segments[e.ind], e.ind));
			} else {
				dist_heap.remove(opened[e.ind]);
				opened[e.ind] = -1;
			}
		}
		int nextind = -1;
		point nextp(0,0);
		if (!dist_heap.empty()) {
			nextind = dist_heap.top().second;
			nextp = project_point(e.p, dist_heap.top().first);
		}
		if (nextind != lastind) {
			if (lastind != -1)
			{
				segment s;
				s.a = lastp;
				s.b = project_point(e.p, segments[lastind]);
				ans.emplace_back(s, lastind);
			}
			lastind = nextind;
			lastp = nextp;
		}
	}
	if (lastind != -1)
	{
		segment s;
		s.a = lastp;
		s.b = project_point(point(1,0), segments[lastind]);
		ans.emplace_back(s, lastind);
	}
	return ans;
}

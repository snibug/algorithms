#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <cstdlib>

using namespace std;

/* max_heap */
template<typename Ty>
struct removable_heap
{
	typedef int id_t;
	typedef ptrdiff_t pos_t;
	vector<pair<Ty, id_t>> heap;
	id_t lastid;
	unordered_map<id_t, pos_t> pos;
	removable_heap():lastid(0){}
	/* returns id */
	id_t push(typename Ty val)
	{
		id_t id = lastid++;
		pos_t cur = heap.size();
		heap.emplace_back(val, id);
		while(cur > 0)
		{
			pos_t par = (cur-1)>>1;
			/* satisfies heap */
			if (heap[par] >= heap[cur])
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
			if (hole > 0 && heap[par] < heap[hole]){
				swpind = par;
			} else if (right < heap.size()) {
				if (heap[left] < heap[right]) {
					if (heap[hole] < heap[right]) {
						swpind = right;
					}
				} else {
					if (heap[hole] < heap[left]) {
						swpind = left;
					}
				}
			} else if (left < heap.size()) {
				if (heap[hole] < heap[left]) {
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


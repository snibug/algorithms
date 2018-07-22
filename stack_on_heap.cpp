#include <cstdio>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <climits>
#include <cstring>
#include <string>
#include <vector>
#include <queue>
#include <numeric>
#include <functional>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <thread>
#include <tuple>
#include <utility>
#include <limits>

using namespace std;

void __attribute__((sysv_abi)) stack_on_heap(size_t size, void __attribute__((sysv_abi)) (*func)(void *), void *data) {
	// allocate enough
	size >>= 16; size += 2; size <<= 16;
	void *space = malloc(size);
	uintptr_t addr = reinterpret_cast<uintptr_t>(space) + size;
	// align anyway (4096 is probably more than enough)
	addr >>= 12; addr <<= 12;

	asm volatile(
			// backup
			"mov %%rsp, %%r12\n"
			"mov %%rbp, %%r13\n"
			"mov %%rcx, %%r14\n"
			// overwrite
			"mov %%rax, %%rsp\n"
			"sub $0x100, %%rsp\n"
			"mov %%rax, %%rbp\n"
			// call with rdi=data
			"call *%%rbx\n"
			// restore
			"mov %%r12, %%rsp\n"
			"mov %%r13, %%rbp\n"
			"mov %%r14, %%rcx\n"
			:"+a"(addr), "+b"(func), "+D"(data) /* in-out registers */
			:"c"(space) /* restore same value */
			:
			/* non-preserved registers */
			"rdx", "rsi", "r8", "r9", "r10", "r11",
			"xmm0", "xmm1", "xmm2", "xmm3", 
			"xmm4", "xmm5", "xmm6", "xmm7", 
			"xmm8", "xmm9", "xmm10", "xmm11", 
			"xmm12", "xmm13", "xmm14", "xmm15", 
			/* callee-saved registers for backup */
			"r12", "r13", "r14");
	// warning: addr, func, and data are invalid now
	free((void *)space);
}

struct Solver {
	long long ans;
	void input() {
	}

	long long factorial(int n) {
		if (n <= 0) return 1;
		return n * factorial(n-1) % 1'000'000'009;
	}

	void solve() {
		ans = factorial(10'000'000);
	}

	void output(int testcase) {
		printf("Case #%d: %d\n", testcase, (int)ans);
	}
};


void __attribute__((sysv_abi)) solve_caller(void *data) {
	Solver *solver = static_cast<Solver *>(data);
	solver->solve();
}

void my_start_function(Solver *solver) {
	stack_on_heap(160*1048576, &solve_caller, static_cast<void *>(solver));
}

int main() {
	int testCaseCount;
	scanf("%d", &testCaseCount);

	const int batchSize = max(1, (int)(thread::hardware_concurrency() / 2));
	for (int testcase = 1; testcase <= testCaseCount; testcase += batchSize) {
		vector<unique_ptr<Solver>> solvers; solvers.reserve(batchSize);
		vector<unique_ptr<thread>> threads; threads.reserve(batchSize);
		for (int batchInd = 0; batchInd < batchSize && testcase + batchInd <= testCaseCount; batchInd++) {
			solvers.emplace_back(new Solver());
		}
		for (auto &solver : solvers) {
			solver->input();
			threads.emplace_back(new thread(&my_start_function, solver.get()));
		}
		for (int batchInd = 0; batchInd < (int)solvers.size(); batchInd++) {
			threads[batchInd]->join();
			solvers[batchInd]->output(testcase + batchInd);
		}
	}
	return 0;
}


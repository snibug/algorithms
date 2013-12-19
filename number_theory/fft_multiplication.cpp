#include <stdio.h>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <stdexcept>
#include <ctime>

using namespace std;

namespace FFT
{
	// 다음 소수는 2^a * 3^b + 1 꼴인 것 중 적당히 고른 것들이다.
	int primes[]=
	{
		13, 37, 73, 97, 109, 193, 433, 577, 769, 1153, 1297, 2593,
		2917, 3457, 3889, 10369, 12289, 17497, 18433, 52489, 139969, 147457, 209953,
		331777, 472393, 629857, 746497, 786433, 839809, 995329, 1179649, 1492993,
		1769473, 1990657, 2654209, 5038849, 5308417, 14155777, 28311553, 63700993, 71663617
	};
	const int primeCnt = sizeof(primes)/sizeof(primes[0]);
	int unities[primeCnt];

	long long powmod(long long a, long long p, long long m) {
		long long res = 1;
		if (p < 0) {
			// assertion: m IS prime
			p += m-1;
		}
		while(p) {
			if (p % 2) {
				res *= a;
				res %= m;
			}
			a *= a;
			a %= m;
			p /= 2;
		}
		return res;
	}
	
	void initUnity()
	{
		if (unities[0] != 0) return;
		for(int i = 0; i < primeCnt; i++) {
			int p = primes[i];
			for(int j = 3; j < p; j += 2) {
				if ((p-1)%2 == 0 && powmod(j, (p-1)/2, p) == 1) {
					continue;
				}
				if ((p-1)%3 == 0 && powmod(j, (p-1)/3, p) == 1) {
					continue;
				}
				unities[i] = j;
				break;
			}
		}
	}

	vector<int> FFT(const vector<int> &input, int unity, int mod, int scalar)
	{
		vector<int> res(input.size(), 0);
		// base case
		if (input.size() <= 3)
		{
			long long curMult = 1;
			for(int i = 0; i < input.size(); i++) {
				long long sum = 0;
				long long innerMult = scalar;
				for(int j = 0; j < input.size(); j++) {
					sum += input[j] * innerMult % mod;
					sum %= mod;
					innerMult *= curMult;
					innerMult %= mod;
				}
				res[i] = sum;
				curMult *= unity;
				curMult %= mod;
			}
		}
		else
		{
			int divSize = 2;
			if (input.size() % 2) divSize = 3;
			if (input.size() % 3) throw std::runtime_error("wrong input size");
			vector<vector<int>> subinput;
			int upperUnity = powmod(unity, divSize, mod);
			for(int i = 0; i < divSize; i++) {
				subinput.emplace_back();
				subinput[i].reserve(input.size() / divSize);
				for(int j = i; j < input.size(); j += divSize) {
					subinput[i].push_back(input[j]);
				}
				subinput[i] = FFT(subinput[i], upperUnity, mod, scalar);
			}
			int N1 = divSize;
			int N2 = input.size() / divSize;
			long long curMult = 1;
			for(int k1 = 0; k1 < N1; k1++) {
				for(int k2 = 0; k2 < N2; k2++) {
					long long sum = 0;
					long long innerMult = 1;
					for(int n1 = 0; n1 < N1; n1++) {
						sum += subinput[n1][k2] * innerMult % mod;
						sum %= mod;
						innerMult *= curMult;
						innerMult %= mod;
					}
					res[N2*k1+k2] = sum;
					curMult *= unity;
					curMult %= mod;
				}
			}
		}
		return res;
	}
	vector<int> iFFT(const vector<int> &input, int unity, int mod, int scalar)
	{
		return FFT(input, powmod(unity, -1, mod), mod, scalar);
	}

	int getPrimeIndex(int inputLength, int radix) {
		initUnity();
		for(int i = 0; i < primeCnt; i++) {
			if (inputLength <= primes[i]-1 && radix <= primes[i]) {
				return i;
			}
		}
		return -1;
	}
	
	vector<int> FFT(vector<int> input, int primeIndex) {
		int p = primes[primeIndex], u = unities[primeIndex];
		input.resize(p-1,0);
		return FFT(input, u, p, 1);
	}

	vector<int> convolution(vector<int> input1, const vector<int> &input2, int primeIndex) {
		int p = primes[primeIndex];
		for(int i = 0; i < input1.size(); i++)
			input1[i] = input1[i] * (long long)input2[i] % p;
		return input1;
	}

	vector<int> iFFT(vector<int> input,int primeIndex) {
		int p = primes[primeIndex], u = unities[primeIndex];
		return iFFT(input, u, p, p-1);
	}
}
int main(){
	vector<int> testVector, testVector2;
	for(int i = 0; i < 100000; i++) {
		testVector.push_back(rand()%10);
		testVector2.push_back(rand()%10);
	}
	int pInd = FFT::getPrimeIndex(100000*2, 10*10);
	if (pInd < 0) { printf("너무 큼\n"); return 0; }
	vector<int> fftRes = FFT::FFT(testVector, pInd);
	vector<int> fftRes2 = FFT::FFT(testVector2, pInd);
	vector<int> multiplicationResult = FFT::iFFT(FFT::convolution(fftRes, fftRes2, pInd), pInd);
	printf("%d\n", (int)clock());
	return 0;
}


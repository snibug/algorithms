#include <cstdio>
#include <string>
#include <cstdint>
#include <vector>
#include <algorithm>

using namespace std;


/* Caution: very slow */
struct BigInteger
{
	static const int baselen = 9;
	static const int base = 1000000000;
	int sign; /* 1 if val >= 0, -1 otherwise */
	vector<uint32_t> num; /* each contains 10^9 */

	BigInteger() : sign(1), num(0) {}
	BigInteger(long long n) : sign(n >= 0 ? 1 : -1) {
		unsigned long long unum = n;
		if (sign == -1) unum = 0ull-unum;
		while (unum != 0) {
			num.push_back(unum % base);
			unum /= base;
		}
	}
	static int absoluteCmp(const vector<uint32_t> &n1, const vector<uint32_t> &n2) {
		if (n1.size() != n2.size()) {
			return (n1.size() > n2.size()) ? 1 : -1;
		}
		for (auto i = n1.size(); i-->0; ) {
			if (n1[i] != n2[i]) {
				return n1[i] > n2[i] ? 1 : -1;
			}
		}
		return 0;
	}
	bool operator < (const BigInteger &other) const {
		if (sign != other.sign)
			return sign < other.sign;
		return absoluteCmp(num, other.num) < 0;
	}
	bool operator <= (const BigInteger &other) const {
		if (sign != other.sign)
			return sign < other.sign;
		return absoluteCmp(num, other.num) <= 0;
	}
	bool operator > (const BigInteger &other) const {
		if (sign != other.sign)
			return sign > other.sign;
		return absoluteCmp(num, other.num) > 0;
	}
	bool operator >= (const BigInteger &other) const {
		if (sign != other.sign)
			return sign > other.sign;
		return absoluteCmp(num, other.num) >= 0;
	}
	bool operator == (const BigInteger &other) const {
		return sign == other.sign && num == other.num;
	}
	bool operator != (const BigInteger &other) const {
		return sign != other.sign || num != other.num;
	}
	static void add(BigInteger &dest, const vector<uint32_t> &n1, const vector<uint32_t> &n2) {
		dest.num.reserve(1 + max(n1.size(), n2.size()));
		uint32_t carry = 0;
		for (size_t i = 0; i < n1.size() || i < n2.size(); i++) {
			uint32_t cur = carry;
			if (i < n1.size()) cur += n1[i];
			if (i < n2.size()) cur += n2[i];
			if (cur >= base) {
				cur -= base;
				carry = 1;
			} else {
				carry = 0;
			}
			dest.num.push_back(cur);
		}
		if (carry) {
			dest.num.push_back(carry);
		}
	}
	static void subtract(BigInteger &dest, const vector<uint32_t> &n1, const vector<uint32_t> &n2) {
		const vector<uint32_t> *np1 = &n1, *np2 = &n2;
		dest.num.reserve(max(n1.size(), n2.size()));
		if (absoluteCmp(n1, n2) <= 0) {
			swap(np1, np2);
			dest.sign = -1;
		} else {
			dest.sign = 1;
		}
		int32_t carry = 0;
		for (size_t i = 0; i < np1->size() || i < np2->size(); i++) {
			int32_t cur = carry;
			if (i < np1->size()) cur += (*np1)[i];
			if (i < np2->size()) cur -= (*np2)[i];
			if (cur < 0) {
				cur += base;
				carry = -1;
			} else {
				carry = 0;
			}
			dest.num.push_back(cur);
		}
		while (!dest.num.empty() && dest.num.back() == 0) {
			dest.num.pop_back();
		}
	}
	static void multiply(BigInteger &dest, const vector<uint32_t> &n1, const vector<uint32_t> &n2) {
		vector<uint64_t> v(n1.size() + n2.size() + 3);
		for (size_t i = 0; i < n1.size(); i++) {
			uint64_t carry = 0;
			for (size_t j = 0; j < n2.size(); j++) {
				v[i+j] += (uint64_t) n1[i] * n2[j] + carry;
				carry = v[i+j] / base;
				v[i+j] %= base;
			}
			v[i+n2.size()] += carry;
		}
		{
			uint64_t carry = 0;
			for (size_t i = 0; i < v.size(); i++) {
				v[i] += carry;
				carry = v[i] / base;
				v[i] %= base;
			}
			while (!v.empty() && v.back() == 0)
				v.pop_back();
		}
		dest.num.reserve(v.size());
		for (auto d : v) dest.num.push_back(d);
	}
	static void division(BigInteger &dest, const vector<uint32_t> &n1, const vector<uint32_t> &n2) {
		if (absoluteCmp(n1, n2) < 0) {
			dest = 0;
			return;
		}
		dest.num.clear();

		BigInteger remain(0);
		remain.num = n1;

		for (int k = n1.size(); k >= 0; k--)
		{
			BigInteger sub;
			sub.num.assign(k, 0);
			sub.num.insert(sub.num.end(), n2.begin(), n2.end());
			if (sub > remain) {
				if (!dest.num.empty()) {
					dest.num.push_back(0);
				}
				continue;
			}

			int ans = 0;
			int low = 0, high = base - 1;
			while (low <= high) {
				int mid = low + (high-low)/2;
				BigInteger submul = sub * mid;
				if (submul <= remain) {
					ans = mid;
					low = mid + 1;
				} else {
					high = mid - 1;
				}
			}

			remain -= sub * ans;
			dest.num.push_back(ans);
		}
		reverse(dest.num.begin(), dest.num.end());
	}
	BigInteger operator + (const BigInteger &other) const {
		BigInteger ret;
		if (sign != other.sign) {
			subtract(ret, num, other.num);
			ret.sign *= sign;
		} else {
			add(ret, num, other.num);
			ret.sign = sign;
		}
		if (ret.num.empty())
			ret.sign = 1;
		return ret;
	}
	BigInteger operator - (const BigInteger &other) const {
		BigInteger ret;
		if (sign == other.sign) {
			subtract(ret, num, other.num);
			ret.sign *= sign;
		} else {
			add(ret, num, other.num);
			ret.sign = sign;
		}
		if (ret.num.empty())
			ret.sign = 1;
		return ret;
	}
	string to_string() const {
		string res;
		res.reserve(num.size() * baselen + 2);
		if (sign == -1) res.push_back('-');
		int fzero = 1;
		for (auto i = num.size(); i-->0; ) {
			uint32_t v = num[i];
			int pos = base / 10;
			while (pos) {
				int digit = v / pos;
				v %= pos;
				pos /= 10;
				if (digit)
					fzero = 0;
				if (fzero == 0) {
					res.push_back("0123456789"[digit]);
				}
			}
		}
		if (fzero)
			res.push_back('0');
		return res;
	}
	BigInteger operator * (const BigInteger &other) const {
		BigInteger ret;
		ret.sign = sign * other.sign;
		multiply(ret, num, other.num);
		if (ret.num.empty())
			ret.sign = 1;
		return ret;
	}
	BigInteger operator / (const BigInteger &other) const {
		BigInteger ret;
		ret.sign = sign * other.sign;
		division(ret, num, other.num);
		if (ret.num.empty())
			ret.sign = 1;
		return ret;
	}
	BigInteger operator % (const BigInteger &other) const {
		BigInteger val = *this;
		val -= other * (val / other);
		return val;
	}
	BigInteger &operator += (const BigInteger &other) {
		*this = *this + other;
		return *this;
	}
	BigInteger &operator -= (const BigInteger &other) {
		*this = *this - other;
		return *this;
	}
	BigInteger &operator *= (const BigInteger &other) {
		*this = *this * other;
		return *this;
	}
	BigInteger &operator /= (const BigInteger &other) {
		*this = *this / other;
		return *this;
	}
	BigInteger &operator %= (const BigInteger &other) {
		*this = *this % other;
		return *this;
	}
	long long getLongLong() const 
	{
		long long res = 0;
		long long b = sign;
		for (int i = 0; i < num.size(); i++) {
			res += b * num[i];
			b *= base;
		}
		return res;
	}
	long long getLongLongMod(const int mod) const 
	{
		long long res = 0;
		long long b = sign % mod;
		for (int i = 0; i < num.size(); i++) {
			res += b * num[i] % mod;
			b *= base;
			b %= mod;
		}
		res %= mod;
		return res;
	}
};


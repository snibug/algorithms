template<unsigned int mod>
struct ModInt
{
	unsigned int val;
	ModInt() : val(0) {}
	ModInt(const ModInt<mod> &other) : val(other.val) {}
	ModInt(long long x) : val(x % mod) {}
	unsigned int inverse() const {
		int a = val, b= mod, x = 1, y = 0;
		while (b) {
			int q = a / b;
			int t = a % b;
			a = b;
			b = t;
			int u = x - q * y;
			x = y;
			y = u;
		}
		if (x < 0) x += mod;
		return x;
	}
	ModInt &operator +=(const ModInt<mod> &other) {
		val += other.val;
		if (val >= mod)
			val -= mod;
		return *this;
	}
	ModInt &operator -=(const ModInt<mod> &other) {
		val += mod-other.val;
		if (val >= mod)
			val -= mod;
		return *this;
	}
	ModInt &operator *=(const ModInt<mod> &other) {
		val = (unsigned long long)val * other.val % mod;
		return *this;
	}
	ModInt &operator /=(const ModInt<mod> &other) {
		val = (unsigned long long)val * other.inverse() % mod;
		return *this;
	}
	ModInt operator +(const ModInt<mod> &other) const { return ModInt<mod>(val) += other; }
	ModInt operator -(const ModInt<mod> &other) const { return ModInt<mod>(val) -= other; }
	ModInt operator *(const ModInt<mod> &other) const { return ModInt<mod>(val) *= other; }
	ModInt operator /(const ModInt<mod> &other) const { return ModInt<mod>(val) /= other; }
	bool operator ==(const ModInt<mod> &other) const { return val == other.val; }
	bool operator !=(const ModInt<mod> &other) const { return val != other.val; }
	ModInt operator +() const { return *this; }
	ModInt operator -() const { return 0 == val ? 0 : (mod - val); }
	int operator !() const { return !val; }
};

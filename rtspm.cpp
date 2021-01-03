#include <tuple>
#include <vector>
#include <cassert>
#include <cstdio>
using namespace std;
#define debug(...) //fprintf(stderr, __VA_ARGS__)

typedef tuple <long long, long long, long long> hashtype;

long long fexp(long long base, int exponent, long long mod)
{
	long long res = 1;
	
	for (int a = 1; a < exponent; a *= 2, base = base * base % mod)
		if (exponent & a) res = res * base % mod;
	
	return res;
}

hashtype suffixFingerprint(hashtype pref, hashtype word, long long mod)
{
	long long fingerprint, baseExp, baseExpInw;
	auto [prefFP, prefExp, prefInw] = pref;
	auto [wordFP, wordExp, wordInw] = word;

	baseExp = wordExp * prefInw % mod;
	baseExpInw = wordInw * prefExp % mod;
	fingerprint = wordFP - prefFP * baseExp % mod;
	fingerprint = (mod + fingerprint) % mod; 

	return {fingerprint, baseExp, baseExpInw};
}

hashtype concatFingerprint(hashtype pref, hashtype suf, long long mod)
{
	long long fingerprint, baseExp, baseExpInw;
	auto [prefFP, prefExp, prefInw] = pref;
	auto [sufFP, sufExp, sufInw] = suf;

	fingerprint = (prefFP * sufExp + sufFP) % mod;
	baseExp = prefExp * sufExp;
	baseExpInw = prefInw * sufInw;

	return {fingerprint, baseExp, baseExpInw};
}

void appendLetter(hashtype &FP, int letter, long long base, long long baseInw, long long mod)
{
	auto [fingerprint, baseExp, baseExpInw] = FP;
	fingerprint = (fingerprint * base + letter) % mod;
	baseExp = baseExp * base % mod;
	baseExpInw = baseExpInw * baseInw % mod;
	FP = {fingerprint, baseExp, baseExpInw};
}

class Container {
public:

	Container(long long _mod)
	{
		mod = _mod;
	}

	bool insert(int pos, hashtype hash)
	{
		assert(pos > firstPos * n or n == 0);

		if (n == 0) {
			firstPos = pos;
			firstFP = hash;

		} else if (n == 1) {
			period = pos - firstPos;
			periodFP = suffixFingerprint(firstFP, hash, mod);
			lastFP = hash;

		} else {
			if (pos != firstPos * (n+1) or
				periodFP == suffixFingerprint(lastFP, hash, mod)) {
				
				return false;
			}

			lastFP = hash;
		}

		n++;
		return true;
	}

	bool query(int pos, hashtype &hash) {
		
		if (n == 0) return false;
		assert(pos <= firstPos);
		if (pos < firstPos) return false;

		hash = firstFP;
		if ((--n) > 0) {
			firstFP = concatFingerprint(firstFP, periodFP, mod);
			firstPos += period;
		}
		return true;
	}

private:
	int n = 0;

	int firstPos, period;
	hashtype firstFP, periodFP, lastFP;
	long long mod;
};

class RTSPM {
public:
	RTSPM(long long _mod = 1e9+7, long long _base = 29)
	{
		mod = _mod;
		base = _base;
		baseInw = fexp(base, mod-2, mod);
		currFP = {0, 1, 1};
	}

	void processPattern(int a)
	{
		assert(n == 0);
		if (m == 1 << levels) {
			Lengths.push_back(m);
			Fingerprints.push_back(currFP);
			Containers.push_back(Container(mod));
			levels++;
		}

		m++;
		appendLetter(currFP, a, base, baseInw, mod);
	}

	bool processText(int a)
	{
		if ((n++) == 0) {
			Lengths.push_back(m);
			Fingerprints.push_back(currFP);

			debug ("we have %d levels\n", levels);
			currFP = {0, 1, 1};
		}

		debug ("\n\nProcess a letter %d\n", a);

		if (collision) return true;

		
		bool report = (a == get<0>(Fingerprints[0]));
		hashtype reportFP = currFP;

		appendLetter(currFP, a, base, baseInw, mod);

		for (int i = 0; i<levels; i++)
		{
			if (report) {
				debug ("New occ at level %d\n", i);
			}

			bool occurrence;
			hashtype hash, levelFP;

			occurrence = Containers[i].query(n - Lengths[i+1], hash);
			if (occurrence) {
				levelFP =  suffixFingerprint(hash, currFP, mod);

				assert (get<1>(levelFP) == get<1>(Fingerprints[i+1]));
				debug ("Poromote occurance of level %d ? : %lld %lld\n", i, get<0>(levelFP), get<0>(Fingerprints[i+1]));

				occurrence = (levelFP == Fingerprints[i+1]);
			}

			if (report) {
				debug ("Save level %d at %d -- %lld\n", i, n - Lengths[i], get<1>(reportFP));
				Containers[i].insert(n - Lengths[i], reportFP);
			}

			report = occurrence;
			reportFP = hash;
		}

		return report;
	}

private:
	long long mod, base, baseInw;

	int m = 0, levels = 0;
	vector <hashtype> Fingerprints;
	vector <int> Lengths;
	vector <Container> Containers;

	int n = 0;
	hashtype currFP;
	bool collision = false;


};



int main ()
{
	int n, m;
	scanf ("%d %d", &n, &m);

	RTSPM Solver(1e9+7, 31);

	for (int i = 1; i <= m; i++)
	{
		char c;
		scanf (" %c", &c);
		Solver.processPattern(c - 'a');
	}

	for (int i = 1; i <= n; i++)
	{
		char c;
		scanf (" %c", &c);
		bool occurrence = Solver.processText(c - 'a');

		if (occurrence) printf ("%d\n", i - m);

		fflush(stdout);
	}
}
#include <tuple>
#include <vector>
#include <cassert>
#include <cstdio>
using namespace std;

/**
 * hashtype -- the signature of the fingerprint
 * Let h be the finger print of a word of length n, then:
 * h[0] is the actual fingerprint 
 * h[1] = base^n % mod
 * h[2] = base^(-n) % mod
 */
typedef tuple <long long, long long, long long> hashtype;

/**
 * fexp: fast exponentiation -- returns base^exponent % mod
 */
long long fexp(long long base, int exponent, long long mod)
{
	long long res = 1;
	for (int a = 1; a <= exponent; a *= 2, base = base * base % mod)
		if (exponent & a) res = res * base % mod;
	
	return res;
}

/**
 * suffixFingerprint -- returns the fingerprint of the suffix.
 * Let word and pref be fingerprints
 * of words a and ab respectively, then
 * suffixFingerprint returns the fingerprint of the word b.
 */
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

/** concatFingerprint -- returns the fingerprint of concatenation.
 * Let pref and suf be fingerprints of x and y respectively, then
 * the fingerprint of xy is returned.
 */
hashtype concatFingerprint(hashtype pref, hashtype suf, long long mod)
{
	long long fingerprint, baseExp, baseExpInw;
	auto [prefFP, prefExp, prefInw] = pref;
	auto [sufFP, sufExp, sufInw] = suf;

	fingerprint = (prefFP * sufExp + sufFP) % mod;
	baseExp = prefExp * sufExp % mod;
	baseExpInw = prefInw * sufInw % mod;

	return {fingerprint, baseExp, baseExpInw};
}

/** appendLetter -- updates the fingerprint to contain
 * the letter appended to the word this fingerprint represents.
 */
void appendLetter(hashtype &FP, int letter, long long base, long long baseInw, long long mod)
{
	auto [fingerprint, baseExp, baseExpInw] = FP;
	fingerprint = (fingerprint * base + letter) % mod;
	baseExp = baseExp * base % mod;
	baseExpInw = baseExpInw * baseInw % mod;
	FP = {fingerprint, baseExp, baseExpInw};
}


/** Container -- a class for storing occurrences
 * of pattern's prefixes of the same length.
 * Assumptions:
 * 
 * All positions in the text are queried
 * from left to right.
 * 
 * All occurrences in the structure must intersect. 
 */
class Container {
public:

	Container(long long _mod)
	{
		mod = _mod;
	}

	/**
	 * insert -- insert an occurrence:
	 * pos -- start of the occurrence
	 * hash -- fingerprint of the text's prefix (T[0...(pos-1)])
	 * returns true if the insertion succeeded
	 * if a hash collision was detected,
	 * the insertion fails and false is returned
	 */
	bool insert(int pos, hashtype hash)
	{
		assert(pos > firstPos + period * (n-1) or n == 0);

		if (n == 0) {
			firstPos = pos;
			firstFP = hash;

		} else if (n == 1) {
			period = pos - firstPos;
			periodFP = suffixFingerprint(firstFP, hash, mod);
			lastFP = hash;

		} else {

			/* Check if distances between occurrences are
			 * indeed equal. */
			if (pos != firstPos + period * n) {
				return false;
			}

			/* Check if fingerprints of supposed period
			 * occurrences match. */
			if (periodFP != suffixFingerprint(lastFP, hash, mod)) {
				return false;
			}

			lastFP = hash;
		}

		n++;
		return true;
	}

	/**
	 * query -- check whether an occurrence starting at
	 * pos has been recorded.
	 * If it was, remove it from the structure,
	 * populate hash with fingerprint of its prefix
	 * and return true.
	 * Otherwise, return false.
	 */
	bool query(int pos, hashtype &hash)
	{
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
	/* number of stored occurrences */
	int n = 0;

	/* beginning of the earliest occurrence */
	int firstPos;
	
	/* (supposed) length of the smallest period of the pattern's prefix */
	int period;

	/* fingerprints of the (supposed) smallest period, prefixes ending
	 * before the first and last occurrence */  
	hashtype periodFP, firstFP, lastFP;

	long long mod;
};

/**
 * RTSPM -- real time streaming pattern matching
 * 
 * A class implementing the algorithm processing
 * letters of the patterns and text one by one.
 * All letters of the pattern must have been already 
 * processed before the processing of the text begins.
 * 
 * False positives may occur
 * with probability at most 3*n*m/mod
 * (assuming that the base is chosen uniformly at
 * random from [0, mod-1] and mod is a prime).
 * 
 * If the bases have been chosen independently,
 * then the probabilities of failure of the
 * corresponding algorithms are also independent.
 * Hence, the desired probability of failure can
 * be achieved by running many instances of the 
 * algorithm in parallel.
 * 
 * Memory complexity: O(log m)
 * 
 */
class RTSPM {
public:
	RTSPM(long long _mod = 1e9+7, long long _base = 29)
	{
		mod = _mod;
		base = _base;
		baseInw = fexp(base, mod-2, mod);
		currFP = {0, 1, 1};
	}

	/**
	 * processPattern
	 * process the next letter of the pattern.
	 * Complexity: O(1) (amortized, because of vector usage)
	 */
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

	/**
	 * processText
	 * process the next letter of the text.
	 * returns false if no occurrence of the pattern
	 * ends at the processed symbol and true otherwise.
	 * 
	 * Note that false positives are possible.
	 * 
	 * Complexity: O(log m)
	 */
	bool processText(int a)
	{
		if ((n++) == 0) {
			Lengths.push_back(m);
			Fingerprints.push_back(currFP);

			currFP = {0, 1, 1};
		}

		if (collision) return true;

		bool report = (a % mod == get<0>(Fingerprints[0]));
		hashtype reportFP = currFP;

		appendLetter(currFP, a, base, baseInw, mod);

		for (int i = 0; i<levels; i++)
		{
			bool occurrence;
			hashtype hash, levelFP;

			occurrence = Containers[i].query(n - Lengths[i+1], hash);

			if (occurrence) {
				levelFP =  suffixFingerprint(hash, currFP, mod);
				occurrence = (levelFP == Fingerprints[i+1]);
			}

			if (report) {

				if (not Containers[i].insert(n - Lengths[i], reportFP)) {
					/* A collision has been detected, from now on always
					 * report an occurrence of the pattern. */
					collision = true;
				}
			}

			report = occurrence;
			reportFP = hash;
		}

		return report;
	}

private:
	long long mod, base, baseInw;

	/* Information about the pattern */
	int m = 0, levels = 0;
	
	/* For log (m) pattern's prefixes, their
	 * lengths (powers of 2) and fingerprints are stored.
	 * There is a container for occurrences
	 * of each one of them. */ 
	vector <hashtype> Fingerprints;
	vector <int> Lengths;
	vector <Container> Containers;

	int n = 0;
	hashtype currFP;
	bool collision = false;


};

#ifdef MEMREPORT
extern void memreport();
#endif

int main (int argc, char *argv[])
{
	int mod, base;

	mod = (argc == 1 ? 1e9+7 : atoi(argv[1]));
	base = (argc == 1 ? 31 : atoi(argv[2]));
	
	int n, m;
	scanf ("%d %d", &n, &m);

	RTSPM Solver(mod, base);

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
	}

	#ifdef MEMREPORT
	memreport();
	#endif
}
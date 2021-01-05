#include <iostream>
#include <string>
#include <vector>
using namespace std;

void kmp (string pattern, int n)
{
	vector <int> pi;
	int m = pattern.size();

	pi.push_back(0);
	for (int i = 1; i < m; i++) {
		int nxt = pi[i-1];
		while (pattern[i] != pattern[nxt] and nxt > 0)
			nxt = pi[nxt-1];

		pi.push_back(pattern[i] == pattern[nxt] ? nxt+1 : nxt);
	}

	int nxt = 0;
	for (int i = 0; i < n; i++) {
		char c;
		scanf (" %c", &c);
		while (c != pattern[nxt] and nxt > 0)
			nxt = pi[nxt-1];

		nxt = (c == pattern[nxt] ? nxt+1 : nxt);
		if (nxt == m) {
			printf("%d\n", i - m + 1);
			nxt = pi[nxt-1];
		}
	}
}

#ifdef MEMREPORT
extern void memreport();
#endif

int main ()
{
	int n, m;
	string pattern;
	scanf ("%d %d", &n, &m);

	pattern.resize(m);
	for (int i=0; i < m; i++) 
		scanf (" %c", &pattern[i]);

	kmp(pattern, n);

	#ifdef MEMREPORT
	memreport();
	#endif
}
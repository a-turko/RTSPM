#include <iostream>
#include <string>
#include <vector>
using namespace std;
#define debug(...) fprintf(stderr, __VA_ARGS__)

vector <int> occurrences (string pattern, int n)
{
	vector <int> ans, pi;
	int m = pattern.size();

	pi.push_back(0);
	for (int i = 1; i < m; i++) {
		int nxt = pi[i-1];
		while (pattern[i] != pattern[nxt] and nxt > 0) nxt = pi[nxt-1];

		pi.push_back( pattern[i] == pattern[nxt] ? nxt+1 : nxt);
	}

	debug ("Got pi: ");
	for (int p: pi) debug ("%d ", p);
	debug ("\n");

	int nxt = 0;
	for (int i = 0; i < n; i++) {
		char c;
		cin>>c;
		while (c != pattern[nxt] and nxt > 0) nxt = pi[nxt-1];

		nxt = (c == pattern[nxt] ? nxt+1 : nxt);
		if (nxt == m) {
			ans.push_back(i - m + 1);
			nxt = pi[nxt-1];
		}
	}

	return ans;
}

int main ()
{
	int n, m;
	string text, pattern;
	cin>>n >>m >>pattern;

	vector <int> ans = occurrences(pattern, n);

	for (int x: ans) cout<<x <<"\n";
}
#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;

string fib(int k)
{
	if (k == 0) return "a";
	if (k == 1) return "ab";
	return fib(k-1) + fib(k-2);
}

string fibWord(int k)
{
	string ans, w = fib(5);
	int cnt = 6;
	while ((int)w.size() < k) w = fib(cnt++);
	for (int i=0; i < k; i++) ans.push_back(w[i]);
	return ans;
}

string word(int n, int k)
{
	string res;
	for (int i=0; i<n; i++) res.push_back('a' + rand()%k);
	return res;
}

int main(int argc, char *argv[])
{
	int type = atoi(argv[1]);
	int n = atoi(argv[2]);
	int m = atoi(argv[3]);

	if (type == 0) {
		cout<<n <<" " <<m <<"\n" <<fibWord(m) <<"\n" <<fibWord(n) <<"\n";
	} else {
		cout<<n <<" " <<m <<"\n" <<word(m, type) <<"\n" <<word(n, type) <<"\n";
	}
}
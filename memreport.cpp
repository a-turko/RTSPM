#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include <string>
#include <iostream>
using namespace std;

void memreport()
{
	pid_t pid = getpid();
	char pathname[100];
	sprintf(pathname, "/proc/%u/status", pid);

	FILE *fp = freopen(pathname, "r", stdin);
	assert(fp);

	string line;

	while (getline(cin, line)) cerr<<line <<"\n";
}
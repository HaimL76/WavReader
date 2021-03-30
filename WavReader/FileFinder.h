#pragma once

#include <string>
#include <vector>

using std::string;
using std::wstring;
using std::vector;

class FileFinder
{
private:
	bool verbose;
	int level;

	void Find(wstring& dir, wstring& pattern, vector<wstring>& vec, int lev);

public:
	FileFinder()
	{
		level = 0;
		verbose = false;
	}

	FileFinder(int lev, bool v)
	{
		level = lev;
		verbose = v;
	}

	void Find(wstring& dir, wstring& pattern, vector<wstring>& vec)
	{
		Find(dir, pattern, vec, 0);
	}
};


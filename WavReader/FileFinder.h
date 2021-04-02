#pragma once

#include <string>
#include <vector>

using std::string;
using std::wstring;
using std::vector;

class FileFinder
{
private:
	int verbosity;
	int level;

	void Find(wstring& dir, wstring& pattern, vector<wstring>& vec, int lev);

public:
	FileFinder()
	{
		level = 0;
		verbosity = false;
	}

	FileFinder(int lev, bool v)
	{
		level = lev;
		verbosity = v;
	}

	void Find(wstring& dir, wstring& pattern, vector<wstring>& vec)
	{
		Find(dir, pattern, vec, 0);
	}
};


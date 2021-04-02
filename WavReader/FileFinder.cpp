#include <Windows.h>
#include <iostream>
#include <regex>

#include "FileFinder.h"

using std::wstring;

void FileFinder::Find(wstring& dir, wstring& pattern, vector<wstring>& vec, int lev)
{
    if (dir.empty())
    {
        TCHAR buffer[MAX_PATH] = { 0 };
        GetModuleFileName(NULL, buffer, MAX_PATH);

        wstring wstr = wstring(buffer);

        wstring::size_type pos = wstr.find_last_of(L"\\/");
        dir = wstr.substr(0, pos);
    }

    if (verbosity > 1)
        std::wcout << dir << std::endl;

    WIN32_FIND_DATA ffd;
    //LARGE_INTEGER filesize;
    //TCHAR szDir[MAX_PATH];
    //size_t length_of_arg;
    //HANDLE hFind = INVALID_HANDLE_VALUE;

    HANDLE hFile = FindFirstFileW((dir + L"\\*").c_str(), &ffd);

    bool search = hFile != INVALID_HANDLE_VALUE;

    while (search)
    {
        wstring fileName = wstring(ffd.cFileName);

        if (!fileName._Equal(L".") && !fileName._Equal(L".."))
            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                //dir += L"\\" + fileName;
                //dir = fileName;

                wstring wstr = dir + L"\\" + fileName;

                if (level < 1 || lev < level)
                    Find(wstr, pattern, vec, lev + 1);
            }
            else
            {
                std::wsmatch results;

                if (pattern.empty() || std::regex_match(fileName, results, std::wregex(pattern, std::regex_constants::icase)))
                    vec.push_back(dir + L"\\" + fileName);
            }

        search = FindNextFileW(hFile, &ffd);
    }

    FindClose(hFile);

    DWORD dwError = GetLastError();

    if (dwError != ERROR_NO_MORE_FILES)
    {
        //DisplayErrorBox(TEXT("FindFirstFile"));
    }

    //FindClose(hFind);
    //return dwError;
}
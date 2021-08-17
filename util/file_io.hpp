#ifndef _FILE_IO_HPP_
#define _FILE_IO_HPP_

#include <string>
#include <iostream>
#include <fstream>

namespace util
{

template <class T>
int writeToFile(const std::string& fileName, const T& obj)
{
    using namespace std;
    ofstream fout(fileName);
    if (fout.fail())
    {
        cout << "error: open file" << endl;
        return -1;
    }
    fout << obj << endl;
    fout.close();
    return 0;
}


template <class T>
int readFromFile(const std::string& fileName, T& obj)
{
    using namespace std;
    ifstream fin(fileName);
    if (fin.fail())
    {
        cout << "error: open file" << endl;
        return -1;
    }
    fin >> obj;
    fin.close();
    return 0;
}

} //namespace util

#endif


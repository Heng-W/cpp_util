#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <cstddef>
#include <string>
#include <iostream>
#include <vector>


void getFiles(std::string path, std::vector<std::string>& files)
{
    DIR* dir;
    struct dirent* ptr;

    if ((dir = opendir(path.c_str())) == NULL)
    {
        perror("Open dir error...");
        return;
    }

    while ((ptr = readdir(dir)) != NULL)
    {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
            continue;
        else if (ptr->d_type == 8) //-
            files.push_back(path + ptr->d_name);
        else if (ptr->d_type == 10) //l
            continue;
        else if (ptr->d_type == 4) //d
        {
            // files.push_back(ptr->d_name);
            getFiles(path + ptr->d_name + "/", files);
        }
    }
    closedir(dir);
}



#ifdef TEST_MAIN

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "please input 2 args" << std::endl;
        return 1;
    }
    std::vector<std::string> files;
    getFiles(argv[1], files);
    size_t len = files.size();
    for (size_t i = 0; i < len; i ++)
    {
        std::cout << files[i] << std::endl;
    }

    return 0;
}

#endif

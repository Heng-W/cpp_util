#include "md5.h"
#include <stdio.h>
#include <string.h>
#include <string>
using namespace std;

void convertUnCharToStr(char* str, unsigned char* UnChar, int ucLen)
{
    int i = 0;
    for (i = 0; i < ucLen; i++)
    {
        //格式化输str,每unsigned char 转换字符占两位置%x写输%X写输
        sprintf(str + i * 2, "%02x", UnChar[i]);
    }
}

int main()
{
    unsigned char content[256];
    unsigned char decrypt[16];
    int i;

    while (scanf("%s", content))
    {
        MD5_CTX md5;
        MD5Init(&md5);
        MD5Update(&md5, content, strlen((char*)content));
        MD5Final(&md5, decrypt);
        printf("加密前:%s\n加密后32位:", content);
        for (i = 0; i < 16; i++)
        {
            printf("%02x", decrypt[i]);
        }
        printf("\n加密后16位：");
        for (int i = 4; i < 12; i++)
        {
            printf("%02x", decrypt[i]);
        }
        char result[32];
        printf("\n转换char *的结果：");
        convertUnCharToStr(result, decrypt, 16);

        printf("%s", result);
        printf("\n");
    }
}

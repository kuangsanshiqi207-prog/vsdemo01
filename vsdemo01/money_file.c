#include"money_file.h"

int saveMoney(const Money* money, const char* pPath)
{

    FILE* fp = fopen(pPath, "ab");  // 二进制追加模式
    if (fp == NULL) {
        return FALSE;
    }

    size_t written = fwrite(money, sizeof(Money), 1, fp);
    fclose(fp);

    return (written == 1) ? TRUE : FALSE;
}
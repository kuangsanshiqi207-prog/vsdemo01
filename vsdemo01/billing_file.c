#include"billing_file.h"

int saveBilling(const Billing* pBilling, const char* pPath)
{
    FILE* fp = fopen(pPath, "ab");  // 二进制追加模式
    if (fp == NULL) {
        return FALSE;
    }

    size_t written = fwrite(pBilling, sizeof(Billing), 1, fp);
    fclose(fp);

    return (written == 1) ? TRUE : FALSE;
}
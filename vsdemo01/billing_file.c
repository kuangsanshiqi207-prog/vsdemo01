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

// 在 billing_file.c 末尾添加以下函数

/**
 * 获取二进制计费文件中的记录数量
 * @param pPath 文件路径
 * @return 记录数，文件打开失败返回 0
 */
int getBillingCount(const char* pPath) {
    FILE* fp = fopen(pPath, "rb");
    if (fp == NULL) {
        return 0;
    }
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fclose(fp);
    return (int)(size / sizeof(Billing));
}

/**
 * 读取计费文件中的所有记录到数组
 * @param pBilling 指向数组的指针（需提前分配足够空间）
 * @param pPath 文件路径
 * @return 实际读取的记录数，失败返回 -1
 */
int readBilling(Billing* pBilling, const char* pPath) {
    FILE* fp = fopen(pPath, "rb");
    if (fp == NULL) {
        return -1;
    }
    int count = 0;
    while (fread(&pBilling[count], sizeof(Billing), 1, fp) == 1) {
        count++;
    }
    fclose(fp);
    return count;
}

/**
 * 按索引更新二进制计费文件中的一条记录（索引从 1 开始）
 * @param pBilling 新的计费信息指针
 * @param pPath 文件路径
 * @param nIndex 要更新的记录索引（1-based）
 * @return TRUE 成功，FALSE 失败
 */
int updateBilling(const Billing* pBilling, const char* pPath, int nIndex) {
    if (nIndex < 1) return FALSE;
    FILE* fp = fopen(pPath, "rb+");
    if (fp == NULL) return FALSE;

    // 定位到第 nIndex 条记录
    long offset = (long)(nIndex - 1) * sizeof(Billing);
    if (fseek(fp, offset, SEEK_SET) != 0) {
        fclose(fp);
        return FALSE;
    }

    size_t written = fwrite(pBilling, sizeof(Billing), 1, fp);
    fclose(fp);
    return (written == 1) ? TRUE : FALSE;
}

#include "card_file.h"
#include <stdlib.h>   // 为 NULL 定义

/**
 * 全量保存链表中的所有卡片到文件（覆盖写入）
 * @param head 链表头指针（带头结点）
 * @param path 文件路径
 * @return 0 成功，-1 失败
 */
int saveCard(const CardList* head, const char* path) {
    if (head == NULL) return -1;
    FILE* fp = fopen(path, "wb");   // 二进制写入，覆盖
    if (fp == NULL) return -1;

    CardList* p = head->next;       // 跳过虚拟头结点
    while (p != NULL) {
        if (fwrite(&p->card, sizeof(Card), 1, fp) != 1) {
            fclose(fp);
            return -1;
        }
        p = p->next;
    }
    fclose(fp);
    return 0;
}

/**
 * 追加一条卡片记录到文件末尾
 * @param pCard 卡片结构体指针
 * @param path 文件路径
 * @return 1 成功，0 失败
 */
int appendCard(const Card* pCard, const char* path) {
    if (pCard == NULL) return 0;
    FILE* fp = fopen(path, "ab");   // 二进制追加
    if (fp == NULL) return 0;
    size_t written = fwrite(pCard, sizeof(Card), 1, fp);
    fclose(fp);
    return (written == 1) ? 1 : 0;
}

/**
 * 获取二进制文件中卡片记录数量
 * @param pPath 文件路径
 * @return 记录数，文件打开失败返回 0
 */
int getCardCount(const char* pPath) {
    FILE* fp = fopen(pPath, "rb");
    if (fp == NULL) return 0;
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fclose(fp);
    return (int)(size / sizeof(Card));
}

/**
 * 读取所有卡片记录到数组（需预先分配足够空间）
 * @param pCard 数组指针
 * @param pPath 文件路径
 * @return 实际读取的记录数，失败返回 -1
 */
int readCard(Card* pCard, const char* pPath) {
    if (pCard == NULL) return -1;
    FILE* fp = fopen(pPath, "rb");
    if (fp == NULL) return -1;

    int count = 0;
    while (fread(&pCard[count], sizeof(Card), 1, fp) == 1) {
        count++;
    }
    fclose(fp);
    return count;
}

/**
 * 按索引更新一条卡片记录（索引从 1 开始）
 * @param pCard 新卡片数据指针
 * @param pPath 文件路径
 * @param nIndex 1‑based 索引
 * @return 1 成功，0 失败
 */
int updateCardFile(const Card* pCard, const char* pPath, int nIndex) {
    if (pCard == NULL || nIndex < 1) return 0;
    FILE* fp = fopen(pPath, "rb+");
    if (fp == NULL) return 0;

    long offset = (long)(nIndex - 1) * sizeof(Card);
    if (fseek(fp, offset, SEEK_SET) != 0) {
        fclose(fp);
        return 0;
    }
    size_t written = fwrite(pCard, sizeof(Card), 1, fp);
    fclose(fp);
    return (written == 1) ? 1 : 0;
}

/* ========== 以下函数（parseCard）已不再需要，可删除或保留供数据迁移使用 ========== */
/*
Card parseCard(char* pBuf) {
    // 二进制存储不再需要文本解析，此函数可废弃
    Card card = {0};
    return card;
}
*/
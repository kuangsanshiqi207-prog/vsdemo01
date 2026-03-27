#include"card_file.h"

/**
 * 将链表中的所有卡片信息保存到指定文件
 * @param head 链表头指针
 * @param path 文件路径
 * @return 0 成功，-1 失败
 */
int saveCard(const CardList* head, const char* path) {
    FILE* fp = fopen(path, "a");
    if (fp == NULL) {
        return -1;
    }

    const CardList* p = head;
    while (p != NULL) {
        const Card* c = &p->card;
        char startTimeStr[20];
        char endTimeStr[20];
        char lastTimeStr[20];

        format_time(startTimeStr, sizeof(startTimeStr), c->tStart);
        format_time(endTimeStr, sizeof(endTimeStr), c->tEnd);
        format_time(lastTimeStr, sizeof(lastTimeStr), c->tLast);

        // 固定宽度输出，无前导空格
        fprintf(fp, "%18s##%8s##%2d##%19s##%19s##%8.2f##%19s##%5d##%8.2f##%2d\n",
            c->aName, c->aPwd, c->nStatus,
            startTimeStr, endTimeStr, c->fTotalUse,
            lastTimeStr, c->nUseCount, c->fBalance, c->nDel);

        p = p->next;
    }

    fclose(fp);
    return 0;
}


int getCardCount(const char* pPath) {
    FILE* fp = fopen(pPath, "r");
    if (fp == NULL) {
        return 0;  // 文件打开失败，返回0
    }
    int count = 0;
    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL) {
        // 跳过空行
        if (strlen(line) > 1) {  // 至少有一个字符（不含换行符）
            count++;
        }
    }
    fclose(fp);
    return count;
}

int readCard(Card* pCard, const char* pPath) {
    FILE* fp = fopen(pPath, "r");
    if (fp == NULL) {
        return 0;  // 打开失败，返回 FALSE
    }
    int index = 0;
    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL) {
        // 跳过空行
        if (strlen(line) <= 1) {
            continue;
        }
        // 去除末尾换行符
        line[strcspn(line, "\n")] = '\0';
        // 解析一行，得到 Card 结构体
        Card card = parseCard(line);
        // 复制到数组
        pCard[index++] = card;
    }
    fclose(fp);
    return (index > 0) ? 1 : 0;  // 至少读取一条记录返回 TRUE，否则 FALSE
}

Card parseCard(char* pBuf) {
    Card card = { 0 };
    char name[19] = { 0 };
    char pwd[9] = { 0 };
    int status;
    char start_time[20] = { 0 };
    char end_time[20] = { 0 };
    float total_use;
    char last_time[20] = { 0 };
    int use_count;
    float balance;
    int del;

    // 使用 %[^#] 读取到分隔符 '#'，避免时间字符串中的空格导致截断
    if (sscanf(pBuf, "%18[^#]##%8[^#]##%d##%19[^#]##%19[^#]##%f##%19[^#]##%d##%f##%d",
        name, pwd, &status,
        start_time, end_time, &total_use,
        last_time, &use_count, &balance, &del) == 10) {

        // 去除字段前可能的空格（固定宽度左补空格，所以去除前导空格）
        char* p = name;
        while (*p == ' ') p++;
        strncpy(card.aName, p, sizeof(card.aName) - 1);
        card.aName[sizeof(card.aName) - 1] = '\0';

        p = pwd;
        while (*p == ' ') p++;
        strncpy(card.aPwd, p, sizeof(card.aPwd) - 1);
        card.aPwd[sizeof(card.aPwd) - 1] = '\0';

        card.nStatus = status;
        card.tStart = parse_time(start_time);
        card.tEnd = parse_time(end_time);
        card.fTotalUse = total_use;
        card.tLast = parse_time(last_time);
        card.nUseCount = use_count;
        card.fBalance = balance;
        card.nDel = del;
    }
    return card;
}

int updateCard(const Card* pCard, const char* pPath, int nIndex) {
    nIndex -= 1;
    FILE* fp = fopen(pPath, "rb+");          // 二进制读写模式，确保字节定位准确
    if (fp == NULL) {
        return FALSE;                        // 文件打开失败
    }

    // 计算并定位到第 nIndex 行的起始偏移
    long offset = (long)nIndex * CARD_LINE_SIZE;
    if (fseek(fp, offset, SEEK_SET) != 0) {
        fclose(fp);
        return FALSE;                        // 定位失败（索引超出范围等）
    }

    // 格式化时间字段
    char startTimeStr[20], endTimeStr[20], lastTimeStr[20];
    format_time(startTimeStr, sizeof(startTimeStr), pCard->tStart);
    format_time(endTimeStr, sizeof(endTimeStr), pCard->tEnd);
    format_time(lastTimeStr, sizeof(lastTimeStr), pCard->tLast);

    // 生成新行字符串（必须与 saveCard 格式完全一致）
    char newLine[CARD_LINE_SIZE + 1];        // 多留一个字节用于检查长度
    int written = snprintf(newLine, sizeof(newLine),
        "%18s##%8s##%2d##%19s##%19s##%8.2f##%19s##%5d##%8.2f##%2d\r\n",
        pCard->aName, pCard->aPwd, pCard->nStatus,
        startTimeStr, endTimeStr, pCard->fTotalUse,
        lastTimeStr, pCard->nUseCount, pCard->fBalance, pCard->nDel);

    // 验证生成的行长度是否与预期一致
    if (written != CARD_LINE_SIZE) {
        fclose(fp);
        return FALSE;                        // 长度不匹配（可能字段超宽或平台换行符问题）
    }

    // 覆盖写入整行数据
    if (fwrite(newLine, 1, CARD_LINE_SIZE, fp) != CARD_LINE_SIZE) {
        fclose(fp);
        return FALSE;                        // 写入失败
    }

    fclose(fp);
    return TRUE;                             // 更新成功
}
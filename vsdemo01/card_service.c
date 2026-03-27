#include "card_service.h"


// 全局变量
CardList* cardlist;          // 带头结点的链表头指针
int g_cardCount = 0;         // 当前卡片数量（用于容量限制）

// 初始化链表，创建头结点
void initCard() {
    // 1. 创建头结点（虚拟头结点）
    CardList* dummy = (CardList*)malloc(sizeof(CardList));
    if (dummy == NULL) {
        printf("内存不足，初始化失败\n");
        return;
    }
    dummy->next = NULL;
    cardlist = dummy;

    // 2. 获取文件中卡片数量
    g_cardCount = getCardCount(userpath);
    if (g_cardCount == 0) {
        // 文件为空或不存在，链表仅有头结点
        return;
    }

    // 3. 打开文件，逐行读取并构建链表
    FILE* fp = fopen(userpath, "r");
    if (fp == NULL) {
        printf("无法打开卡片文件: %s\n", userpath);
        return;
    }

    char line[256];
    CardList* tail = dummy;   // 尾指针，用于 O(1) 插入
    int actualCount = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        // 跳过空行
        if (strlen(line) <= 1) {
            continue;
        }
        // 去掉末尾换行符
        line[strcspn(line, "\n")] = '\0';

        // 利用已有的 parseCard 函数解析一行，得到 Card 结构体
        Card card = parseCard(line);

        // 创建新节点
        CardList* newNode = (CardList*)malloc(sizeof(CardList));
        if (newNode == NULL) {
            printf("内存不足，创建节点失败\n");
            break;
        }
        newNode->card = card;
        newNode->next = NULL;

        // 插入链表尾部
        tail->next = newNode;
        tail = newNode;
        actualCount++;
    }

    fclose(fp);

    // 4. 如果实际读取数量与计数不一致，更新 g_cardCount（可选）
    if (actualCount != g_cardCount) {
        g_cardCount = actualCount;
        printf("警告: 文件实际卡片数量与计数不一致 (实际 %d，计数 %d)\n",
            actualCount, g_cardCount);
    }
}

// 添加卡片（插入到链表尾部）
int addCard(Card* newCard)
{
    if (newCard == NULL) {
        return 0;
    }

    int flag = 0;
    queryCard(newCard->aName, &flag);

    if (flag == 1)
    {
        return 0;
    }

    // 容量检查
    if (g_cardCount >= MAX_SIZE) {
        fprintf(stderr, "addCard: card storage full (MAX_SIZE=%d)\n", MAX_SIZE);
        return 0;
    }


    // 创建新节点
    CardList* newNode = (CardList*)malloc(sizeof(CardList));
    if (newNode == NULL) {
        printf("空间不足\n");
        return 0;
    }
    copyCard(&newNode->card, newCard);
    newNode->next = NULL;

    // 找到链表尾部（头结点不存数据）
    CardList* tail = cardlist;
    while (tail->next != NULL) {
        tail = tail->next;
    }
    tail->next = newNode;     // 将新节点链接到末尾

    saveCard(newNode, userpath);
    g_cardCount++;            // 卡片数增加
    return 1;
}


CardList* queryCard(char* name, int* flag)
{
    if (name == NULL) {
        if (flag) *flag = -1;
        return NULL;
    }

    // 1. 精确匹配
    CardList* curr = cardlist->next;   // 跳过表头
    while (curr != NULL) {
        if (strcmp(curr->card.aName, name) == 0) {
            // 分配新节点并拷贝数据
            CardList* result = (CardList*)malloc(sizeof(CardList));
            if (result == NULL) {
                if (flag) *flag = -1;
                return NULL;
            }
            copyCard(&result->card, &curr->card);
            result->next = NULL;
            if (flag) *flag = 1;      // 精确匹配
            return result;
        }
        curr = curr->next;
    }

    // 2. 模糊匹配：收集所有包含 name 的卡片
    CardList* head = NULL;   // 结果链表的头
    CardList* tail = NULL;   // 结果链表的尾
    curr = cardlist->next;
    int matchCount = 0;

    while (curr != NULL) {
        if (strstr(curr->card.aName, name) != NULL) {
            // 为新匹配的卡片分配节点
            CardList* newNode = (CardList*)malloc(sizeof(CardList));
            if (newNode == NULL) {
                // 内存分配失败，清理已分配的节点并返回 NULL
                while (head != NULL) {
                    CardList* tmp = head;
                    head = head->next;
                    free(tmp);
                }
                if (flag) *flag = -1;
                return NULL;
            }
            copyCard(&newNode->card, &curr->card);
            newNode->next = NULL;

            // 链接到结果链表尾部
            if (head == NULL) {
                head = newNode;
                tail = newNode;
            }
            else {
                tail->next = newNode;
                tail = newNode;
            }
            matchCount++;
        }
        curr = curr->next;
    }

    if (matchCount == 0) {
        if (flag) *flag = -1;
        return NULL;
    }

    if (flag) *flag = 0;   // 模糊匹配
    return head;
}

// 拷贝卡片数据（确保字符串安全）
void copyCard(Card* dest, Card* src)
{
    if (dest == NULL || src == NULL) {
        return;
    }

    // 安全拷贝字符串
    strncpy(dest->aName, src->aName, sizeof(dest->aName) - 1);
    dest->aName[sizeof(dest->aName) - 1] = '\0';

    strncpy(dest->aPwd, src->aPwd, sizeof(dest->aPwd) - 1);
    dest->aPwd[sizeof(dest->aPwd) - 1] = '\0';

    // 拷贝其他字段
    dest->nStatus = src->nStatus;
    dest->tStart = src->tStart;
    dest->tEnd = src->tEnd;
    dest->fTotalUse = src->fTotalUse;
    dest->tLast = src->tLast;
    dest->nUseCount = src->nUseCount;
    dest->fBalance = src->fBalance;
    dest->nDel = src->nDel;
}

// 释放整个链表
void freeCard()
{
    CardList* p = cardlist;
    while (p != NULL) {
        CardList* next = p->next;
        free(p);
        p = next;
    }
    cardlist = NULL;      // 避免悬空指针
    g_cardCount = 0;
}

// 释放 queryCard 返回的链表（每个节点都是 malloc 的）
void freeQueryResult(CardList* result)
{
    while (result != NULL) {
        CardList* next = result->next;
        free(result);
        result = next;
    }
}

Card* checkAndUpdateCard(const char* name, const char* pPwd, int* pIndex)
{
    CardList* curr = cardlist->next;   // 跳过表头
    CardList* result = NULL;
    while (curr != NULL) {
        *pIndex+=1;
        if (strcmp(curr->card.aName, name) == 0) {
           
            result = (CardList*)malloc(sizeof(CardList));
            if (result == NULL) {
                *pIndex = -1;
                return NULL;
            }
            copyCard(&result->card, &curr->card);
            result->next = NULL;
            break;
        }
        curr = curr->next;
    }
    if (curr == NULL)
    {
        return NULL;
    }
    if (!strcmp(result->card.aPwd, pPwd) == 0)
    {
        return NULL;
    }
    if (result->card.nStatus != 0 || result->card.fBalance <= 0)
    {
        return NULL;
    }
    result->card.nStatus = 1;
    copyCard(&curr->card, &result->card);
    updateCard(&result->card, userpath,*pIndex);
    return result;
}
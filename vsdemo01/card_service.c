#include "card_service.h"


// 全局变量
CardList* cardlist;          // 带头结点的链表头指针
int g_cardCount = 0;         // 当前卡片数量（用于容量限制）

void initCard() {
    // 创建头结点
    CardList* dummy = (CardList*)malloc(sizeof(CardList));
    if (dummy == NULL) {
        printf("内存不足，初始化失败\n");
        return;
    }
    dummy->next = NULL;
    cardlist = dummy;

    // 获取记录数
    g_cardCount = getCardCount(userpath);
    if (g_cardCount == 0) return;

    // 分配数组并读取所有记录
    Card* arr = (Card*)malloc(g_cardCount * sizeof(Card));
    if (arr == NULL) {
        printf("内存不足，读取卡片数据失败\n");
        return;
    }
    int readCnt = readCard(arr, userpath);
    if (readCnt <= 0) {
        free(arr);
        return;
    }

    // 构建链表
    CardList* tail = dummy;
    for (int i = 0; i < readCnt; i++) {
        CardList* newNode = (CardList*)malloc(sizeof(CardList));
        if (newNode == NULL) break;
        newNode->card = arr[i];
        newNode->next = NULL;
        tail->next = newNode;
        tail = newNode;
    }
    free(arr);
}

int addCard(Card* newCard) {
    if (newCard == NULL) return 0;

    int flag = 0;
    queryCard(newCard->aName, &flag);
    if (flag == 1) return 0;   // 卡号已存在

    if (g_cardCount >= MAX_SIZE) {
        fprintf(stderr, "addCard: card storage full (MAX_SIZE=%d)\n", MAX_SIZE);
        return 0;
    }

    // 创建链表节点
    CardList* newNode = (CardList*)malloc(sizeof(CardList));
    if (newNode == NULL) return 0;
    copyCard(&newNode->card, newCard);
    newNode->next = NULL;

    // 追加到文件（二进制追加）
    if (appendCard(&newNode->card, userpath) == 0) {
        free(newNode);
        return 0;
    }

    // 插入链表尾部
    CardList* tail = cardlist;
    while (tail->next != NULL) tail = tail->next;
    tail->next = newNode;

    g_cardCount++;
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

    // 2. 模糊匹配:收集所有包含 name 的卡片
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

Card* checkCard(const char* name, const char* pPwd, int* pIndex)
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
    //copyCard(&curr->card, &result->card);
    //updateCardFile(&result->card, userpath,*pIndex);
    return &result->card;
}

// 在 card_service.c 末尾添加

CardList* findCardForSettle(const char* name, const char* pwd, int* pIndex) {
    if (name == NULL || pwd == NULL) return NULL;
    CardList* curr = cardlist->next;
    int idx = 1;
    while (curr != NULL) {
        if (strcmp(curr->card.aName, name) == 0) {
            // 检查密码
            if (strcmp(curr->card.aPwd, pwd) != 0) {
                if (pIndex) *pIndex = -1;
                return NULL;
            }
            // 检查状态是否为正在上机、未删除、未失效、余额大于0、未过期
            if (curr->card.nStatus != 1 || curr->card.nDel != 0) {
                if (pIndex) *pIndex = -2;   // 状态不允许下机
                return NULL;
            }
            if (curr->card.fBalance <= 0) {
                if (pIndex) *pIndex = -3;   // 余额不足（下机时还会再算）
                return NULL;
            }
            // 检查有效期
            time_t now = time(NULL);
            if (now > curr->card.tEnd) {
                if (pIndex) *pIndex = -4;   // 卡已过期
                return NULL;
            }
            if (pIndex) *pIndex = idx;
            return curr;   // 返回链表中的节点指针
        }
        idx++;
        curr = curr->next;
    }
    return NULL;
}

int updateCard(Card* card,int index)
{
    int i = index;
    CardList* curr = cardlist;
    while (i--)
    {
        curr = curr->next;
    }
    copyCard(&curr->card,card);
    if (updateCardFile(card, userpath, index) == 0)
    {
        return FALSE;
    }
    return TRUE;
}
#include "card_service.h"

#define MAX_SIZE 50

// 全局变量
CardList* cardlist;          // 带头结点的链表头指针
int g_cardCount = 0;         // 当前卡片数量（用于容量限制）

// 初始化链表，创建头结点
void initCard()
{
    CardList* dummy = (CardList*)malloc(sizeof(CardList));
    if (dummy == NULL) {
        printf("空间不足，初始化失败\n");
        return;
    }
    dummy->next = NULL;       // 重要：头结点的 next 必须初始化为 NULL
    cardlist = dummy;
    g_cardCount = 0;          // 初始卡片数为 0
}

// 添加卡片（插入到链表尾部）
void addCard(Card* newCard)
{
    if (newCard == NULL) {
        return;
    }

    // 容量检查
    if (g_cardCount >= MAX_SIZE) {
        fprintf(stderr, "addCard: card storage full (MAX_SIZE=%d)\n", MAX_SIZE);
        return;
    }

    // 创建新节点
    CardList* newNode = (CardList*)malloc(sizeof(CardList));
    if (newNode == NULL) {
        printf("空间不足\n");
        return;
    }
    copyCard(&newNode->card, newCard);
    newNode->next = NULL;

    // 找到链表尾部（头结点不存数据）
    CardList* tail = cardlist;
    while (tail->next != NULL) {
        tail = tail->next;
    }
    tail->next = newNode;     // 将新节点链接到末尾

    g_cardCount++;            // 卡片数增加
}

// 查询卡片：
//   精确匹配：返回包含该卡片拷贝的单节点链表，*flag = 1
//   模糊匹配：返回所有包含子串的卡片组成的链表（按原顺序），*flag = 0
//   无匹配：返回 NULL，*flag = -1
//   注意：返回的链表需要调用者使用 freeQueryResult() 释放（或手动遍历释放）
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
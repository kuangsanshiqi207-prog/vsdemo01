#include"billing_service.h"

int InitBilling(Billing* billing)
{
	billing->fAmount = 0;
	billing->nDel = 0;
	billing->nStatus = 0;
	billing->tEnd = 0;
    return 1;
}


// 在 billing_service.c 开头添加全局变量
BillingList* billinglist = NULL;   // 带头结点的链表

//初始化计费链表:从文件读取所有记录构建链表
void initBillingList() {
    // 创建头结点
    BillingList* dummy = (BillingList*)malloc(sizeof(BillingList));
    if (dummy == NULL) {
        printf("内存不足，初始化计费链表失败\n");
        return;
    }
    dummy->next = NULL;
    billinglist = dummy;

    int count = getBillingCount(billingpath);
    if (count == 0) return;

    Billing* arr = (Billing*)malloc(count * sizeof(Billing));
    if (arr == NULL) {
        printf("内存不足，读取计费记录失败\n");
        return;
    }

    int readCount = readBilling(arr, billingpath);
    if (readCount <= 0) {
        free(arr);
        return;
    }

    BillingList* tail = dummy;
    for (int i = 0; i < readCount; i++) {
        BillingList* newNode = (BillingList*)malloc(sizeof(BillingList));
        if (newNode == NULL) break;
        newNode->billing = arr[i];
        newNode->next = NULL;
        tail->next = newNode;
        tail = newNode;
    }
    free(arr);
}


// 释放计费链表
void releaseBillingList() {
    BillingList* p = billinglist;
    while (p != NULL) {
        BillingList* next = p->next;
        free(p);
        p = next;
    }
    billinglist = NULL;
}

// 查找指定卡号的状态为 0（未结算）的计费记录，返回节点指针，并返回索引（1-based）
BillingList* findActiveBilling(const char* cardName, int* pIndex) {
    if (billinglist == NULL || cardName == NULL) return NULL;
    BillingList* p = billinglist->next;
    int idx = 1;
    while (p != NULL) {
        if (p->billing.nStatus == 0 && strcmp(p->billing.aCardName, cardName) == 0) {
            if (pIndex) *pIndex = idx;
            return p;
        }
        idx++;
        p = p->next;
    }
    return NULL;
}

// 修改 addBilling 函数，使其同时向文件和链表添加记录
int addBilling(Billing* billing) {
    // 保存到文件
    if (saveBilling(billing, billingpath) == FALSE) {
        return FALSE;
    }
    // 插入到链表尾部
    if (billinglist == NULL) {
        // 链表未初始化，先初始化
        initBillingList();
    }
    BillingList* newNode = (BillingList*)malloc(sizeof(BillingList));
    if (newNode == NULL) return FALSE;
    newNode->billing = *billing;
    newNode->next = NULL;

    // 找到尾节点
    BillingList* tail = billinglist;
    while (tail->next != NULL) tail = tail->next;
    tail->next = newNode;

    return TRUE;
}
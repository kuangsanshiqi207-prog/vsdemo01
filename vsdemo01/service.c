#include"service.h"
#include <math.h>  

int doLogon(const char* pName, const char* pPwd, LogonInfo* pInfo) {
    int index = 0;
    Card* pCard = checkCard(pName, pPwd, &index);
    if (pCard == NULL) return FALSE;
    if (pCard->nStatus != 0) return UNUSE;
    if (pCard->fBalance <= 0) return ENOUGHMONEY;
    if (time(NULL) > pCard->tEnd) return UNUSE;

    // 修改卡状态
    pCard->nStatus = 1;

    // 将卡状态更新到文件（并同步链表，updateCard内部会调用copyCard）
    if (updateCard(pCard, index) == FALSE) {
        pCard->nStatus = 0; // 回滚
        return FALSE;
    }

    // 填充上机信息
    strcpy(pInfo->aCardName, pCard->aName);
    pInfo->fBalance = pCard->fBalance;
    pInfo->tLogon = time(NULL);

    // 创建计费记录（未结算）
    Billing* billing = (Billing*)malloc(sizeof(Billing));
    if (billing == NULL) return FALSE;
    InitBilling(billing);
    billing->tStart = pInfo->tLogon;
    strcpy(billing->aCardName, pInfo->aCardName);
    addBilling(billing);
    free(billing);

    return TRUE;
}


/**
 * 计算消费金额
 * @param tStart 上机时间
 * @param tEnd   下机时间
 * @return 消费金额（元）
 */
 // 在 service.c 开头添加
double getAmount(time_t tStart, time_t tEnd) {
    double minutes = difftime(tEnd, tStart) / 60.0;
    if (minutes <= 0) return 0.0;
    int units = (int)ceil(minutes / g_config.unitMinutes);
    return units * g_config.charge;
}

/**
 * 下机操作
 * @param pName 卡号
 * @param pPwd  密码
 * @param pInfo 输出下机信息
 * @return 1-成功，0-失败（卡号密码错/未上机），2-卡状态不允许，3-余额不足
 */
int doSettle(const char* pName, const char* pPwd, SettleInfo* pInfo) {
    if (pName == NULL || pPwd == NULL || pInfo == NULL) return FALSE;

    // 1. 查找卡信息（必须处于上机状态）
    int cardIndex = 0;
    CardList* cardNode = findCardForSettle(pName, pPwd, &cardIndex);
    if (cardNode == NULL) {
        if (cardIndex == -2) return UNUSE;      // 卡状态不对
        if (cardIndex == -3) return ENOUGHMONEY;// 余额不足
        return FALSE;                           // 卡号密码错或未上机
    }

    // 2. 查找该卡未结算的计费记录（上机记录）
    int billingIndex = 0;
    BillingList* billingNode = findActiveBilling(pName, &billingIndex);
    if (billingNode == NULL) {
        return FALSE;   // 没有上机记录，无法下机
    }

    // 3. 获取当前时间作为下机时间
    time_t now = time(NULL);
    // 4. 计算消费金额
    double amount = getAmount(billingNode->billing.tStart, now);
    // 5. 检查余额是否足够
    if (cardNode->card.fBalance < amount) {
        return ENOUGHMONEY;
    }

    // 6. 更新卡信息
    Card updatedCard = cardNode->card;
    updatedCard.fBalance -= (float)amount;
    updatedCard.fTotalUse += (float)amount;
    updatedCard.nUseCount++;
    updatedCard.tLast = now;
    updatedCard.nStatus = 0;   // 改为未上机

    // 更新文件中的卡信息
    if (updateCard(&updatedCard, cardIndex) == FALSE) {
        return FALSE;
    }
    // 同步更新链表中的节点
    cardNode->card = updatedCard;

    // 7. 更新计费记录
    Billing updatedBilling = billingNode->billing;
    updatedBilling.tEnd = now;
    updatedBilling.fAmount = (float)amount;
    updatedBilling.nStatus = 1;   // 已结算
    if (updateBilling(&updatedBilling, billingpath, billingIndex) == FALSE) {
        // 注意:卡信息已经更新，这里如果计费更新失败可能需要回滚，简单处理返回失败
        // 实际可考虑回滚，但为简化先返回失败
        return FALSE;
    }
    // 同步链表
    billingNode->billing = updatedBilling;

    // 8. 填充输出信息
    strcpy(pInfo->aCardName, updatedCard.aName);
    pInfo->tStart = billingNode->billing.tStart;
    pInfo->tEnd = now;
    pInfo->fAmount = (float)amount;
    pInfo->fBalance = updatedCard.fBalance;

    return TRUE;
}

int doAddMoney(const char* pName, const char* pPwd, Money* money)
{
    if (pName == NULL || pPwd == NULL || money->fMoney <= 0 ) {
        return FALSE;
    }

    int index = 0;
    Card* pCard  = checkCard(pName, pPwd, &index);
    if (pCard == NULL) {
        return FALSE;  // 卡号密码错误
    }
    
    // 检查卡状态:只有未上机(0)或正在上机(1)可以充值（未注销且未失效）
    if (pCard->nStatus == 2 || pCard->nStatus == 3) {
        return FALSE;  // 已注销或失效
    }

    if (pCard->fBalance + money->fMoney > MAX_BALANCE) {
        return FALSE;   // 充值失败，超过上限
    }

    // 更新余额
    float oldBalance = pCard->fBalance;
    pCard->fBalance += money->fMoney;

    // 更新文件中的卡信息
    if (updateCard(pCard, index) == FALSE) {
        // 回滚内存中的余额
        pCard->fBalance = oldBalance;
        return FALSE;
    }


    // 保存充值记录
    Money moneyRecord = { 0 };
    strcpy(moneyRecord.aCardName, pCard->aName);
    moneyRecord.tTime = time(NULL);
    moneyRecord.nStatus = 0;   // 0-充值
    moneyRecord.fMoney = money->fMoney;
    moneyRecord.nDel = 0;
    if (addMoney(&moneyRecord) == FALSE) {
        // 记录保存失败不影响充值结果，但可以打印日志（略）
    }

    // 填充返回信息
    strcpy(money->aCardName, pCard->aName);
    money->tTime = moneyRecord.tTime;
    money->nStatus = 0;
    money->fMoney = pCard->fBalance;

    return TRUE;
}

// 退费（退还全部余额，卡余额清零，卡状态不变）
int doRefundMoney(const char* pName, const char* pPwd, Money* pMoneyInfo) 
{
    if (pName == NULL || pPwd == NULL || pMoneyInfo == NULL) {
        return FALSE;
    }

    int index = 0;
    Card* pCard = checkCard(pName, pPwd, &index);
    if (pCard == NULL) {
        return FALSE;  // 卡号密码错误
    }

    // 退费条件:未上机(0) 且 余额大于0
    if (pCard->nStatus != 0 || pCard->fBalance <= 0) {
        return FALSE;
    }

    float refundAmount = pCard->fBalance;
    // 清零余额
    pCard->fBalance = 0.0f;

    // 更新文件
    if (updateCard(pCard, index) == FALSE) {
        // 回滚
        pCard->fBalance = refundAmount;
        return FALSE;
    }

    // 保存退费记录
    Money moneyRecord = { 0 };
    strcpy(moneyRecord.aCardName, pCard->aName);
    moneyRecord.tTime = time(NULL);
    moneyRecord.nStatus = 1;   // 1-退费
    moneyRecord.fMoney = refundAmount;
    moneyRecord.nDel = 0;
    addMoney(&moneyRecord);

    // 填充返回信息
    strcpy(pMoneyInfo->aCardName, pCard->aName);
    pMoneyInfo->tTime = moneyRecord.tTime;
    pMoneyInfo->nStatus = 1;
    pMoneyInfo->fMoney = refundAmount;

    return TRUE;
}

int doCancelCard(const char* pName, const char* pPwd, Money* pMoneyInfo) {
    if (pName == NULL || pPwd == NULL || pMoneyInfo == NULL) {
        return FALSE;
    }

    int index = 0;
    Card* pCard = checkCard(pName, pPwd, &index);
    if (pCard == NULL) {
        return FALSE;  // 卡号密码错误
    }

    // 注销条件:未上机(0) 且 未注销(状态!=2)
    if (pCard->nStatus != 0) {
        return FALSE;
    }

    float refundAmount = pCard->fBalance;
    // 修改卡状态为已注销(2)，余额清零
    pCard->nStatus = 2;
    pCard->fBalance = 0.0f;

    // 更新文件
    if (updateCard(pCard, index) == FALSE) {
        // 回滚
        pCard->nStatus = 0;
        pCard->fBalance = refundAmount;
        return FALSE;
    }

    // 保存退费记录（注销也算退费，记录在money.ams中）
    Money moneyRecord = { 0 };
    strcpy(moneyRecord.aCardName, pCard->aName);
    moneyRecord.tTime = time(NULL);
    moneyRecord.nStatus = 1;   // 退费类型
    moneyRecord.fMoney = refundAmount;
    moneyRecord.nDel = 0;
    addMoney(&moneyRecord);

    // 填充返回信息
    strcpy(pMoneyInfo->aCardName, pCard->aName);
    pMoneyInfo->tTime = moneyRecord.tTime;
    pMoneyInfo->nStatus = 1;
    pMoneyInfo->fMoney = refundAmount;

    return TRUE;
}

// 在 service.c 末尾添加

// 1. 消费记录查询（根据卡号和时间段）
int queryConsumptionByCard(const char* cardName, time_t start, time_t end, Billing** ppResults, int* pCount) {
    if (cardName == NULL || ppResults == NULL || pCount == NULL) return FALSE;
    Billing* allBillings = NULL;
    int total = getAllBillings(&allBillings);
    if (total <= 0) {
        *ppResults = NULL;
        *pCount = 0;
        return FALSE;
    }
    // 先统计符合条件的数量
    int matchCount = 0;
    for (int i = 0; i < total; i++) {
        if (allBillings[i].nStatus == 1 && strcmp(allBillings[i].aCardName, cardName) == 0) {
            if (allBillings[i].tStart >= start && allBillings[i].tEnd <= end) {
                matchCount++;
            }
        }
    }
    if (matchCount == 0) {
        freeBillings(allBillings);
        *ppResults = NULL;
        *pCount = 0;
        return FALSE;
    }
    Billing* results = (Billing*)malloc(matchCount * sizeof(Billing));
    if (results == NULL) {
        freeBillings(allBillings);
        return FALSE;
    }
    int idx = 0;
    for (int i = 0; i < total; i++) {
        if (allBillings[i].nStatus == 1 && strcmp(allBillings[i].aCardName, cardName) == 0) {
            if (allBillings[i].tStart >= start && allBillings[i].tEnd <= end) {
                results[idx++] = allBillings[i];
            }
        }
    }
    freeBillings(allBillings);
    *ppResults = results;
    *pCount = matchCount;
    return TRUE;
}

// 2. 统计总营业额（时间段内所有已结算消费金额之和）
double getTotalTurnover(time_t start, time_t end) {
    Billing* allBillings = NULL;
    int total = getAllBillings(&allBillings);
    if (total <= 0) return 0.0;
    double sum = 0.0;
    for (int i = 0; i < total; i++) {
        if (allBillings[i].nStatus == 1 && allBillings[i].tStart >= start && allBillings[i].tEnd <= end) {
            sum += allBillings[i].fAmount;
        }
    }
    freeBillings(allBillings);
    return sum;
}

// 3. 统计月营业额（给定年份，返回12个月每个月的营业额）
int getMonthlyTurnover(int year, double monthlyTurnover[12]) {
    if (monthlyTurnover == NULL) return FALSE;
    // 初始化数组为0
    for (int i = 0; i < 12; i++) monthlyTurnover[i] = 0.0;
    Billing* allBillings = NULL;
    int total = getAllBillings(&allBillings);
    if (total <= 0) return FALSE;
    for (int i = 0; i < total; i++) {
        if (allBillings[i].nStatus == 1) {
            struct tm* tm_info = localtime(&allBillings[i].tEnd);
            if (tm_info->tm_year + 1900 == year) {
                int month = tm_info->tm_mon; // 0-11
                monthlyTurnover[month] += allBillings[i].fAmount;
            }
        }
    }
    freeBillings(allBillings);
    return TRUE;
}

// 4. 特色功能：现金流统计（充值总额、退费总额）
int getCashFlow(time_t start, time_t end, double* totalRecharge, double* totalRefund) {
    if (totalRecharge == NULL || totalRefund == NULL) return FALSE;
    *totalRecharge = 0.0;
    *totalRefund = 0.0;
    Money* allMoneys = NULL;
    int total = getAllMoneys(&allMoneys);
    if (total <= 0) return FALSE;
    for (int i = 0; i < total; i++) {
        if (allMoneys[i].tTime >= start && allMoneys[i].tTime <= end) {
            if (allMoneys[i].nStatus == 0) { // 充值
                *totalRecharge += allMoneys[i].fMoney;
            }
            else if (allMoneys[i].nStatus == 1) { // 退费
                *totalRefund += allMoneys[i].fMoney;
            }
        }
    }
    freeMoneys(allMoneys);
    return TRUE;
}

// 5. 辅助函数：将统计结果输出到文件（追加或覆盖）
int exportStatisticsToFile(const char* filename, const char* content) {
    if (filename == NULL || content == NULL) return FALSE;
    FILE* fp = fopen(filename, "a");  // 追加模式
    if (fp == NULL) return FALSE;
    fprintf(fp, "%s", content);
    fclose(fp);
    return TRUE;
}
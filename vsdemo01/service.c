#include"service.h"
#include <math.h>   // 用于 ceil 向上取整

int doLogon(const char* pName, const char* pPwd, LogonInfo* pInfo)
{
    int ind = 0;
    Card* card=checkCard(pName, pPwd, &ind);
    if (card == NULL)
    {
        return FALSE;
    }
    if (card->nStatus != 0)
    {
        return UNUSE;
    }
    if(card->fBalance <= 0)
    {
        return ENOUGHMONEY;
    }
    card->nStatus = 1;

    strcpy(pInfo->aCardName , card->aName);
    pInfo->fBalance = card->fBalance;
    pInfo->tLogon = time(NULL);

    Billing* billing = (Billing*)malloc(sizeof(Billing));
    if (billing == NULL)
    {
        return FALSE;
    }
    InitBilling(billing);
    billing->tStart = pInfo->tLogon;
    strcpy(billing->aCardName , pInfo->aCardName);

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
double getAmount(time_t tStart, time_t tEnd) {
    double minutes = difftime(tEnd, tStart) / 60.0;
    if (minutes <= 0) return 0.0;
    // 不足一个收费单元按一个单元计算
    int units = (int)ceil(minutes / UNIT);
    return units * CHARGE;
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
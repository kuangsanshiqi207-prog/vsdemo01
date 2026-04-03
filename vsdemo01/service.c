#include"service.h"
#include <math.h>   // 用于 ceil 向上取整

int doLogon(const char* pName, const char* pPwd, LogonInfo* pInfo)
{
    int ind = 0;
    Card* card=checkAndUpdateCard(pName, pPwd, &ind);
    if (card == NULL)
    {
        return FALSE;
    }

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
    if (updateCard(&updatedCard, userpath, cardIndex) == FALSE) {
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
        // 注意：卡信息已经更新，这里如果计费更新失败可能需要回滚，简单处理返回失败
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
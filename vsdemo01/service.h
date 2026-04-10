#pragma once

#ifndef SERVICE_H
#define SERVICE_H

#include"global.h"
#include"billing_service.h"
#include"card_service.h"
#include "config.h"
#include"money_service.h"

int doLogon(const char* pName, const char* pPwd, LogonInfo* pInfo);
int doSettle(const char* pName, const char* pPwd, SettleInfo* pInfo);
double getAmount(time_t tStart, time_t tEnd);
int doAddMoney(const char* pName, const char* pPwd, Money* money);
int doRefundMoney(const char* pName, const char* pPwd, Money* pMoneyInfo);
int doCancelCard(const char* pName, const char* pPwd, Money* pMoneyInfo);
int queryConsumptionByCard(const char* cardName, time_t start, time_t end, Billing** ppResults, int* pCount);
double getTotalTurnover(time_t start, time_t end);
int getMonthlyTurnover(int year, double monthlyTurnover[12]);
int getCashFlow(time_t start, time_t end, double* totalRecharge, double* totalRefund);
int exportStatisticsToFile(const char* filename, const char* content);


#endif // !SERVICE_H


#pragma once

#ifndef SERVICE_H
#define SERVICE_H

#include"global.h"
#include"billing_service.h"
#include"card_service.h"
#include"money_service.h"

int doLogon(const char* pName, const char* pPwd, LogonInfo* pInfo);
int doSettle(const char* pName, const char* pPwd, SettleInfo* pInfo);
double getAmount(time_t tStart, time_t tEnd);
int doAddMoney(const char* pName, const char* pPwd, Money* money);
int doRefundMoney(const char* pName, const char* pPwd, Money* pMoneyInfo);
int doCancelCard(const char* pName, const char* pPwd, Money* pMoneyInfo);

#endif // !SERVICE_H


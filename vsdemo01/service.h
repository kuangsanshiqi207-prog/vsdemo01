#pragma once

#ifndef SERVICE_H
#define SERVICE_H

#include"global.h"
#include"billing_service.h"
#include"card_service.h"

int doLogon(const char* pName, const char* pPwd, LogonInfo* pInfo);
int doSettle(const char* pName, const char* pPwd, SettleInfo* pInfo);
double getAmount(time_t tStart, time_t tEnd);

#endif // !SERVICE_H


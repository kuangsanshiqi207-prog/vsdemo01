#pragma once

#ifndef BILLING_FIL_H
#define BILLING_FIL_H

#include<stdio.h>
#include"model.h"
#include"global.h"
#include<stdlib.h>
int saveBilling(const Billing* pBilling, const char* pPath);
int getBillingCount(const char* pPath);
int readBilling(Billing* pBilling, const char* pPath);
int updateBilling(const Billing* pBilling, const char* pPath, int nIndex);
int getAllBillings(Billing** ppBillings);  // 返回记录数，通过ppBillings返回动态数组
void freeBillings(Billing* pBillings);

#endif // !BILLING_FIL_H
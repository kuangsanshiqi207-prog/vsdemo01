#pragma once

#ifndef BILLING_FIL_H
#define BILLING_FIL_H

#include<stdio.h>
#include"model.h"
#include"global.h"
int saveBilling(const Billing* pBilling, const char* pPath);
int getBillingCount(const char* pPath);
int readBilling(Billing* pBilling, const char* pPath);
int updateBilling(const Billing* pBilling, const char* pPath, int nIndex);

#endif // !BILLING_FIL_H
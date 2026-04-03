#pragma once

#ifndef BILLING_SERVICE_H
#define BILLING_SERVICE_H

#include"model.h"
#include<stdio.h>
#include"billing_file.h"
#include"global.h"
#include<string.h>
#include <stdlib.h>

int InitBilling(Billing* billing);
int addBilling(Billing* billing);

extern struct BillingList* billinglist;   // 计费信息链表头结点

void initBillingList();
void releaseBillingList();
struct BillingList* findActiveBilling(const char* cardName, int* pIndex);

#endif // !BILLING_SERVICE_H


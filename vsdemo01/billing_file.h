#pragma once

#ifndef BILLING_FIL_H
#define BILLING_FIL_H

#include<stdio.h>
#include"model.h"
#include"global.h"
int saveBilling(const Billing* pBilling, const char* pPath);

#endif // !BILLING_FIL_H
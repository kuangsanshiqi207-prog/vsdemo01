#pragma once

#ifndef BILLING_SERVICE_H
#define BILLING_SERVICE_H

#include"model.h"
#include<stdio.h>
#include"billing_file.h"
#include"global.h"

int InitBilling(Billing* billing);
int addBilling(Billing* billing);


#endif // !BILLING_SERVICE_H


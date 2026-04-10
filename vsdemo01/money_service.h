#pragma once
#ifndef MONEY_SERVICE
#define MONEY_SERVICE

#include<stdio.h>
#include"global.h"
#include"model.h"
#include"money_file.h"
#include<stdlib.h>


int addMoney(Money* money);
int getAllMoneys(Money** ppMoneys);   // ·µ»Ø¼ÇÂ¼Êý
void freeMoneys(Money* pMoneys);


#endif // !MONEY_SERVICE


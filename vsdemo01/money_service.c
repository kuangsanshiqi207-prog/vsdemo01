#include"money_service.h"

int addMoney(Money* money)
{
	return saveMoney(money, moneypath);
}
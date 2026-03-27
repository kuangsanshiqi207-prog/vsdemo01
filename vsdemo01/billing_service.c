#include"billing_service.h"

int InitBilling(Billing* billing)
{
	billing->fAmount = 0;
	billing->nDel = 0;
	billing->nStatus = 0;
	billing->tEnd = 0;
}

int addBilling(Billing* billing)
{
	if (saveBilling(billing, billingpath))
	{
		return TRUE;
	}
	return FALSE;
}
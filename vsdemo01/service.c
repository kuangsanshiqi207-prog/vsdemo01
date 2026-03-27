#include"service.h"

int doLogon(const char* pName, const char* pPwd, LogonInfo* pInfo)
{
    int ind = 0;
    Card* card=checkAndUpdateCard(pName, pPwd, &ind);
    if (card == NULL)
    {
        return FALSE;
    }

    strcpy(pInfo->aCardName , card->aName);
    pInfo->fBalance = card->fBalance;
    pInfo->tLogon = time(NULL);

    Billing* billing = (Billing*)malloc(sizeof(Billing));
    if (billing == NULL)
    {
        return FALSE;
    }
    InitBilling(billing);
    billing->tStart = pInfo->tLogon;
    strcpy(billing->aCardName , pInfo->aCardName);

    addBilling(billing);

    free(billing);
    return TRUE;
}

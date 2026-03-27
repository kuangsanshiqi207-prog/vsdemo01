#include"service.h"

int doLogon(const char* pName, const char* pPwd, LogonInfo* pInfo)
{
    int ind = 0;
    Card* card=checkCard(pName, pPwd, &ind);
    if (card == NULL)
    {
        return FALSE;
    }

    strcpy(pInfo->aCardName , card->aName);
    pInfo->fBalance = card->fBalance;
    pInfo->tLogon = time(NULL);
    return TRUE;
}

#include"money_service.h"

int addMoney(Money* money)
{
	return saveMoney(money, moneypath);
}
// ÔÚ money_file.c Ä©Î²Ìí¼Ó
int getAllMoneys(Money** ppMoneys) {
    FILE* fp = fopen(moneypath, "rb");
    if (fp == NULL) {
        *ppMoneys = NULL;
        return 0;
    }
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    int count = (int)(size / sizeof(Money));
    if (count == 0) {
        fclose(fp);
        *ppMoneys = NULL;
        return 0;
    }
    Money* arr = (Money*)malloc(count * sizeof(Money));
    if (arr == NULL) {
        fclose(fp);
        *ppMoneys = NULL;
        return 0;
    }
    rewind(fp);
    size_t readCount = fread(arr, sizeof(Money), count, fp);
    fclose(fp);
    if (readCount != count) {
        free(arr);
        *ppMoneys = NULL;
        return 0;
    }
    *ppMoneys = arr;
    return count;
}

void freeMoneys(Money* pMoneys) {
    if (pMoneys) free(pMoneys);
}
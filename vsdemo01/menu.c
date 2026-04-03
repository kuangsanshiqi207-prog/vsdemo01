#include "menu.h"



/**
 * 显示菜单界面
 * 功能: 打印计费管理系统的菜单选项，供用户选择操作
 * @param void
 * @return void
 */
void showMenu()
{
    printf("------菜单界面------\n");
    printf("      1.添加卡\n");
    printf("      2.查询卡\n");
    printf("      3.上机\n");
    printf("      4.下机\n");
    printf("      5.充值\n");
    printf("      6.退费\n");
    printf("      7.查询统计\n");
    printf("      8.注销卡\n");
    printf("      0.退出系统\n");
    printf("--------------------\n");
    printf("请输入您的选择: (1-8)");
}

void initSpace()
{
    initCard();
    initBillingList();
}

void add()
{
    printf("------添加卡------\n");
    Card* newCard = (Card*)malloc(sizeof(struct Card));
    if (newCard == NULL)
    {
        printf("分配空间失败\n");
        return;
    }

    printf("请输入卡的信息:\n");
    char cardName[100];   // 存储卡号，大小足够
    char pass[100];   // 存储密码

    // 输入卡号
    printf("请输入卡号: <长度为0-18>");
    fgets(cardName, sizeof(cardName), stdin);
    // 去掉 fgets 读入的换行符
    cardName[strcspn(cardName, "\n")] = '\0';

    // 输入密码
    printf("请输入密码: <长度为8>");
    fgets(pass, sizeof(pass), stdin);
    pass[strcspn(pass, "\n")] = '\0';

    // 检查长度
    if (strlen(cardName) > 18 || strlen(pass) != 8)
    {
        printf("卡号或密码不符合规定\n");
        return;
    }

    strcpy(newCard->aName, cardName);
    strcpy(newCard->aPwd, pass);

    newCard->nStatus = 0;
    newCard->tStart = time(NULL);
    newCard->tEnd = time(NULL) + 30 * 24 * 60 * 60;  // 当前时间 + 30 天的秒数
    newCard->fTotalUse = 0;
    newCard->tLast = time(NULL);
    newCard->nUseCount = 0;
    newCard->nDel = 0;

    printf("请输入余额: <RMB>");
    scanf("%f", &newCard->fBalance);
    getchar();
    
    if (!addCard(newCard))
    {
        printf("添加卡失败\n");
    }
    else
    {
        printf("------添加卡的信息如下所示------\n");
        printf("卡号: %s\n", newCard->aName);
        printf("密码: %s\n", newCard->aPwd);
        printf("余额: %f\n\n", newCard->fBalance);
    }

    free(newCard);
}


void query()
{
    printf("------查询卡------\n");
    char cardName[256] = { '\0' };
    printf("请输入您的卡号:");
    fgets(cardName, sizeof(cardName), stdin);
    cardName[strcspn(cardName, "\n")] = '\0';
    if (strlen(cardName) > 18)
    {
        printf("卡号不符合规定\n");
        return;
    }

    int flag = 0;
    CardList* sercCard = queryCard(cardName, &flag);
    CardList* p = NULL;
    if (sercCard == NULL)
    {
        printf("卡号不存在,模糊匹配也不存在\n");
        return;
    }

    char time[20];

    printf("------卡的信息如下所示------\n");
    printf("卡号\t状态\t余额\t累计使用\t使用次数\t上次上机时间\n");
    while (sercCard != NULL)
    {
        format_time(time, 20, sercCard->card.tLast);
        p = sercCard;
        printf("%s\t%d\t%f\t%f\t%d\t%s\n", sercCard->card.aName, sercCard->card.nStatus, sercCard->card.fBalance, sercCard->card.fTotalUse, sercCard->card.nUseCount, time);
        sercCard = sercCard->next;
        free(p);
    }

}

void login()
{
    LogonInfo* logonInfo=(LogonInfo*)malloc(sizeof(LogonInfo));
    char cardName[19] = "";
    char password[9] = "";
    printf("请输入上机卡号:<1-18>");
    scanf("%s",cardName);
    getchar();
    printf("请输入上机密码<8>:");
    scanf("%s", password);
    getchar();
    int nResult = doLogon(cardName,password, logonInfo);
    char time[20]="";
    format_time(time,20,logonInfo->tLogon);
    printf("----上机结果----");
    switch (nResult)
    {
    case 0:printf("%s\n","上机失败"); break;
    case 1:
        printf("%s\n", "上机成功");
        printf("%8s %8s %8s","卡号","余额","上机时间\n");
        printf("%s,%f,%s\n", logonInfo->aCardName, logonInfo->fBalance, time);
        break;
    case 2:printf("%s\n","卡不能使用"); break;
    case 3:printf("%s\n","余额不足"); break;
    }
    free(logonInfo);
}


// 在 menu.c 中添加

void settle() {
    printf("------下机------\n");
    char cardName[19] = { 0 };
    char password[9] = { 0 };

    printf("请输入下机卡号 (1-18位): ");
    scanf("%18s", cardName);
    getchar();

    printf("请输入密码 (8位): ");
    scanf("%8s", password);
    getchar();

    SettleInfo info = { 0 };
    int result = doSettle(cardName, password, &info);

    char startTime[20], endTime[20];
    format_time(startTime, sizeof(startTime), info.tStart);
    format_time(endTime, sizeof(endTime), info.tEnd);

    switch (result) {
    case TRUE:
        printf("下机成功！\n");
        printf("卡号\t消费金额\t余额\t\t上机时间\t\t下机时间\n");
        printf("%s\t%.2f\t\t%.2f\t\t%s\t%s\n",
            info.aCardName, info.fAmount, info.fBalance,
            startTime, endTime);
        break;
    case FALSE:
        printf("下机失败：卡号或密码错误，或该卡未处于上机状态。\n");
        break;
    case UNUSE:
        printf("下机失败：卡状态不允许下机（可能已注销或未上机）。\n");
        break;
    case ENOUGHMONEY:
        printf("下机失败：余额不足，请充值后再下机。\n");
        break;
    default:
        printf("下机失败：未知错误。\n");
        break;
    }
}

void freespace()
{
    freeCard();
    releaseBillingList();
}
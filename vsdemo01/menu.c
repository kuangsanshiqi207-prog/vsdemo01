#include "menu.h"

void initSpace()
{
    initCard();
}

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
    printf("------添加卡的信息如下所示------\n");
    printf("卡号: %s\n", newCard->aName);
    printf("密码: %s\n", newCard->aPwd);
    printf("余额: %f\n\n", newCard->fBalance);
    addCard(newCard);

    free(newCard);
}


void query()
{
    printf("------查询卡------\n");
    char cardName[256] = { '\0'};
    printf("请输入您的卡号:");
    fgets(cardName, sizeof(cardName), stdin);
    cardName[strcspn(cardName, "\n")] = '\0';
    if (strlen(cardName) > 18)
    {
        printf("卡号不符合规定\n");
        return;
    }

    int flag = 0;
    CardList* sercCard = queryCard(cardName,&flag);
    CardList* p=NULL;
    if(sercCard== NULL)
    {
        printf("卡号不存在,模糊匹配也不存在\n");
        return;
    }

    printf("------卡的信息如下所示------\n");
    printf("卡号\t状态\t余额\t累计使用\t使用次数\t上次上机时间\n");
    while(sercCard!=NULL)
    {
        p = sercCard;
        printf("%s\t%d\t%f\t%f\t%d\t%lld\n", sercCard->card.aName, sercCard->card.nStatus, sercCard->card.fBalance, sercCard->card.fTotalUse, sercCard->card.nUseCount, sercCard->card.tLast);
        sercCard = sercCard->next;
        free(p);
    }
    
}

void freespace()
{
    freeCard();
}
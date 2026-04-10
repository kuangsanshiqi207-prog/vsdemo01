#include "menu.h"

#include <ctype.h>

/**
 * 显示菜单界面
 * 功能: 打印计费管理系统的菜单选项，供用户选择操作
 * @param void
 * @return void
 */
void showMenu() {
    printf("------菜单界面------\n");
    printf("      1.添加卡\n");
    printf("      2.查询卡\n");
    printf("      3.上机\n");
    printf("      4.下机\n");
    printf("      5.充值\n");
    printf("      6.退费\n");
    printf("      7.查询统计\n");
    printf("      8.注销卡\n");
    printf("      9.计费标准管理\n");
    printf("     10.恢复卡\n");    // 新增
    printf("      0.退出系统\n");
    printf("--------------------\n");
    printf("请输入您的选择: (0-10)");
}

void manageConfig() {
    int choice;
    do {
        printf("\n====== 计费标准管理 ======\n");
        printf("1. 查看当前标准\n");
        printf("2. 修改标准\n");
        printf("3. 恢复默认标准\n");
        printf("0. 返回上级菜单\n");
        printf("请选择: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
        case 1:
            showConfig();
            break;
        case 2: {
            int unit;
            double charge;
            printf("请输入新的收费单元（分钟）: ");
            scanf("%d", &unit);
            printf("请输入每个单元的金额（元）: ");
            scanf("%lf", &charge);
            getchar();
            if (unit > 0 && charge > 0) {
                setConfig(unit, charge);
                printf("计费标准已更新！\n");
            }
            else {
                printf("输入无效，标准未改变。\n");
            }
            break;
        }
        case 3:
            setConfig(UNIT, CHARGE);  // 恢复默认宏
            printf("已恢复默认计费标准。\n");
            break;
        case 0:
            break;
        default:
            printf("无效选择。\n");
        }
    } while (choice != 0);
}

void initSpace()
{
    initCard();
    initBillingList();
}

// 检查字符串是否全为数字
static int isAllDigits(const char* str) {
    for (int i = 0; str[i]; i++) {
        if (!isdigit((unsigned char)str[i])) return 0;
    }
    return 1;
}

// 检查密码是否同时包含字母和数字
static int isAlphaNumMix(const char* pwd) {
    int hasDigit = 0, hasAlpha = 0;
    for (int i = 0; pwd[i]; i++) {
        if (isdigit((unsigned char)pwd[i])) hasDigit = 1;
        else if (isalpha((unsigned char)pwd[i])) hasAlpha = 1;
        else return 0;
    }
    return hasDigit && hasAlpha;
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
    printf("请输入卡号: <1-18位数字>");
    fgets(cardName, sizeof(cardName), stdin);
    cardName[strcspn(cardName, "\n")] = '\0';
    if (strlen(cardName) == 0 || strlen(cardName) > 18 || !isAllDigits(cardName)) {
        printf("卡号必须是1-18位数字！\n");
        free(newCard);
        return;
    }

    // 输入密码
    printf("请输入密码: <8位，必须包含数字和字母>");
    fgets(pass, sizeof(pass), stdin);
    pass[strcspn(pass, "\n")] = '\0';
    if (strlen(pass) != 8 || !isAlphaNumMix(pass)) {
        printf("密码必须是8位数字和字母的组合！\n");
        free(newCard);
        return;
    }

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
    if (!logonInfo == NULL)
    {
        format_time(time, 20, logonInfo->tLogon);
    }
    printf("----上机结果----\n");
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
        printf("下机失败:卡号或密码错误，或该卡未处于上机状态。\n");
        break;
    case UNUSE:
        printf("下机失败:卡状态不允许下机（可能已注销或未上机）。\n");
        break;
    case ENOUGHMONEY:
        printf("下机失败:余额不足，请充值后再下机。\n");
        break;
    default:
        printf("下机失败:未知错误。\n");
        break;
    }
}

void rechange()
{
    printf("------充值------\n");
    char cardName[19] = { 0 };
    char password[9] = { 0 };

    printf("请输入充值卡号 (1-18位): ");
    scanf("%18s", cardName);
    getchar();

    printf("请输入密码 (8位): ");
    scanf("%8s", password);
    getchar();

    Money money = { 0 };

    printf("请输入充值金额: ");
    scanf("%f", &money.fMoney);
    getchar();

    int result = doAddMoney(cardName,password,&money);

    char rechangeTime[20];
    format_time(rechangeTime, sizeof(rechangeTime), money.tTime);


    switch (result) {
    case TRUE:
        printf("充值成功！\n");
        printf("------卡的信息如下所示------\n");
        printf("卡号\t余额\t\t充值时间\n");
        printf("%s\t%.2f\t\t%s\n",
           money.aCardName,money.fMoney,rechangeTime);
        break;
    case FALSE:
        printf("充值失败:卡号或密码错误。\n");
        break;
    case ENOUGHMONEY:
        printf("充值失败:余额不能为零。\n");
        break;
    default:
        printf("充值失败:未知错误。\n");
        break;
    }
}


void refundMoney()
{
    printf("------退费------\n");
    char cardName[19] = { 0 };
    char password[9] = { 0 };

    printf("请输入卡号 (1-18位): ");
    scanf("%18s", cardName);
    getchar();

    printf("请输入密码 (8位): ");
    scanf("%8s", password);
    getchar();

    Money moneyInfo = { 0 };
    int result = doRefundMoney(cardName, password, &moneyInfo);

    if (result == TRUE) {
        printf("退费成功！\n");
        printf("卡号\t\t退费金额\t余额\n");
        printf("%s\t\t%.2f\t\t0.00\n", cardName, moneyInfo.fMoney);
    }
    else {
        printf("退费失败！请检查卡号密码或卡状态（非未上机/余额为0）\n");
    }
}
void cancelCard()
{
    printf("------注销卡------\n");
    char cardName[19] = { 0 };
    char password[9] = { 0 };

    printf("请输入卡号 (1-18位): ");
    scanf("%18s", cardName);
    getchar();

    printf("请输入密码 (8位): ");
    scanf("%8s", password);
    getchar();

    Money moneyInfo = { 0 };
    int result = doCancelCard(cardName, password, &moneyInfo);

    if (result == TRUE) {
        printf("注销成功！\n");
        printf("卡号\t\t退款金额\n");
        printf("%s\t\t%.2f\n", cardName, moneyInfo.fMoney);
    }
    else {
        printf("注销失败！请检查卡号密码或卡未处于未上机状态\n");
    }
}


void statistics() {
    printf("\n====== 查询统计 ======\n");
    printf("1. 消费记录查询\n");
    printf("2. 统计总营业额\n");
    printf("3. 统计月营业额\n");
    printf("4. 现金流统计（特色）\n");
    printf("0. 返回上级菜单\n");
    printf("请选择: ");
    int choice;
    scanf("%d", &choice);
    getchar();

    char startStr[20], endStr[20];
    time_t start, end;
    struct tm tmStart = { 0 }, tmEnd = { 0 };

    switch (choice) {
    case 1:  // 消费记录查询
    {
        char cardName[19] = { 0 };
        printf("请输入卡号: ");
        scanf("%18s", cardName);
        getchar();
        printf("请输入起始时间 (格式: YYYY-MM-DD HH:MM:SS): ");
        fgets(startStr, sizeof(startStr), stdin);
        startStr[strcspn(startStr, "\n")] = '\0';
        printf("请输入结束时间 (格式: YYYY-MM-DD HH:MM:SS): ");
        fgets(endStr, sizeof(endStr), stdin);
        endStr[strcspn(endStr, "\n")] = '\0';

        start = parse_time(startStr);
        end = parse_time(endStr);

        Billing* results = NULL;
        int count = 0;
        if (queryConsumptionByCard(cardName, start, end, &results, &count)) {
            printf("\n消费记录 (%d 条):\n", count);
            printf("卡号\t\t上机时间\t\t下机时间\t\t消费金额\n");
            char startTime[20], endTime[20];
            for (int i = 0; i < count; i++) {
                format_time(startTime, sizeof(startTime), results[i].tStart);
                format_time(endTime, sizeof(endTime), results[i].tEnd);
                printf("%s\t%s\t%s\t%.2f\n", results[i].aCardName, startTime, endTime, results[i].fAmount);
            }
            free(results);
        }
        else {
            printf("未找到符合条件的消费记录。\n");
        }
        break;
    }
    case 2:  // 统计总营业额
    {
        printf("请输入起始时间: ");
        fgets(startStr, sizeof(startStr), stdin);
        startStr[strcspn(startStr, "\n")] = '\0';
        printf("请输入结束时间: ");
        fgets(endStr, sizeof(endStr), stdin);
        endStr[strcspn(endStr, "\n")] = '\0';
        start = parse_time(startStr);
        end = parse_time(endStr);
        double turnover = getTotalTurnover(start, end);
        char buf[256];
        sprintf(buf, "\n时间段 %s 至 %s 的总营业额为: %.2f 元\n", startStr, endStr, turnover);
        printf("%s", buf);
        // 输出到文件
        exportStatisticsToFile("statistics.txt", buf);
        break;
    }
    case 3:  // 统计月营业额
    {
        int year;
        printf("请输入年份 (如 2026): ");
        scanf("%d", &year);
        getchar();
        double monthly[12];
        if (getMonthlyTurnover(year, monthly)) {
            char buf[1024] = { 0 };
            char line[128];
            sprintf(buf, "\n%s 年各月营业额:\n", year);
            for (int i = 0; i < 12; i++) {
                sprintf(line, "%2d月: %.2f 元\n", i + 1, monthly[i]);
                strcat(buf, line);
                printf("%s", line);
            }
            exportStatisticsToFile("statistics.txt", buf);
        }
        else {
            printf("无数据或读取失败。\n");
        }
        break;
    }
    case 4:  // 现金流统计
    {
        printf("请输入起始时间: ");
        fgets(startStr, sizeof(startStr), stdin);
        startStr[strcspn(startStr, "\n")] = '\0';
        printf("请输入结束时间: ");
        fgets(endStr, sizeof(endStr), stdin);
        endStr[strcspn(endStr, "\n")] = '\0';
        start = parse_time(startStr);
        end = parse_time(endStr);
        double recharge, refund;
        if (getCashFlow(start, end, &recharge, &refund)) {
            char buf[512];
            sprintf(buf, "\n时间段 %s 至 %s 的现金流:\n充值总额: %.2f 元\n退费总额: %.2f 元\n净流入: %.2f 元\n",
                startStr, endStr, recharge, refund, recharge - refund);
            printf("%s", buf);
            exportStatisticsToFile("statistics.txt", buf);
        }
        else {
            printf("无充值退费数据或读取失败。\n");
        }
        break;
    }
    case 0:
        return;
    default:
        printf("无效选择。\n");
    }
}

void restoreCardMenu() {
    printf("------恢复已注销卡------\n");
    char cardName[19] = { 0 };
    char password[9] = { 0 };
    printf("请输入卡号: ");
    scanf("%18s", cardName);
    getchar();
    printf("请输入密码: ");
    scanf("%8s", password);
    getchar();
    if (restoreCard(cardName, password)) {
        printf("卡号 %s 恢复成功！\n", cardName);
    }
    else {
        printf("恢复失败，请检查卡号密码或该卡未处于注销状态。\n");
    }
}

void freespace()
{
    freeCard();
    releaseBillingList();
}

#include "admin_service.h"

void adminManagementMenu() {
    int choice;
    do {
        printf("\n====== 管理员管理 ======\n");
        printf("1. 添加管理员\n");
        printf("2. 删除管理员\n");
        printf("3. 修改管理员权限\n");
        printf("4. 查看所有管理员\n");
        printf("0. 返回上级菜单\n");
        printf("请选择: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
        case 1: {
            char username[32], password[32];
            int role;
            printf("请输入用户名: ");
            scanf("%31s", username);
            printf("请输入密码: ");
            scanf("%31s", password);
            printf("选择角色 (0-超级管理员, 1-普通管理员): ");
            scanf("%d", &role);
            getchar();
            if (addAdmin(username, password, role)) {
                printf("添加成功！\n");
            }
            else {
                printf("添加失败，用户名可能已存在。\n");
            }
            break;
        }
        case 2: {
            char username[32];
            printf("请输入要删除的管理员用户名: ");
            scanf("%31s", username);
            if (deleteAdmin(username)) {
                printf("删除成功！\n");
            }
            else {
                printf("删除失败，不能删除自己或用户不存在。\n");
            }
            break;
        }
        case 3: {
            char username[32];
            int newRole;
            printf("请输入要修改权限的管理员用户名: ");
            scanf("%31s", username);
            printf("新角色 (0-超级管理员, 1-普通管理员): ");
            scanf("%d", &newRole);
            getchar();
            if (modifyAdminRole(username, newRole)) {
                printf("修改成功！\n");
            }
            else {
                printf("修改失败，不能修改自己的权限或用户不存在。\n");
            }
            break;
        }
        case 4: {
            AdminList* admins = findAllAdmins();
            if (admins == NULL) {
                printf("没有找到任何管理员。\n");
            }
            else {
                printf("用户名\t\t角色\n");
                AdminList* p = admins;
                while (p != NULL) {
                    printf("%s\t\t%s\n", p->admin.username,
                        p->admin.role == 0 ? "超级管理员" : "普通管理员");
                    p = p->next;
                }
                freeAdminListResult(admins);
            }
            break;
        }
        case 0:
            break;
        default:
            printf("无效选择。\n");
        }
    } while (choice != 0);
}
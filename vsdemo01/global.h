#pragma once
#ifndef GLOBAL_H
#define GLOBAL_H

#define FALSE 0 //上机失败
#define TRUE 1 //上机成功
#define UNUSE 2  // 卡不能使用
#define ENOUGHMONEY 3 //余额不足
#define BILLINGPATH "datallbilling.ams" // 计费信息保存路径
#define ADMIN_PATH ".\\admin.bin"
#define CARD_LINE_SIZE 128
#define MAX_SIZE 50
#define UNIT 15      // 最小收费单元（分钟）
#define CHARGE 0.5   // 每个收费单元的金额（元）
#define MAX_BALANCE 10000000.0   // 最大余额限制（元）

extern char userpath[100];
extern char billingpath[100];
extern char moneypath[100];


#endif // !GLOBAL_H


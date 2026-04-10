#pragma once
#ifndef MODEL_H
#define MODEL_H

#include<stdio.h>
#include<time.h>

typedef struct Card {
	char aName[19];    // 卡号
	char aPwd[9];     // 密码
	int nStatus;   // 卡状态(0-未上机；1-正在上机；2-已注销；3-失效)
	time_t tStart;     // 开卡时间
	time_t tEnd;      // 卡的截止时间
	float fTotalUse;   // 累计金额
	time_t tLast;       // 最后使用时间
	int nUseCount;      // 使用次数
	float fBalance;     // 余额
	int nDel;           // 删除标识0-未删除,1-删除
}Card;

typedef struct CardList {
	Card card;
	struct CardList* next;
}CardList;

typedef struct Billing {
	char aCardName[18];    // 卡号
	time_t tStart;         // 上机时间
	time_t tEnd;           // 下机时间
	float fAmount;         // 消费金额
	int nStatus;           // 消费状态，0-未结算，1-已经结算
	int nDel;              // 删除标识，0-未删除,1-删除
}Billing;


typedef struct BillingList {
	Billing billing;
	struct BillingList* next;
} BillingList;

typedef struct LogonInfo {
	char aCardName[18];    // 上机卡号
	time_t tLogon;         // 上机时间
	float fBalance;        // 上机时的卡余额
}LogonInfo;

typedef struct SettleInfo {
	char aCardName[18];   // 卡号
	time_t tStart;        // 上机时间
	time_t tEnd;          // 下机时间
	float fAmount;        // 消费金额
	float fBalance;       // 余额
}SettleInfo;

typedef struct Money {
	char aCardName[18];  // 卡号
	time_t tTime;        // 充值退费的时间
	int nStatus;      // 状态:0-表示充值；1-表示退费
	float fMoney;       // 充值退费金额
	int nDel;         // 删除标识，0-未删除,1-删除
}Money;

// 管理员结构体
typedef struct Admin {
	char username[32];   // 用户名
	char password[32];   // 密码（实际应加密，此处简化）
	int role;            // 角色: 0-超级管理员, 1-普通管理员
	int nDel;            // 删除标识 0-未删除, 1-已删除
} Admin;

typedef struct AdminList {
	Admin admin;
	struct AdminList* next;
} AdminList;

#endif // !MODEL_H

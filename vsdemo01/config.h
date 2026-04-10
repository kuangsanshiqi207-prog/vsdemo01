#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>

// 计费标准配置结构体
typedef struct {
    int unitMinutes;   // 收费单元（分钟）
    double charge;     // 每个单元金额（元）
} BillingConfig;

// 全局配置变量（声明）
extern BillingConfig g_config;

// 函数声明
void loadConfig();                // 从文件加载配置，若文件不存在则使用默认值
void saveConfig();                // 保存配置到文件
void setConfig(int unit, double charge); // 修改配置并保存
void showConfig();                // 显示当前配置

#endif
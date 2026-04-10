#include "config.h"
#include "global.h"   // 使用 FALSE/TRUE 宏

// 全局配置变量定义
BillingConfig g_config;

// 配置文件路径（与数据文件在同一目录）
static const char* CONFIG_FILE = ".\\billing_config.bin";

void loadConfig() {
    FILE* fp = fopen(CONFIG_FILE, "rb");
    if (fp == NULL) {
        // 文件不存在，使用默认值（与 global.h 中的 UNIT/CHARGE 一致）
        g_config.unitMinutes = UNIT;
        g_config.charge = CHARGE;
        saveConfig();  // 创建默认配置文件
        return;
    }
    if (fread(&g_config, sizeof(BillingConfig), 1, fp) != 1) {
        // 读取失败，使用默认值
        g_config.unitMinutes = UNIT;
        g_config.charge = CHARGE;
    }
    fclose(fp);
}

void saveConfig() {
    FILE* fp = fopen(CONFIG_FILE, "wb");
    if (fp == NULL) return;
    fwrite(&g_config, sizeof(BillingConfig), 1, fp);
    fclose(fp);
}

void setConfig(int unit, double charge) {
    if (unit <= 0 || charge <= 0) return;
    g_config.unitMinutes = unit;
    g_config.charge = charge;
    saveConfig();
}

void showConfig() {
    printf("\n====== 当前计费标准 ======\n");
    printf("收费单元：%d 分钟\n", g_config.unitMinutes);
    printf("每个单元金额：%.2f 元\n", g_config.charge);
    printf("计算公式：消费金额 = ceil(上机分钟数 / 单元) × 单元金额\n\n");
}
#include "tool.h"

// 工具文件

/**
 * 将 time_t 时间转换为字符串格式 "YYYY-MM-DD HH:MM:SS"
 */
void format_time(char* buffer, size_t size, time_t t) {
    struct tm* tm_info = localtime(&t);
    if (tm_info == NULL) {
        snprintf(buffer, size, "1970-01-01 00:00:00");
    }
    else {
        strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
    }
}

/**
 * 将时间字符串转换为 time_t
 */
time_t parse_time(const char* time_str) {
    struct tm tm = { 0 };
    if (sscanf(time_str, "%d-%d-%d %d:%d:%d",
        &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
        &tm.tm_hour, &tm.tm_min, &tm.tm_sec) == 6) {
        tm.tm_year -= 1900;
        tm.tm_mon -= 1;
        return mktime(&tm);
    }
    return 0;
}

/**
 * 清空输入缓冲区（直到遇到换行符或 EOF）
 */
void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * 安全地读取一个整数（范围限制）
 * @param prompt  提示信息（若为空则不打印）
 * @param value   存储输入值的指针
 * @param min     允许的最小值
 * @param max     允许的最大值
 * @return        1-成功，0-失败（EOF 或无效输入）
 */
int safeGetInt(const char* prompt, int* value, int min, int max) {
    char buffer[100];
    while (1) {
        if (prompt && *prompt) {
            printf("%s", prompt);
        }
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            return 0;   // EOF
        }
        // 去掉换行符
        buffer[strcspn(buffer, "\n")] = '\0';

        // 检查空输入
        if (strlen(buffer) == 0) {
            printf("输入不能为空，请重新输入。\n");
            continue;
        }

        int temp;
        char extra;
        // 解析整数，并检查是否有多余字符
        if (sscanf(buffer, "%d %c", &temp, &extra) == 1) {
            if (temp >= min && temp <= max) {
                *value = temp;
                return 1;
            }
            else {
                printf("数值必须在 %d 到 %d 之间，请重新输入。\n", min, max);
            }
        }
        else {
            printf("无效输入，请输入一个整数。\n");
        }
    }
}

/**
 * 安全地读取一个双精度浮点数（范围限制）
 * @param prompt  提示信息（若为空则不打印）
 * @param value   存储输入值的指针
 * @param min     允许的最小值
 * @param max     允许的最大值
 * @return        1-成功，0-失败（EOF 或无效输入）
 */
int safeGetDouble(const char* prompt, double* value, double min, double max) {
    char buffer[100];
    while (1) {
        if (prompt && *prompt) {
            printf("%s", prompt);
        }
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            return 0;   // EOF
        }
        buffer[strcspn(buffer, "\n")] = '\0';

        if (strlen(buffer) == 0) {
            printf("输入不能为空，请重新输入。\n");
            continue;
        }

        double temp;
        char extra;
        if (sscanf(buffer, "%lf %c", &temp, &extra) == 1) {
            if (temp >= min && temp <= max) {
                *value = temp;
                return 1;
            }
            else {
                printf("数值必须在 %.2f 到 %.2f 之间，请重新输入。\n", min, max);
            }
        }
        else {
            printf("无效输入，请输入一个数字。\n");
        }
    }
}
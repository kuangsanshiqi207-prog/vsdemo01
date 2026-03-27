#include"tool.h"

//工具文件
/**
 * 将 time_t 时间转换为字符串格式 "YYYY-MM-DD HH:MM:SS"
 * @param buffer 输出缓冲区
 * @param size 缓冲区大小
 * @param t 时间值
 */
 void format_time(char* buffer, size_t size, time_t t) {
    struct tm* tm_info = localtime(&t);
    if (tm_info == NULL) {
        // 无效时间，填充默认字符串
        snprintf(buffer, size, "1970-01-01 00:00:00");
    }
    else {
        strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
    }
}


// 辅助函数：将时间字符串转换为 time_t
 time_t parse_time(const char* time_str) {
    struct tm tm = { 0 };
    // 使用 sscanf 解析时间字符串
    if (sscanf(time_str, "%d-%d-%d %d:%d:%d",
        &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
        &tm.tm_hour, &tm.tm_min, &tm.tm_sec) == 6) {
        tm.tm_year -= 1900;   // tm_year 从 1900 开始
        tm.tm_mon -= 1;       // tm_mon 从 0 开始
        return mktime(&tm);
    }
    // 解析失败，返回 0（1970-01-01 00:00:00）
    return 0;
}
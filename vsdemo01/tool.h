#pragma once
#ifndef TOOL_H
#define TOOL_H

#include <stdio.h>
#include <string.h>
#include <time.h>

// 时间处理函数
void format_time(char* buffer, size_t size, time_t t);
time_t parse_time(const char* time_str);

// 输入缓冲区清空函数
void clearInputBuffer(void);

// 安全的数值输入函数
int safeGetInt(const char* prompt, int* value, int min, int max);
int safeGetDouble(const char* prompt, double* value, double min, double max);

#endif // TOOL_H
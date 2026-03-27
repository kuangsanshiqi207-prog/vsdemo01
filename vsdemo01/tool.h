#pragma once
#ifndef TOOL_H
#define TOOL_H

#include<stdio.h>
#include<time.h>
void format_time(char* buffer, size_t size, time_t t);
time_t parse_time(const char* time_str);
#endif // !TOOL_H

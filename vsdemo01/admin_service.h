#pragma once
#ifndef ADMIN_SERVICE_H
#define ADMIN_SERVICE_H

#include "admin_file.h"
#include "model.h"
#include "global.h"
#include <string.h>
#include <stdlib.h>

extern AdminList* adminList;          // 带头结点的管理员链表
extern Admin* currentAdmin;           // 当前登录的管理员（指针指向链表中的节点）
extern int currentAdminRole;          // 当前登录管理员的角色

void initAdminList();                 // 初始化，从文件加载
void releaseAdminList();              // 释放链表
int adminLogin(const char* username, const char* password);  // 登录，返回角色，并设置 currentAdmin
int addAdmin(const char* username, const char* password, int role);
int modifyAdminRole(const char* username, int newRole);
int deleteAdmin(const char* username);  // 逻辑删除
AdminList* findAllAdmins();           // 返回所有未删除的管理员链表（供查询用）
void freeAdminListResult(AdminList* p);

#endif
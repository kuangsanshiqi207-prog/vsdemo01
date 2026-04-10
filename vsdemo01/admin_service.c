#include "admin_service.h"

AdminList* adminList = NULL;
Admin* currentAdmin = NULL;
int currentAdminRole = -1;

void initAdminList() {
    // 创建头结点
    AdminList* dummy = (AdminList*)malloc(sizeof(AdminList));
    if (dummy == NULL) return;
    dummy->next = NULL;
    adminList = dummy;

    int count = getAdminCount(ADMIN_PATH);
    if (count == 0) {
        // 如果没有管理员文件，创建一个默认超级管理员 admin / admin123
        Admin defaultAdmin;
        strcpy(defaultAdmin.username, "admin");
        strcpy(defaultAdmin.password, "admin123");
        defaultAdmin.role = 0;   // 超级管理员
        defaultAdmin.nDel = 0;
        saveAdmin(&defaultAdmin, ADMIN_PATH);
        count = 1;
    }

    Admin* arr = (Admin*)malloc(count * sizeof(Admin));
    if (arr == NULL) return;
    int readCnt = readAllAdmins(arr, ADMIN_PATH);
    if (readCnt <= 0) {
        free(arr);
        return;
    }

    AdminList* tail = dummy;
    for (int i = 0; i < readCnt; i++) {
        // 只加载未删除的
        if (arr[i].nDel == 0) {
            AdminList* newNode = (AdminList*)malloc(sizeof(AdminList));
            if (newNode == NULL) break;
            newNode->admin = arr[i];
            newNode->next = NULL;
            tail->next = newNode;
            tail = newNode;
        }
    }
    free(arr);
}

void releaseAdminList() {
    AdminList* p = adminList;
    while (p != NULL) {
        AdminList* next = p->next;
        free(p);
        p = next;
    }
    adminList = NULL;
    currentAdmin = NULL;
}

int adminLogin(const char* username, const char* password) {
    if (adminList == NULL) initAdminList();
    AdminList* p = adminList->next;
    while (p != NULL) {
        if (strcmp(p->admin.username, username) == 0 &&
            strcmp(p->admin.password, password) == 0 &&
            p->admin.nDel == 0) {
            currentAdmin = &p->admin;
            currentAdminRole = p->admin.role;
            return currentAdminRole;   // 返回角色
        }
        p = p->next;
    }
    return -1;  // 登录失败
}

int addAdmin(const char* username, const char* password, int role) {
    if (username == NULL || password == NULL) return FALSE;
    // 检查是否已存在（未删除）
    AdminList* p = adminList->next;
    while (p != NULL) {
        if (strcmp(p->admin.username, username) == 0 && p->admin.nDel == 0) {
            return FALSE;  // 已存在
        }
        p = p->next;
    }
    Admin newAdmin;
    strcpy(newAdmin.username, username);
    strcpy(newAdmin.password, password);
    newAdmin.role = role;
    newAdmin.nDel = 0;
    // 追加到文件
    if (saveAdmin(&newAdmin, ADMIN_PATH) == FALSE) return FALSE;
    // 插入链表尾部
    AdminList* newNode = (AdminList*)malloc(sizeof(AdminList));
    if (newNode == NULL) return FALSE;
    newNode->admin = newAdmin;
    newNode->next = NULL;
    AdminList* tail = adminList;
    while (tail->next != NULL) tail = tail->next;
    tail->next = newNode;
    return TRUE;
}

int modifyAdminRole(const char* username, int newRole) {
    if (username == NULL) return FALSE;
    // 不能修改自己的角色
    if (currentAdmin != NULL && strcmp(currentAdmin->username, username) == 0) {
        return FALSE;
    }
    AdminList* p = adminList->next;
    int idx = 1;
    while (p != NULL) {
        if (strcmp(p->admin.username, username) == 0 && p->admin.nDel == 0) {
            // 更新文件中的记录（需要找到索引）
            // 重新从文件读取索引较麻烦，简单方式：全量保存链表到文件
            // 先更新内存
            p->admin.role = newRole;
            // 全量保存
            if (saveAllAdmins(adminList, ADMIN_PATH) == FALSE) {
                // 回滚
                p->admin.role = (newRole == 0) ? 1 : 0;
                return FALSE;
            }
            return TRUE;
        }
        idx++;
        p = p->next;
    }
    return FALSE;
}

int deleteAdmin(const char* username) {
    if (username == NULL) return FALSE;
    // 不能删除自己
    if (currentAdmin != NULL && strcmp(currentAdmin->username, username) == 0) {
        return FALSE;
    }
    AdminList* p = adminList->next;
    int idx = 1;
    while (p != NULL) {
        if (strcmp(p->admin.username, username) == 0 && p->admin.nDel == 0) {
            // 逻辑删除
            p->admin.nDel = 1;
            if (saveAllAdmins(adminList, ADMIN_PATH) == FALSE) {
                p->admin.nDel = 0;
                return FALSE;
            }
            return TRUE;
        }
        idx++;
        p = p->next;
    }
    return FALSE;
}

AdminList* findAllAdmins() {
    AdminList* resultHead = NULL;
    AdminList* tail = NULL;
    AdminList* p = adminList->next;
    while (p != NULL) {
        if (p->admin.nDel == 0) {
            AdminList* newNode = (AdminList*)malloc(sizeof(AdminList));
            if (newNode == NULL) {
                freeAdminListResult(resultHead);
                return NULL;
            }
            newNode->admin = p->admin;
            newNode->next = NULL;
            if (resultHead == NULL) resultHead = newNode;
            else tail->next = newNode;
            tail = newNode;
        }
        p = p->next;
    }
    return resultHead;
}

void freeAdminListResult(AdminList* p) {
    while (p != NULL) {
        AdminList* next = p->next;
        free(p);
        p = next;
    }
}
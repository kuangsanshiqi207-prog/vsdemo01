#include<stdio.h>
#include<stdbool.h>
#include"menu.h"


/**
* 计费管理系统主函数
* 功能: 显示菜单界面，获取用户输入，并根据输入执行相应操作
* @param void
* @return 0 - 程序正常退出 
*/


int main() {
    loadConfig();

    // 初始化管理员链表
    initAdminList();

    char username[32], password[32];
    int role = 0;
    while (true)
    {
        // 登录
        printf("====== 管理员登录 ======\n");
        printf("用户名: ");
        scanf("%31s", username);
        printf("密码: ");
        scanf("%31s", password);
        getchar();

        role = adminLogin(username, password);
        if (role != -1) {
            printf("登录成功！当前角色: %s\n", (role == 0) ? "超级管理员" : "普通管理员"); 
            break;
        }
        printf("用户名或密码错误。\n");
    }

    initSpace();  // 初始化卡片和计费链表

    int exit = 1;
    do {
        // 动态显示菜单：如果是普通管理员，不显示“管理员管理”选项
        showMenu(role);

        int userChoice;
        scanf("%d", &userChoice);
        getchar();

        switch (userChoice) {
        case 1: add(); break;
        case 2: query(); break;
        case 3: login(); break;
        case 4: settle(); break;
        case 5: rechange(); break;
        case 6: refundMoney(); break;
        case 7: statistics(); break;
        case 8: cancelCard(); break;
		case 9: restoreCardMenu(); break;
        case 10:
            if (currentAdminRole == 0) manageConfig();
            else printf("权限不足，无法修改计费标准！\n");
            break;
        case 11:
            if (currentAdminRole == 0) adminManagementMenu();
            else printf("无效选择。\n");
            break;
        case 0:
            exit = 0;
            freespace();
            releaseAdminList();  // 释放管理员链表
            printf("感谢使用计费管理系统，再见！\n");
            break;
        default:
            printf("无效的选择。\n");
        }
    } while (exit);

    return 0;
}
#include<stdio.h>
#include<stdbool.h>
#include"menu.h"


/**
* 计费管理系统主函数
* 功能: 显示菜单界面，获取用户输入，并根据输入执行相应操作
* @param void
* @return 0 - 程序正常退出 
*/
int main()
{
	printf("欢迎进入计费管理系统\n");
	initSpace();
	int exit = 1;				//登录状态变量
    do {
        showMenu();				//显示菜单界面
		
		//fflush(stdin);			//清除输入缓冲区	
		
		int userChoice=-1;		//获取用户输入
		scanf("%d", &userChoice);
		getchar();

		switch (userChoice)		//根据用户输入执行相应操作
		{
			case 1:				//添加卡
			{
				printf("添加卡\n");
				add();
				break;
			}
			case 2:				//查询卡
			{
				printf("查询卡\n");
				query();
				break;
			}
			case 3:				//上机
			{
				printf("上机\n");
				login();
				break;
			}
			case 4:				//下机
			{
				printf("下机\n");
				break;
			}
			case 5:				//充值
			{
				printf("充值\n");
				break;
			}
			case 6:				//退费
			{
				printf("退费\n");
				break;
			}
			case 7:				//查询统计
			{
				printf("查询统计\n");
				break;
			}
			case 8:				//注销卡
			{
				printf("注销卡\n");
				break;
			}
			case 0:
			{
				exit = 0;	//退出系统
				freespace();
				printf("感谢使用计费管理系统，再见！\n");
				break;
			}
			default:
				printf("无效的选择。\n");
		}
	} while (exit);

    return 0;
}


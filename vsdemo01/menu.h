#pragma once

#ifndef MENU_H
#define MENU_H

#include<stdio.h>
#include<stdlib.h>
#include"service.h"
#include"model.h"
#include<string.h>
#include<time.h>
#include "admin_service.h"


void showMenu();
void initSpace();
void add();
void query();
void freespace();
void login();
void settle();
void rechange();
void refundMoney();  
void cancelCard();  
void statistics();
void manageConfig();
void restoreCardMenu();
void adminManagementMenu();


#endif // !MENU_H
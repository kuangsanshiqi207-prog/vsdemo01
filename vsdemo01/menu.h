#pragma once

#ifndef MENU_H
#define MENU_H

#include<stdio.h>
#include<stdlib.h>
#include"service.h"
#include"model.h"
#include<string.h>
#include<time.h>


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

#endif // !MENU_H
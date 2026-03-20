#pragma once

#ifndef CARD_SERVICE_H

#define CARD_SERVICE_H
#include"card_file.h"
#include"model.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

void initCard();
void addCard(Card* newCard);
CardList* queryCard(char* name, int* flag);
void copyCard(Card* dest, Card* src);
void freeCard();

#endif // !CARD_SERVICE_H


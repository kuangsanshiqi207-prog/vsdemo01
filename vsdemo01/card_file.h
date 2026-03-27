#pragma once

#ifndef CARD_FILE_H
#define CARD_FILE_H
#include"model.h"
#include"tool.h"
#include"global.h"

int saveCard(const CardList* head, const char* path);
int getCardCount(const char* pPath);
int readCard(Card* pCard, const char* pPath);
Card parseCard(char* pBuf);
int updateCard(const Card* pCard, const char* pPath, int nIndex);
#endif // !CARD_FILE_H

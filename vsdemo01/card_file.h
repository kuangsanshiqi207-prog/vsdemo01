#pragma once

#ifndef CARD_FILE_H
#define CARD_FILE_H

#include "model.h"
#include "tool.h"
#include "global.h"
#include <string.h>

int saveCard(const CardList* head, const char* path);
int appendCard(const Card* pCard, const char* path);
int getCardCount(const char* pPath);
int readCard(Card* pCard, const char* pPath);
int updateCardFile(const Card* pCard, const char* pPath, int nIndex);

// 删除 parseCard 声明（不再需要）
// Card parseCard(char* pBuf);

#endif
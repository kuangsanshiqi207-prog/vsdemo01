#pragma once

#ifndef SERVICE_H
#define SERVICE_H

#include"global.h"
#include"billing_service.h"
#include"card_service.h"

int doLogon(const char* pName, const char* pPwd, LogonInfo* pInfo);

#endif // !SERVICE_H


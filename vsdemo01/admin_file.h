#pragma once
#ifndef ADMIN_FILE_H
#define ADMIN_FILE_H

#include "model.h"
#include "global.h"
#include <stdio.h>
#include <stdlib.h>

int saveAdmin(const Admin* pAdmin, const char* path);          // ×·¼Ó
int saveAllAdmins(const AdminList* head, const char* path);    // È«Á¿¸²¸Ç
int getAdminCount(const char* path);
int readAllAdmins(Admin* pArray, const char* path);
int updateAdminFile(const Admin* pAdmin, const char* path, int index);
int deleteAdminFile(const char* path, int index);              // Âß¼­É¾³ý

#endif
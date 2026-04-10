#include "admin_file.h"

int saveAdmin(const Admin* pAdmin, const char* path) {
    if (pAdmin == NULL) return FALSE;
    FILE* fp = fopen(path, "ab");
    if (fp == NULL) return FALSE;
    size_t written = fwrite(pAdmin, sizeof(Admin), 1, fp);
    fclose(fp);
    return (written == 1) ? TRUE : FALSE;
}

int saveAllAdmins(const AdminList* head, const char* path) {
    if (head == NULL) return FALSE;
    FILE* fp = fopen(path, "wb");
    if (fp == NULL) return FALSE;
    AdminList* p = head->next;
    while (p != NULL) {
        if (fwrite(&p->admin, sizeof(Admin), 1, fp) != 1) {
            fclose(fp);
            return FALSE;
        }
        p = p->next;
    }
    fclose(fp);
    return TRUE;
}

int getAdminCount(const char* path) {
    FILE* fp = fopen(path, "rb");
    if (fp == NULL) return 0;
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fclose(fp);
    return (int)(size / sizeof(Admin));
}

int readAllAdmins(Admin* pArray, const char* path) {
    if (pArray == NULL) return -1;
    FILE* fp = fopen(path, "rb");
    if (fp == NULL) return -1;
    int count = 0;
    while (fread(&pArray[count], sizeof(Admin), 1, fp) == 1) {
        count++;
    }
    fclose(fp);
    return count;
}

int updateAdminFile(const Admin* pAdmin, const char* path, int index) {
    if (pAdmin == NULL || index < 1) return FALSE;
    FILE* fp = fopen(path, "rb+");
    if (fp == NULL) return FALSE;
    long offset = (long)(index - 1) * sizeof(Admin);
    if (fseek(fp, offset, SEEK_SET) != 0) {
        fclose(fp);
        return FALSE;
    }
    size_t written = fwrite(pAdmin, sizeof(Admin), 1, fp);
    fclose(fp);
    return (written == 1) ? TRUE : FALSE;
}

// Âß¼­É¾³ý£º½« nDel ÖÃÎª 1
int deleteAdminFile(const char* path, int index) {
    Admin admin;
    FILE* fp = fopen(path, "rb+");
    if (fp == NULL) return FALSE;
    long offset = (long)(index - 1) * sizeof(Admin);
    if (fseek(fp, offset, SEEK_SET) != 0) {
        fclose(fp);
        return FALSE;
    }
    if (fread(&admin, sizeof(Admin), 1, fp) != 1) {
        fclose(fp);
        return FALSE;
    }
    admin.nDel = 1;
    if (fseek(fp, offset, SEEK_SET) != 0) {
        fclose(fp);
        return FALSE;
    }
    size_t written = fwrite(&admin, sizeof(Admin), 1, fp);
    fclose(fp);
    return (written == 1) ? TRUE : FALSE;
}
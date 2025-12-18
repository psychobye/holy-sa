//
// Created on 14.04.2023.
//

#include "CFileMgr.h"
#include "main.h"
#include "util/patch.h"

void CFileMgr::SetDir(const char *path) {
    return CHook::CallFunction<void>("_ZN8CFileMgr6SetDirEPKc", path);
}

void CFileMgr::SetDirMyDocuments() {
    return CHook::CallFunction<void>("_ZN8CFileMgr17SetDirMyDocumentsEv");
}

FILE* CFileMgr::OpenFile(const char *path, const char *mode) {
    sprintf(ms_path, "%s%s", g_pszStorage, path);

    auto file = fopen(ms_path, mode);

    if(!file) {
        Log("Fail open file %s", ms_path);
    }
    return file;
}

int32_t CFileMgr::CloseFile(FILE* file) {
    return fclose(file);
}

void CFileMgr::Initialise() {
    CHook::CallFunction<void>("_ZN8CFileMgr10InitialiseEv"); // ������� ����� �����
}

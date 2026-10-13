#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef LAUNCH_CMD
#define LAUNCH_CMD "adt"
#endif

#ifndef LAUNCH_TITLE
#define LAUNCH_TITLE "AutoDockTools"
#endif

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    char exePath[MAX_PATH];
    if (!GetModuleFileNameA(NULL, exePath, MAX_PATH)) {
        MessageBoxA(NULL, "Failed to determine application directory.", LAUNCH_TITLE, MB_ICONERROR | MB_OK);
        return 1;
    }
    char *lastSlash = strrchr(exePath, '\\');
    if (lastSlash) *lastSlash = '\0';

    char pythonPath[MAX_PATH];
    snprintf(pythonPath, sizeof(pythonPath), "%s\\python\\pythonw.exe", exePath);

    char cmdLine[8192];
    if (lpCmdLine && lpCmdLine[0] != '\0') {
        snprintf(cmdLine, sizeof(cmdLine), "\"%s\" -m mgltools.launch %s %s", pythonPath, LAUNCH_CMD, lpCmdLine);
    } else {
        snprintf(cmdLine, sizeof(cmdLine), "\"%s\" -m mgltools.launch %s", pythonPath, LAUNCH_CMD);
    }

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessA(NULL, cmdLine, NULL, NULL, FALSE, 0, NULL, exePath, &si, &pi)) {
        char msg[1024];
        snprintf(msg, sizeof(msg), "Failed to launch %s runtime:\n%s\n\nError code: %lu", LAUNCH_TITLE, cmdLine, GetLastError());
        MessageBoxA(NULL, msg, LAUNCH_TITLE, MB_ICONERROR | MB_OK);
        return 1;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
}

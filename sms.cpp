#include <windows.h>
#include <stdio.h>

HANDLE hSerial;

void write_com(char *buf)
{
    printf(">> '%s'\n", buf);
    DWORD bytesWritten;
    if (!WriteFile(hSerial, buf, strlen(buf), &bytesWritten, NULL)) {
        fprintf(stderr, "Error writing to COM1\n");
        exit(-1);
    }
    Sleep(300);
}

void read_com()
{
    char buf[256] = {0};
    DWORD bytesRead = 0;
    ReadFile(hSerial, buf, 512, &bytesRead, NULL);
    if (bytesRead > 0) {
        printf("<< '%s'\n", buf);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <phone number> <message>\n", argv[0]);
        return -1;
    }
    hSerial = CreateFile(TEXT("COM1"),
                         GENERIC_READ | GENERIC_WRITE,
                         0,
                         0,
                         OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL,
                         0);
    if (hSerial == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Cannot open COM1\n");
        return -1;
    }
    DCB dcb = {0};
    dcb.DCBlength = sizeof(dcb);
    if (!GetCommState(hSerial, &dcb)) {
        fprintf(stderr, "Cannot get COM1 state\n");
        return -1;
    }
    dcb.BaudRate = CBR_9600;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity = NOPARITY;
    if (!SetCommState(hSerial, &dcb)) {
        fprintf(stderr, "Cannot set COM1 state\n");
        return -1;
    }
    SetupComm(hSerial, 1024, 1024);
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 70;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    if (!SetCommTimeouts(hSerial, &timeouts)) {
        fprintf(stderr, "Cannot set COM1 timeouts\n");
        return -1;
    }
    char buf[512];
    sprintf_s(buf, 512, "AT\r\n");
    write_com(buf);
    read_com();
    sprintf_s(buf, 512, "AT+CMGF=1\r\n");
    write_com(buf);
    read_com();
    sprintf_s(buf, 512, "AT+CMGS=%s\r\n", argv[1]);
    write_com(buf);
    read_com();
    sprintf_s(buf, 512, "%s%c\r\n", argv[2], 26); //Ctrl+Z = 26
    write_com(buf);
    Sleep(700);
    read_com();
    CloseHandle(hSerial);
    return 0;
}
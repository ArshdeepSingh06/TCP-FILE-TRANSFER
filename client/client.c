#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    char filename[100];

    WSAStartup(MAKEWORD(2,2), &wsa);

    printf("Enter file name to send: ");
    scanf("%s", filename);

    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printf("File not found!\n");
        return 1;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection failed!\n");
        return 1;
    }

    // Send fixed-size filename (100 bytes)
    char fixedName[100] = {0};
    strcpy(fixedName, filename);
    send(sock, fixedName, 100, 0);

    // Get file size
    fseek(fp, 0, SEEK_END);
    long long fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Send file size (8 bytes)
    send(sock, (char*)&fileSize, sizeof(fileSize), 0);

    // Send file content
    long long sent = 0;
    int n;
    while ((n = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
        send(sock, buffer, n, 0);
        sent += n;
    }

    shutdown(sock, SD_SEND);

    printf("File sent successfully! (%lld bytes)\n", sent);

    fclose(fp);
    closesocket(sock);
    WSACleanup();

    return 0;
}

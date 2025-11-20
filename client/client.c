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

    printf("========================================\n");
    printf("        TCP FILE TRANSFER - CLIENT      \n");
    printf("========================================\n\n");


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

    printf("Connecting to server...\n");


    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection failed!\n");
        return 1;
    }

    printf("Connection successful.\n\n");

    char fixedName[100] = {0};
    strcpy(fixedName, filename);
    send(sock, fixedName, 100, 0);

    fseek(fp, 0, SEEK_END);
    long long fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    printf("Preparing file: %s\n", filename);
    printf("File size: %lld bytes\n", fileSize);
    printf("Sending file...\n");

    send(sock, (char*)&fileSize, sizeof(fileSize), 0);

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
    printf("----------------------------------------\n");
    printf("        End of Client Process           \n");
    printf("----------------------------------------\n");

    return 0;
}

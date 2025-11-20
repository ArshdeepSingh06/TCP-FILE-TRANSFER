#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsa;
    SOCKET server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];
    char filename[100];
    FILE *fp;
    printf("========================================\n");
    printf("        TCP FILE TRANSFER - SERVER      \n");
    printf("========================================\n");

    WSAStartup(MAKEWORD(2,2), &wsa);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 3);

    printf("Server listening on port %d...\n", PORT);

    new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
    printf("Client connected!\n");

    recv(new_socket, filename, 100, 0);
    printf("Receiving file: %s\n", filename);

    
    long long fileSize = 0;
    recv(new_socket, (char*)&fileSize, sizeof(fileSize), 0);
    printf("Expected size: %lld bytes\n", fileSize);

    fp = fopen(filename, "wb");

    long long total = 0;
    int bytesReceived;

    while (total < fileSize) {
        bytesReceived = recv(new_socket, buffer, BUFFER_SIZE, 0);
        fwrite(buffer, 1, bytesReceived, fp);
        total += bytesReceived;
    }

    printf("File received successfully! (%lld bytes)\n", total);
    printf("Saved as : %s\n\n", filename);

    fclose(fp);
    closesocket(new_socket);
    closesocket(server_fd);
    WSACleanup();

    printf("----------------------------------------\n");
    printf("        End of Server Process           \n");
    printf("----------------------------------------\n");

    return 0;
}

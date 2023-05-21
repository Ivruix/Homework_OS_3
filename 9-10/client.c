#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

int client_socket;

void hanlder(int signal) {
    close(client_socket);
    exit(0);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Server IP address and port not specified\n");
        exit(1);
    }

    signal(SIGINT, hanlder);

    char* server_ip = argv[1];
    int server_port = atoi(argv[2]);

    struct sockaddr_in server_addr;

    // Создание сокета
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Failed to create socket");
        exit(1);
    }

    // Настройка адреса сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    // Установка соединения с сервером
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Failed to connect to the server");
        exit(1);
    }

    char* first_message = "client";
    if (write(client_socket, first_message, strlen(first_message)) == -1) {
        perror("Failed to send first message");
        exit(1);
    }

    printf("Connection to server %s:%d established\n", server_ip, server_port);

    // Цикл поиска клада
    int treasureFound = 0;
    while (!treasureFound) {
        // Получение инструкций от сервера
        char instructions[256];
        int bytesRead = read(client_socket, instructions, sizeof(instructions));
        if (bytesRead == -1) {
            perror("Failed to receive instructions");
            exit(1);
        }

        instructions[bytesRead] = '\0';
        printf("Instructions received: %s\n", instructions);

        if (strcmp(instructions, "stop work") == 0) {
            break;
        }

        sleep(rand() % 2 + 2);

        if (strcmp(instructions, "search section 1") == 0) {
            treasureFound = 1;
        }

        // Отправка сообщения о результатах поиска
        char* message;
        if (treasureFound) {
            message = "treasure found";
            printf("Treasure found!\n");
        }
        else {
            message = "treasure not found";
            printf("Treasure not found\n");
        }
        if (write(client_socket, message, strlen(message)) == -1) {
            perror("Failed to send message");
            exit(1);
        }
    }

    printf("Stopping work\n");

    // Закрытие соединения
    close(client_socket);

    return 0;
}

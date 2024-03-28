#ifndef TCP_H
#define TCP_H

#include <stddef.h>

// Функция прослушивания tcp соединения
// address - адрес соединения
extern int listen_tcp(char* address);

// Принятия соединения для listen
// listener - номер listen
extern int accept_tcp(int listener);

// К какому серверу должен подлючаться клиент
// address - адрес для полдключения
extern int connect_tcp(char* address);

// Закрытие соединения
// conn - номер поделючения (сокета)
extern int close_tcp(int conn);

// Функция для отправки сообщения
// conn - номер поделючения (сокета)
// buffer - буфер сообщения
// size - размер буфера
extern int send_tcp(int conn, char* buffer, size_t size);

// Функция для приема сообщения
// conn - номер поделючения (сокета)
// buffer - буфер сообщения
// size - размер буфера
extern int recv_tcp(int conn, char* buffer, size_t size);

#endif //TCP_H
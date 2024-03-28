#ifdef __linux__
#include <unistd.h>
#include <arpa/inet.h>
#elif _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#warning "there is no library for this platform"
#endif

#if defined(__linux__) || defined(_WIN32)

#include "log.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <thread>
#include <sstream>

typedef enum error_r
{
	WINSOCK_ERR = -1,
	SOCKET_ERR = -2,
	SETOPT_ERR = -3,
	PARSE_ERR = -4,
	BIND_ERR = -5,
	LISTEN_ERR = -6,
	CONNECT_ERR = -7
} error_r;

#include "TCP.h"


static int8_t _parse_address(char* address, char* ipv4, char* port);

// Функция прослушивания tcp соединения
// address - адрес соединения
extern int listen_tcp(char* address)
{
	std::stringstream th_ss;
	th_ss << std::this_thread::get_id();
	uint64_t th_id = std::stoull(th_ss.str());
	int32_t res = 0;
#ifdef _WIN32
	WSADATA wsa;
	if (res = WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		logging(th_id, "Error WSAStartup - code %d - %d", NFILE(__FILE__), __LINE__, NoColor, WINSOCK_ERR, res);
		return WINSOCK_ERR;
	}
#endif
	int listener = socket(AF_INET, SOCK_STREAM, 0);
	// Если ошибка
	if (listener < 0)
	{
		logging(th_id, "Error socket - code %d - %d", NFILE(__FILE__), __LINE__, NoColor, SOCKET_ERR, listener);
		return SOCKET_ERR;
	}
	int8_t param = 1;
	if (res = setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, (char*)(&param), sizeof(int)) < 0)
	{
		logging(th_id, "Error setsockopt - code %d - %d", NFILE(__FILE__), __LINE__, NoColor, SETOPT_ERR, res);
		return SETOPT_ERR;
	}
	char ipv4[16];
	char port[6];
	if (res = _parse_address(address, ipv4, port) != 0)
	{
		logging(th_id, "Error _parse_address - code %d - %d", NFILE(__FILE__), __LINE__, NoColor, PARSE_ERR, res);
		return PARSE_ERR;
	}
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(port));
	addr.sin_addr.s_addr = inet_addr(ipv4);
	if (res = bind(listener, (struct sockaddr*)(&addr), sizeof(addr)) != 0)
	{
		logging(th_id, "Error bind - code %d - %d", NFILE(__FILE__), __LINE__, NoColor, BIND_ERR, res);
		return BIND_ERR;
	}
	if (res = listen(listener, SOMAXCONN) != 0)
	{
		logging(th_id, "Error listen - code %d - %d", NFILE(__FILE__), __LINE__, NoColor, LISTEN_ERR, res);
		return LISTEN_ERR;
	}
	return listener;
}

// Принятия соединения для listen
// listener - номер listen
extern int accept_tcp(int listener)
{
	return accept(listener, NULL, NULL);
}

// К какому серверу должен подлючаться клиент
// address - адрес для полдключения
extern int connect_tcp(char* address)
{
	std::stringstream th_ss;
	th_ss << std::this_thread::get_id();
	uint64_t th_id = std::stoull(th_ss.str());
	int32_t res = 0;
#ifdef _WIN32
	WSADATA wsa;
	if (res = WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		logging(th_id, "Error WSAStartup - code %d - %d", NFILE(__FILE__), __LINE__, NoColor, WINSOCK_ERR, res);
		return WINSOCK_ERR;
	}
#endif
	int conn = socket(AF_INET, SOCK_STREAM, 0);
	// Если ошибка
	if (conn < 0)
	{
		logging(th_id, "Error socket - code %d - %d", NFILE(__FILE__), __LINE__, NoColor, SOCKET_ERR, conn);
		return SOCKET_ERR;
	}
	char ipv4[16];
	char port[6];
	if (res = _parse_address(address, ipv4, port) != 0)
	{
		logging(th_id, "Error _parse_address - code %d - %d", NFILE(__FILE__), __LINE__, NoColor, PARSE_ERR, res);
		return PARSE_ERR;
	}
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(port));
	addr.sin_addr.s_addr = inet_addr(ipv4);
	if (res = connect(conn, (struct sockaddr*)(&addr), sizeof(addr)) != 0)
	{
		logging(th_id, "Error connect - code %d - %d", NFILE(__FILE__), __LINE__, NoColor, CONNECT_ERR, res);
		return CONNECT_ERR;
	}
	return conn;
}

// Закрытие соединения
// conn - номер поделючения (сокета)
extern int close_tcp(int conn)
{
#ifdef __linux___
	return close(conn);
#elif _WIN32
	return closesocket(conn);
#endif
}

// Функция для отправки сообщения
// conn - номер поделючения (сокета)
// buffer - буфер сообщения
// size - размер буфера
extern int send_tcp(int conn, char* buffer, size_t size)
{
	return send(conn, buffer, (int)size, 0);

}

// Функция для приема сообщения
// conn - номер поделючения (сокета)
// buffer - буфер сообщения
// size - размер буфера
extern int recv_tcp(int conn, char* buffer, size_t size)
{
	return recv(conn, buffer, (int)size, 0);
}

// Распаковываем адресс
static int8_t _parse_address(char* address, char* ipv4, char* port)
{
	size_t i = 0, j = 0;
	for (; address[i] != ':'; i++)
	{
		if (address[i] == '\0')
		{
			return 1;
		}
		if (i >= 15)
		{
			return 2;
		}
		ipv4[i] = address[i];
	}
	ipv4[i] = '\0';
	for (i += 1; address[i] != '\0'; i++, j++)
	{
		if (j >= 5)
		{
			return 3;
		}
		port[j] = address[i];
	}
	port[j] = '\0';
	return 0;
}

#endif // defined(__linux__) || defined(_WIN32)
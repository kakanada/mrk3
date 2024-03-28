#ifndef HTTP_H
#define HTTP_H

#include "tcp.h"
#include <stdint.h>
#include <stddef.h>
#include "log.h"
#include "stream.h"

#define METHOD_SIZE 16
#define PATH_SIZE 2048
#define PROTO_SIZE 16
typedef struct HTTPreq
{
	char method[METHOD_SIZE];		// GET | POST | ...
	char path[PATH_SIZE];			// / | /control | ...
	char proto[PROTO_SIZE];			// HTTP/1.1 | ...

	char* elem[64];		// �������� ��������
	char* elem_val[64];	// �������� ���������
	int16_t n_elem;

	// ��� �������
	uint8_t state;		// ���������
	size_t index;	// ������
} HTTPreq;
typedef struct HTTP HTTP;

// �������� ������� HTTP
// address - ����� ����������
// cap - ������������ ���-�� �������
extern HTTP* new_http(char* address, int32_t cap = 128);

// �������� ������� HTTP
// http - HTTP ������
extern void free_http(HTTP* http);

// ������� ������ http
// http - HTTP ������
extern int8_t listen_http(HTTP* http);


// �������� ����� ������� ������������ ����� ������
// HTTP - HTTP ������ � �������� ������� ������
// path - ���� � �������� ����������
// func - ������� ��������� ������� �������
void handle_http(HTTP* http, char* path, void(*func)(int, HTTPreq*));

// �������� ��� ���������� �� ������
// HTTP - HTTP ������ � �������� ������� ������
// path - ���� � �������� ����������
// filename - ���� � ����� ������� ���� ���������
void handle_http(HTTP* http, char* path, char* filename);

// �������� html �����
// conn - ����� ������������
// name_file - ��� ����� ��� ��������
void parsehtml_http(int conn, char* filename);

#endif //HTTP_H
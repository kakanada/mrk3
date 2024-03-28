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

	char* elem[64];		// Название элемента
	char* elem_val[64];	// Значение эелемента
	int16_t n_elem;

	// Для анализа
	uint8_t state;		// Состояние
	size_t index;	// Индекс
} HTTPreq;
typedef struct HTTP HTTP;

// Создание обьекта HTTP
// address - адрес соединения
// cap - максимальное кол-во функций
extern HTTP* new_http(char* address, int32_t cap = 128);

// Удаление обьекта HTTP
// http - HTTP обьект
extern void free_http(HTTP* http);

// Слушаем обьект http
// http - HTTP обьект
extern int8_t listen_http(HTTP* http);


// указание какая функция обрабатывает какой запрос
// HTTP - HTTP обьект с которого прийдет запрос
// path - путь к которому обращаются
// func - функция обработки данного запроса
void handle_http(HTTP* http, char* path, void(*func)(int, HTTPreq*));

// указание что отправлять на запрос
// HTTP - HTTP обьект с которого прийдет запрос
// path - путь к которому обращаются
// filename - путь к файлу который надо выгрузить
void handle_http(HTTP* http, char* path, char* filename);

// Отправка html файла
// conn - номер пользователя
// name_file - имя файла для отправки
void parsehtml_http(int conn, char* filename);

#endif //HTTP_H
#include "http.h"
#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <thread>
#include <sstream>

typedef void (*FUNC)(int, HTTPreq*);
typedef struct HTTP_type_path
{
	FUNC func;					// указатель на функцию
	char filename[128];	// путь файла
	int8_t is_func;				// 0 - выгрузить файл / 1 - выполнить функцию
} HTTP_type_path;
typedef struct HTTP
{
	char* host;					// сам хост (адрес)
	int32_t len;				// кол-во функций используются
	int32_t cap;				// сколько максимально путей может использоваться
	HTTP_type_path* htp;		// указатель что выполнить
	char** tab_;				// таблица с запрошенным путем для вызова функции
} HTTP;
typedef enum FTYPE
{
	FHTTP = 1,
	FCSS = 2,
	FJS = 3,
	FICO = 4,
	FJGEG = 5,
	FPNG = 6,
	FTXT = 7
} FTYPE;

static HTTPreq _new_request(void);
static void _parce_request(HTTPreq* req, char* buffer, size_t size);
static void _null_request(HTTPreq* req);
static void _del_req(HTTPreq* req);
static int8_t _switch_http(HTTP* http, int conn, HTTPreq* req);
static void _page404_http(int conn);
static int8_t _type_file(char* filename);

// Создание обьекта HTTP
// address - адрес соединения
// cap - максимальное кол-во функций
extern HTTP* new_http(char* address, int32_t cap)
{
	std::stringstream th_ss;
	th_ss << std::this_thread::get_id();
	uint64_t th_id = std::stoull(th_ss.str());
	HTTP* http = (HTTP*)malloc(sizeof(HTTP));
	http->cap = cap;
	http->len = 0;
	http->host = (char*)malloc((strlen(address) + 1) * sizeof(char));
	strcpy(http->host, address);
	http->tab_ = (char**)malloc(http->cap * sizeof(char*));
	for (int32_t i = 0; i < http->cap; i++)
	{
		http->tab_[i] = NULL;
	}
	http->htp = (HTTP_type_path*)malloc(http->cap * sizeof(HTTP_type_path));
	logging(th_id, "Generate new http obgect", NFILE(__FILE__), __LINE__, NoColor);
	return http;
}

// Удаление обьекта HTTP
// http - HTTP обьект
extern void free_http(HTTP* http)
{
	std::stringstream th_ss;
	th_ss << std::this_thread::get_id();
	uint64_t th_id = std::stoull(th_ss.str());
	for (int32_t i = 0; i < http->cap; i++)
	{
		if (http->tab_[i] != NULL)
		{
			free(http->tab_[i]);
			http->tab_[i] = NULL;
		}
	}
	free(http->tab_);
	free(http->host);
	free(http->htp);
	free(http);
	logging(th_id, "Delete new http obgect", NFILE(__FILE__), __LINE__, NoColor);
}

// Слушаем обьект http
// http - HTTP обьект
extern int8_t listen_http(HTTP* http)
{
	std::stringstream th_ss;
	th_ss << std::this_thread::get_id();
	uint64_t th_id = std::stoull(th_ss.str());
	int listener = listen_tcp(http->host);
	if (listener < 0)
	{
		logging(th_id, "Error listen http", NFILE(__FILE__), __LINE__, Red);
		return 1;
	}
	while (1)
	{
		//check_thread();
		int conn = accept_tcp(listener);
		if (conn < 0)
		{
			//ошибка соединения
			logging(th_id, "Error connect http", NFILE(__FILE__), __LINE__, Red);
			continue;
		}
		HTTPreq req = _new_request();
		while (1)
		{
			char buffer[BUFSIZ] = { 0 };
			int n = recv_tcp(conn, buffer, BUFSIZ);
			if (n < 0)
			{
				break;
			}
			_parce_request(&req, buffer, n);
			if (n != BUFSIZ)
			{
				break;
			}
		}
		_switch_http(http, conn, &req);
		_del_req(&req);
	}
	close_tcp(listener);
	logging(th_id, "End listen http", NFILE(__FILE__), __LINE__, Yellow);
	return 0;
}

// указание какая функция обрабатывает какой запрос
// HTTP - HTTP обьект с которого прийдет запрос
// path - путь к которому обращаются
// func - функция обработки данного запроса
void handle_http(HTTP* http, char* path, void(*func)(int, HTTPreq*))
{
	std::stringstream th_ss;
	th_ss << std::this_thread::get_id();
	uint64_t th_id = std::stoull(th_ss.str());
	if (http->len == http->cap - 1)
	{
		//произошло переполнение функций
		logging(th_id, "Error max function http table", NFILE(__FILE__), __LINE__, Red);
		return;
	}
	if (func == NULL)
	{
		//Указатель на функцию или путь не указаны
		logging(th_id, "Error func nullptr", NFILE(__FILE__), __LINE__, Red);
		return;
	}
	http->tab_[http->len] = (char*)malloc(sizeof(char) * (strlen(path) + 1));
	strcpy(http->tab_[http->len], path);
	http->htp[http->len].func = func;
	http->htp[http->len].is_func = 1;
	http->len++;
	logging(th_id, "New function or path http tab", NFILE(__FILE__), __LINE__, NoColor);
}

// указание что отправлять на запрос
// HTTP - HTTP обьект с которого прийдет запрос
// path - путь к которому обращаются
// filename - путь к файлу который надо выгрузить
void handle_http(HTTP* http, char* path, char* filename)
{
	std::stringstream th_ss;
	th_ss << std::this_thread::get_id();
	uint64_t th_id = std::stoull(th_ss.str());
	if (http->len == http->cap - 1)
	{
		//произошло переполнение функций
		logging(th_id, "Error max function http table", NFILE(__FILE__), __LINE__, Red);
		return;
	}
	if (filename == NULL)
	{
		//Указатель на функцию или путь не указаны
		logging(th_id, "Error file name nullptr", NFILE(__FILE__), __LINE__, Red);
		return;
	}
	http->tab_[http->len] = (char*)malloc(sizeof(char) * (strlen(path) + 1));
	strcpy(http->tab_[http->len], path);
	strcpy(http->htp[http->len].filename, filename);
	http->htp[http->len].is_func = 0;
	http->len++;
	logging(th_id, "New function or path http tab", NFILE(__FILE__), __LINE__, NoColor);
}

// Отправка html файла
// conn - номер подключения
// name_file - имя файла для отправки
void parsehtml_http(int conn, char* filename)
{
	std::stringstream th_ss;
	th_ss << std::this_thread::get_id();
	uint64_t th_id = std::stoull(th_ss.str());
	char buffer[BUFSIZ];
	switch (_type_file(filename))
	{
	case FHTTP:
		strcpy(buffer, "HTTP/1.1 200 OK\nContent-type: text/html\n\n");
		break;
	case FCSS:
		strcpy(buffer, "HTTP/1.1 200 OK\nContent-type: text/css\n\n");
		break;
	case FJS:
		strcpy(buffer, "HTTP/1.1 200 OK\nContent-type: text/javascript\n\n");
		break;
	case FICO:
		strcpy(buffer, "HTTP/1.1 200 OK\nContent-type: image/ico\n\n");
		break;
	case FJGEG:
		strcpy(buffer, "HTTP/1.1 200 OK\nContent-type: image/jpeg\n\n");
		break;
	case FPNG:
		strcpy(buffer, "HTTP/1.1 200 OK\nContent-type: image/png\n\n");
		break;
	case FTXT:
		strcpy(buffer, "HTTP/1.1 200 OK\nContent-type: image/plain\n\n");
		break;
	default:
		strcpy(buffer, "HTTP/1.1 200 OK\nContent-type: text/plain\n\n");
	}
	size_t readsize = strlen(buffer);
	send_tcp(conn, buffer, readsize);
	FILE* file = fopen(filename, "rb");
	if (file == NULL)
	{
		logging(th_id, "Error unload html file - %s", NFILE(__FILE__), __LINE__, Red, filename);
		return;
	}
	while ((readsize = fread(buffer, sizeof(char), BUFSIZ, file)) != 0)
	{
		send_tcp(conn, buffer, readsize);
	}
	close_tcp(conn);
	fclose(file);
	logging(th_id, "Unload html file - %s", NFILE(__FILE__), __LINE__, NoColor, filename);
}

// Создание HTTPreq для обработк запроса
static HTTPreq _new_request(void)
{
	HTTPreq a;
	a.method[0] = '\0';
	a.path[0] = '\0';
	a.proto[0] = '\0';
	a.n_elem = 0;
	a.state = 0;
	a.index = 0;
	return a;
}

// Удаление request
static void _del_req(HTTPreq* req)
{
	return;
}

//Распаковка запроса
static void _parce_request(HTTPreq* req, char* buffer, size_t size)
{
	std::stringstream th_ss;
	th_ss << std::this_thread::get_id();
	uint64_t th_id = std::stoull(th_ss.str());
	for (size_t i = 0; i < size; i++)
	{
		switch (req->state)
		{
		case 0:		// чтение метода
			if (buffer[i] == ' ' || req->index == METHOD_SIZE - 1)
			{
				req->method[req->index] = '\0';
				_null_request(req);
				continue;
			}
			req->method[req->index] = buffer[i];
			break;
		case 1:		// чтение пути
			if (buffer[i] == ' ' || req->index == PATH_SIZE - 1)
			{
				req->path[req->index] = '\0';
				_null_request(req);
				continue;
			}
			req->path[req->index] = buffer[i];
			break;
		case 2:		// чтение протокола
			if (buffer[i] == '\n' || req->index == PROTO_SIZE - 1)
			{
				req->proto[req->index] = '\0';
				_null_request(req);
				continue;
			}
			req->proto[req->index] = buffer[i];
			break;
		case 3:		// чтение название елемента
			if (req->index == 0)
			{
				req->elem[req->n_elem] = new char[BUFSIZ];
				req->elem_val[req->n_elem] = new char[BUFSIZ];
				req->n_elem += 1;
			}
			if (buffer[i] == ':' || req->index == BUFSIZ - 1)
			{
				req->elem[req->n_elem - 1][req->index] = '\0';
				req->state = 4;
				req->index = 0;
				i++;
				continue;
			}
			req->elem[req->n_elem - 1][req->index] = buffer[i];
			break;
		case 4:		// чтение значение элемента
			if (buffer[i] == '\n' || req->index == BUFSIZ - 1)
			{
				req->elem_val[req->n_elem - 1][req->index] = '\0';
				req->state = 3;
				req->index = 0;
				continue;
			}
			req->elem_val[req->n_elem - 1][req->index] = buffer[i];
			break;
		default:
			return;
		}
		req->index += 1;
	}
	logging(th_id, "Parse request - %s -%s - %s", NFILE(__FILE__), __LINE__, NoColor, req->method, req->path, req->proto);
}

// переход на слудующую стадию распаковки запроса
static void _null_request(HTTPreq* req)
{
	req->state += 1;
	req->index = 0;
}

// Находим необходимую функцию для обработки запроса
// conn - номер подключения
static int8_t _switch_http(HTTP* http, int conn, HTTPreq* req)
{
	std::stringstream th_ss;
	th_ss << std::this_thread::get_id();
	uint64_t th_id = std::stoull(th_ss.str());
	int32_t index = -1;
	for (int32_t i = 0; (i < http->len) && (index == -1); i++)
	{
		if (std::string(req->path).compare(http->tab_[i]) == 0)
		{
			index = i;
		}
	}
	if (index == -1)
	{
		logging(th_id, "No function processing http %s", NFILE(__FILE__), __LINE__, Yellow, req->path);
		_page404_http(conn);
		return 1;
	}
	if (http->htp[index].is_func == 0)
	{
		// отправляем файл
		logging(th_id, "Processing http %s - unload file %s", NFILE(__FILE__), __LINE__, NoColor, req->path, http->htp[index].filename);
		parsehtml_http(conn, http->htp[index].filename);
	}
	else
	{
		// выполняем функцию
		logging(th_id, "Processing http %s - wor function", NFILE(__FILE__), __LINE__, NoColor, req->path);
		http->htp[index].func(conn, req);
	}

	return 0;
}

// Страница не найдена
// conn - номер подключения
static void _page404_http(int conn)
{
	std::stringstream th_ss;
	th_ss << std::this_thread::get_id();
	uint64_t th_id = std::stoull(th_ss.str());
	char header[BUFSIZ] = "HTTP/1.1 404 NotFound\n\n";
	size_t readsize = strlen(header);
	send_tcp(conn, header, readsize);
	FILE* file = fopen((char*)"page404.html", "r");
	if (file == NULL)
	{
		logging(th_id, "Error unload html file - %s", NFILE(__FILE__), __LINE__, Red, (char*)"page404.html");
		return;
	}
	while ((readsize = fread(header, sizeof(char), BUFSIZ, file)) != 0)
	{
		send_tcp(conn, header, readsize);
	}
	fclose(file);
	logging(th_id, "Unload html file - %s", NFILE(__FILE__), __LINE__, NoColor, (char*)"page404.html");
}

// Определяем тип файла
// filename - имя файла
static int8_t _type_file(char* filename)
{
	char* index = filename;
	while ((*filename) != '\0')
	{
		if ((*filename) == '.')
		{
			index = filename;
		}
		filename++;
	}

	char buffer[BUFSIZ];
	strcpy(buffer, index);

	if (std::string(buffer).compare(".html") == 0)
	{
		return FHTTP;
	}
	if (std::string(buffer).compare(".css") == 0)
	{
		return FCSS;
	}
	if (std::string(buffer).compare(".js") == 0)
	{
		return FJS;
	}
	if (std::string(buffer).compare(".ico") == 0)
	{
		return FICO;
	}
	if (std::string(buffer).compare(".jpeg") == 0)
	{
		return FJGEG;
	}
	if (std::string(buffer).compare(".jpg") == 0)
	{
		return FJGEG;
	}
	if (std::string(buffer).compare(".png") == 0)
	{
		return FPNG;
	}
	if (std::string(buffer).compare(".txt") == 0)
	{
		return FTXT;
	}
	
	return 0;
}
#ifndef LOG_H
#define LOG_H

#include <stdio.h> 
#include <stdint.h>

typedef struct LOG
{
	char* filename;
	FILE* file;
	int8_t flagwork;
} LOG;

typedef enum LOG_COLOR
{
	NoColor = -1,
	Write = 0,
	Red = 1,
	Green = 2,
	Yellow = 3
} LOG_COLOR;

// Инициализания
// filename - имя файла
void log_init();

// Добавить запись в лог
// th_id - id потока из которого отправляется сообщение
// mess - сообщение
// codefile - имя файла сообщения
// codeline - строка сообщения
// color - цвет отображения
void logging(int th_od, const char* mess, const char* codefile, int codeline, LOG_COLOR color, ...);

// Освободить структуру
void log_free(void);

// Путь к аутальному логу
char* name_file_log(void);

// Очистка файла от полного пути
// name - полное имя файла
char* NFILE(const char* name);

#endif //LOG_H
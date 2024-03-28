#include "log.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>

typedef enum FWORD
{
	Free = 0,
	Work = 1,
	Create = 2
} FWORK;

// Инициализания
LOG* log = NULL;
void log_init()
{
	char filename[BUFSIZ];
	time_t tim = time(NULL);
	tm* tme = localtime(&tim);
	sprintf(filename, "log_%d.%d.%d_%d-%d-%d.txt", tme->tm_mday, tme->tm_mon + 1, tme->tm_year + 1900, tme->tm_hour, tme->tm_min, tme->tm_sec);

	log = (LOG*)malloc(sizeof(LOG));
	if (log == NULL)
	{
		printf("ERROR!!! Error create log obgect\n");
		return;
	}
	log->flagwork = Create;
	log->filename = (char*)malloc(sizeof(char) * strlen(filename));
	if (log->filename == NULL)
	{
		printf("ERROR!!! Error create log obgect\n");
		free(log);
		log = NULL;
		return;
	}
	strcpy(log->filename, filename);
	log->file = fopen(filename, "w");
	if (log->file == NULL)
	{
		printf("ERORR!!! Error generate log file\n");
		free(log->filename);
		free(log);
		log = NULL;
		return;
	}
	fprintf(log->file, "Generate log - %s\nCompiling a file %s\n", ctime(&tim), __DATE__);
	fflush(log->file);
	log->flagwork = Free;
}

// Добавить запись в лог
// th_id - id потока из которого отправляется сообщение
// mess - сообщение
// codefile - имя файла сообщения
// codeline - строка сообщения
// color - цвет отображения
void logging(int th_id, const char* mess, const char* codefile, int codeline, LOG_COLOR color, ...)
{
	if (log == NULL)
	{
		printf("ERROR!!! Not log obgect\n");
		return;
	}
	if (log->file == NULL)
	{
		printf("ERROR!!! Not opening log file\nMess : %s %d : %s\n", codefile, codeline, mess);
		return;
	}
	unsigned int tw = clock();;
	while (log->flagwork != Free)
	{
		if (clock() - tw > CLOCKS_PER_SEC)
		{
			printf("ERROR!!!\Error timing log file!!!\n");
			log_free();
			return;
		}
	}
	log->flagwork = Work;
	time_t tim = time(NULL);
	tm* tme = localtime(&tim);
	char buffer_zero[BUFSIZ];
	char buffer[BUFSIZ];
	sprintf(buffer_zero, "%02d:%02d:%02d - %d - %s %d - %s\n", tme->tm_hour, tme->tm_min, tme->tm_sec, th_id, codefile, codeline, mess);
	va_list args;
	va_start(args, color);
	vsprintf(buffer, buffer_zero, args);
	va_end(args);
	fwrite(buffer, 1, strlen(buffer), log->file);
	fflush(log->file);

	if (color == LOG_COLOR::NoColor)
	{
		log->flagwork = Free;
		return;
	}
	switch (color)
	{
	case LOG_COLOR::Write:

		break;
	case LOG_COLOR::Red:

		break;
	case LOG_COLOR::Green:

		break;
	}
	printf(buffer);
	log->flagwork = Free;
}

// Освободить структуру
void log_free(void)
{
	unsigned int tw = clock();;
	while (log->flagwork != Free)
	{
		if (clock() - tw > CLOCKS_PER_SEC)
		{
			printf("ERROR!!!\Error timing log file!!!\n");
			log_free();
			return;
		}
	}
	log->flagwork = Work;
	fflush(log->file);
	free(log->filename);
	fclose(log->file);
	free(log);
}

// Путь к аутальному логу
char* name_file_log(void)
{
	if (log == NULL)
		return NULL;
	return log->filename;
}

// Очистка файла от полного пути (используется только для логов)
// name - полное имя файла
char* NFILE(const char* name)
{
	char *a = (char*)name, *b = (char*)name;
	while ((*b) != '\0')
	{
		if ((*b) == '\\')
		{
			a = b;
		}
		b++;
	}
	return a + 1;
}
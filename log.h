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

// �������������
// filename - ��� �����
void log_init();

// �������� ������ � ���
// th_id - id ������ �� �������� ������������ ���������
// mess - ���������
// codefile - ��� ����� ���������
// codeline - ������ ���������
// color - ���� �����������
void logging(int th_od, const char* mess, const char* codefile, int codeline, LOG_COLOR color, ...);

// ���������� ���������
void log_free(void);

// ���� � ���������� ����
char* name_file_log(void);

// ������� ����� �� ������� ����
// name - ������ ��� �����
char* NFILE(const char* name);

#endif //LOG_H
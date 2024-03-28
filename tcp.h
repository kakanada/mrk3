#ifndef TCP_H
#define TCP_H

#include <stddef.h>

// ������� ������������� tcp ����������
// address - ����� ����������
extern int listen_tcp(char* address);

// �������� ���������� ��� listen
// listener - ����� listen
extern int accept_tcp(int listener);

// � ������ ������� ������ ����������� ������
// address - ����� ��� ������������
extern int connect_tcp(char* address);

// �������� ����������
// conn - ����� ����������� (������)
extern int close_tcp(int conn);

// ������� ��� �������� ���������
// conn - ����� ����������� (������)
// buffer - ����� ���������
// size - ������ ������
extern int send_tcp(int conn, char* buffer, size_t size);

// ������� ��� ������ ���������
// conn - ����� ����������� (������)
// buffer - ����� ���������
// size - ������ ������
extern int recv_tcp(int conn, char* buffer, size_t size);

#endif //TCP_H
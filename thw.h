#ifndef TH_STREAM_H
#define TH_STREAM_H

// �������� �� ������� ���������� �������
void check_thread(void);

// �������� ������ ������
// func - ������� ������ ������
// param - ��������� �� ���������
void new_thread(void(*func)(void*), void* param);

#endif //TH_STREAM_H
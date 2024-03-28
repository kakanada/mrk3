#ifndef STREAM_H
#define STREAM_H

#include "log.h"
#include <opencv2/opencv.hpp>
#include <stdio.h>

// �������� ������ ������
// mat - ������ �� �������
// flag - ������ �� ���� �������
// id_stream - ���������� id ������
int8_t new_stream(cv::Mat* mat, int8_t* flag, uint16_t id_stream, int compres);

// �������� ������ ����������� � ������
// conn - ����� �����������
// id_stream - ���������� id ������
int8_t new_conn_stream(int conn, uint16_t id_stream);

// �������� ����������� � ������
// conn - ����� �����������
// id_stream - ���������� id ������
uint8_t delete_connect(int conn, uint16_t id_stream);

// �������� ���� ����������� ������� �� ��� ����������� �����������
void streaming(void);

#endif //STREAM_H
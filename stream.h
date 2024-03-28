#ifndef STREAM_H
#define STREAM_H

#include "log.h"
#include <opencv2/opencv.hpp>
#include <stdio.h>

// Создание нового стрима
// mat - ссылка на матрицу
// flag - ссылка на флаг доступа
// id_stream - заявленный id стрима
int8_t new_stream(cv::Mat* mat, int8_t* flag, uint16_t id_stream, int compres);

// Создание нового подключения к стриму
// conn - номер подключения
// id_stream - заявленный id стрима
int8_t new_conn_stream(int conn, uint16_t id_stream);

// Удаление подключения к стриму
// conn - номер подключения
// id_stream - заявленный id стрима
uint8_t delete_connect(int conn, uint16_t id_stream);

// Отправка всех необходимых стримов во все необходимые подключения
void streaming(void);

#endif //STREAM_H
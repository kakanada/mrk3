#ifndef TH_STREAM_H
#define TH_STREAM_H

// Проверка на наличии законченых потоков
void check_thread(void);

// Создание нового потока
// func - функция нового потока
// param - указатель на параметры
void new_thread(void(*func)(void*), void* param);

#endif //TH_STREAM_H
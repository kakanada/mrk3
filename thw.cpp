#include <thread>
#include <sstream>
#include "log.h"

typedef struct TH
{
	std::thread* thread;
	void(*func)(void*);
	void* param;
	int8_t work;
	TH* next;
} TH;

TH* s_th = NULL;

void _create_thread(void* th);

// Проверка на наличии законченых потоков
void check_thread(void)
{
	std::stringstream th_ss;
	th_ss << std::this_thread::get_id();
	uint64_t th_id = std::stoull(th_ss.str());
	TH* d_th;
	TH* b_th;
	while (s_th != NULL)
	{
		if (s_th->work == 0)
		{
			delete s_th->thread;
			b_th = s_th->next;
			delete s_th;
			s_th = b_th;
		}
		else
		{
			break;
		}
	}

	b_th = s_th;
	while (b_th->next != NULL)
	{
		if (b_th->next->work == 1)
		{
			b_th = b_th->next;
		}
		else
		{
			d_th = b_th->next;
			b_th->next = b_th->next->next;
			logging(th_id, "Delete thread %d", NFILE(__FILE__), __LINE__, Green, d_th->thread->get_id());
			d_th->thread->join();
			delete d_th->thread;
			delete d_th;
		}
	}
}

// Создание нового потока
// func - функция нового потока
// param - указатель на параметры
void new_thread( void(*func)(void*), void* param)
{
	TH* th;
	if (s_th == NULL)
	{
		s_th = new TH;
		th = s_th;
	}
	else
	{
		th = s_th;
		while (th->next != NULL)
			th = th->next;
		th->next = new TH;
		th = th->next;
	}
	th->next = NULL;
	th->work = 1;
	th->func = func;
	th->param = param;
	th->thread = new std::thread(_create_thread, (void*)th);
}

// Обертка функции потока
void _create_thread(void* ptr_th)
{
	std::stringstream th_ss;
	th_ss << std::this_thread::get_id();
	uint64_t th_id = std::stoull(th_ss.str());
	logging(th_id, "Start new thread %d", NFILE(__FILE__), __LINE__, NoColor, std::this_thread::get_id());
	TH* th = (TH*)ptr_th;
	th->func(th->param);
	th->work = 0;
	logging(th_id, "Ending thread %d", NFILE(__FILE__), __LINE__, NoColor, std::this_thread::get_id());
}
#include "stream.h"
#include "tcp.h"
#include <stdio.h>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp> 
#include <string.h>
#include <string>
#include <vector>
#include <thread>

// Период кадров
#define FRAMEPERIOD 33

// Список ошибок
typedef enum error_r
{
	NEW_STREAM_ERR = -1,
	ID_STREAM_ERR = -2,
	GEN_NEW_STREAM_ERR = -3,
	PARAM_NEW_STREAM = -4,
	NEW_CON_ERR = -5,
	SEEK_CON_ERR = -6
} error_r;

typedef struct ELEM_STREAM
{
	int conn[256];				// Номера подключений куда надо вещать до 255 подключений
	uint8_t n_conn;				// Кол-во подключений
	cv::Mat* mat;				// Цказатель на матрицу с изображением
	int8_t* flag;				// Флаг изменения матрицы
	int compres;				// Уровень сжатия

	uint16_t id_stream;
} ELEM_STREAM;

ELEM_STREAM* list_stream = NULL;
uint16_t n_list_stream = 0;

// Создание нового стрима
// mat - ссылка на матрицу
// flag - ссылка на флаг доступа
// id_stream - заявленный id стрима
int8_t new_stream(cv::Mat* mat, int8_t* flag, uint16_t id_stream, int compres)
{
	std::stringstream th_ss;
	th_ss << std::this_thread::get_id();
	uint64_t th_id = std::stoull(th_ss.str());
	if (mat == NULL || flag == NULL)
	{
		logging(th_id, "Error parametr new stream - code %d", NFILE(__FILE__), __LINE__, Red, PARAM_NEW_STREAM);
		return PARAM_NEW_STREAM;
	}
	if (n_list_stream == (uint16_t)(-1))
	{
		logging(th_id, "Error new stream - code %d", NFILE(__FILE__), __LINE__, Red, NEW_STREAM_ERR);
		return NEW_STREAM_ERR;
	}
	for (uint16_t i = 0; i < n_list_stream; i++)
	{
		if (list_stream[i].id_stream == id_stream)
		{
			logging(th_id, "Error new stream id - code %d", NFILE(__FILE__), __LINE__, Red, ID_STREAM_ERR);
			return ID_STREAM_ERR;
		}
	}
	list_stream = (ELEM_STREAM*)realloc(list_stream, sizeof(ELEM_STREAM) * (n_list_stream + 1));
	if (list_stream == NULL)
	{
		logging(th_id, "Error generate new stream - code %d", NFILE(__FILE__), __LINE__, Red, GEN_NEW_STREAM_ERR);
		return GEN_NEW_STREAM_ERR;
	}
	list_stream[n_list_stream].n_conn = 0;
	list_stream[n_list_stream].flag = flag;
	list_stream[n_list_stream].mat = mat;
	list_stream[n_list_stream].id_stream = id_stream;
	list_stream[n_list_stream].compres = compres;
	n_list_stream++;
	logging(th_id, "Generate new stream, id - %d", NFILE(__FILE__), __LINE__, NoColor, (int)id_stream);
	return 0;
}

// Создание нового подключения к стриму
// conn - номер подключения
// id_stream - заявленный id стрима
int8_t new_conn_stream(int conn, uint16_t id_stream)
{
	std::stringstream th_ss;
	th_ss << std::this_thread::get_id();
	uint64_t th_id = std::stoull(th_ss.str());
	uint8_t flag = 0;
	uint16_t id_s = 0;
	for (uint16_t i = 0; i < n_list_stream; i++)
	{
		if (list_stream[i].id_stream == id_stream)
		{
			flag = 1;
			id_s = i;
			break;
		}
	}
	if (flag == 0)
	{
		logging(th_id, "Error new connect stream %d - code %d", NFILE(__FILE__), __LINE__, Red, id_stream, ID_STREAM_ERR);
		return ID_STREAM_ERR;
	}

	if (list_stream[id_s].n_conn == 255)
	{
		logging(th_id, "Error max connect stream - code %d", NFILE(__FILE__), __LINE__, Red, id_s, NEW_CON_ERR);
		return NEW_CON_ERR;
	}

	list_stream[id_s].conn[list_stream[id_s].n_conn] = conn;
	list_stream[id_s].n_conn += 1;
	char buffer[BUFSIZ] = "HTTP/1.1 200 OK\nContent-type: multipart/x-mixed-replace;boundary=--boundary\n\n";
	size_t readsize = strlen(buffer);
	send_tcp(conn, buffer, readsize);
	logging(th_id, "Start stream mjpeg to conn %d stream %d", NFILE(__FILE__), __LINE__, NoColor, conn, list_stream[id_s].id_stream);
	return 0;
}

// Удаление подключения к стриму
// conn - номер подключения
// id_stream - заявленный id стрима
uint8_t delete_connect(int conn, uint16_t id_stream)
{
	std::stringstream th_ss;
	th_ss << std::this_thread::get_id();
	uint64_t th_id = std::stoull(th_ss.str());
	uint8_t flag = 0;
	uint16_t id_s = 0;
	for (uint16_t i = 0; i < n_list_stream; i++)
	{
		if (list_stream[i].id_stream == id_stream)
		{
			flag = 1;
			id_s = i;
			break;
		}
	}
	if (flag == 0)
	{
		logging(th_id, "Error seek stream %d - code %d", NFILE(__FILE__), __LINE__, Red, id_stream, ID_STREAM_ERR);
		return ID_STREAM_ERR;
	}
	uint8_t j = 0;
	for (uint8_t i = 0; i + j < list_stream[id_s].n_conn; i++)
	{
		if (list_stream[id_s].conn[i] == conn)
		{
			close_tcp(conn);
			j++;
		}
		logging(th_id, "Delete stream mjpeg to conn %d stream %d", NFILE(__FILE__), __LINE__, NoColor, conn, list_stream[id_s].id_stream);
		list_stream[id_s].conn[i] = list_stream[id_s].conn[i + j];
	}
	if (j == 0)
	{
		logging(th_id, "Error seek conn %d to stream %s - code %d", NFILE(__FILE__), __LINE__, Red, conn, id_s, SEEK_CON_ERR);
		return SEEK_CON_ERR;
	}
	list_stream[id_s].n_conn -= j;
	return 0;
}

// Отправка всех необходимых стримов во все необходимые подключения
void streaming(void)
{
	std::stringstream th_ss;
	th_ss << std::this_thread::get_id();
	uint64_t th_id = std::stoull(th_ss.str());
	for (uint16_t i_s = 0; i_s < n_list_stream; i_s++)
	{
		if (list_stream[i_s].mat->rows * list_stream[i_s].mat->cols == 0)
		{
			continue;
		}
		std::vector<uchar> data_encode;
		std::vector<int> param = std::vector<int>(2);
		char buffer[BUFSIZ];
		param[0] = cv::IMWRITE_JPEG_QUALITY;
		param[1] = list_stream[i_s].compres;
		unsigned int tw = clock();
		while ((*(list_stream[i_s].flag)) != 0)
		{
			if (clock() - tw > CLOCKS_PER_SEC)
			{
				logging(th_id, "ERROR!!!\Error timing stream %d!!!", NFILE(__FILE__), __LINE__, Red, list_stream[i_s].id_stream);
				break;
			}
		}
		(*(list_stream[i_s].flag)) = 1;
		cv::imencode(".jpg", *(list_stream[i_s].mat), data_encode, param);
		(*(list_stream[i_s].flag)) = 0;
		std::string str = std::string(data_encode.begin(), data_encode.end());
		sprintf(buffer, "--boundary\nContent-Type: image/jpeg\nContent-Length: %d\n\n", (int)data_encode.size());
		size_t readsize = strlen(buffer);
		for (uint8_t i_c = 0; i_c < list_stream[i_s].n_conn; i_c++)
		{
			int res_b = send_tcp(list_stream[i_s].conn[i_c], buffer, readsize);
			int res_i = send_tcp(list_stream[i_s].conn[i_c], (char*)str.c_str(), data_encode.size());
			if (res_i < 0 || res_b < 0)
			{
				delete_connect(list_stream[i_s].conn[i_c], list_stream[i_s].id_stream);
			}
		}
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(FRAMEPERIOD));
}
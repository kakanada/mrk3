#ifndef CAM_H
#define CAM_H

#include <opencv2/opencv.hpp>

typedef struct CAP_STE
{
	cv::Mat* mat;			// указатель на матрицу с изображением
	int8_t flag;			// флаг изменения матрицы
};

// Функция работы камеры
// ptr_cam_ex - структура для обмена изображением камеры
// ptr_cam_ex - структура для обмена изображением карты глубины
void cam_work(CAP_STE* ptr_cam_ex, CAP_STE* ptr_com_ex);

#endif //CAM_H
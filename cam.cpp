#include "cam.h"
#include "log.h"
#include <thread>
#include <sstream>

#define FRAMEPERIOD 33

// Функция работы камеры
// void* ptr = CAP_STE*
void cam_work(CAP_STE* ptr_cam_ex, CAP_STE* ptr_com_ex)
{
	std::stringstream th_ss;
	th_ss << std::this_thread::get_id();
	uint64_t th_id = std::stoull(th_ss.str());
	cv::VideoCapture cap(0);
	if (!cap.isOpened()) {
		logging(th_id, "ERROR!!! Error start work capture", NFILE(__FILE__), __LINE__, Red);
		exit(EXIT_FAILURE);
	}
	logging(th_id, "Start work capture", NFILE(__FILE__), __LINE__, NoColor);
	while (1)
	{
		while (ptr_cam_ex->flag == 1);
		ptr_cam_ex->flag = 1;
		cap >> ptr_cam_ex->mat[0];
		ptr_cam_ex->flag = 0;
		std::this_thread::sleep_for(std::chrono::milliseconds(FRAMEPERIOD));
	}
}
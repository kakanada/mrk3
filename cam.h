#ifndef CAM_H
#define CAM_H

#include <opencv2/opencv.hpp>

typedef struct CAP_STE
{
	cv::Mat* mat;			// ��������� �� ������� � ������������
	int8_t flag;			// ���� ��������� �������
};

// ������� ������ ������
// ptr_cam_ex - ��������� ��� ������ ������������ ������
// ptr_cam_ex - ��������� ��� ������ ������������ ����� �������
void cam_work(CAP_STE* ptr_cam_ex, CAP_STE* ptr_com_ex);

#endif //CAM_H
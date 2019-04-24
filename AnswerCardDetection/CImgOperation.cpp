#include "pch.h"
#include "CImgOperation.h"


CImgOperation::CImgOperation()
{
	// ����load����ͬʱʹ��
	__IoadImg();
	//__loadImgByCamera();
	__ImgToGrayImg();
	__GrayImgToBinImg();
	__ROI_ChoiceQuestion();
	__AnswerOfChoiceQuestion();
}


CImgOperation::~CImgOperation()
{
}


// ����ͼƬ
void CImgOperation::__IoadImg()
{
	cout << "input image's path" << endl;
	cin >> m_s_ImgPath;
	// ����һ��ͼƬ    ��������
	Mat img = imread(m_s_ImgPath);
	//Mat img = imread("C:\\Users\\10614\Desktop\\�Ӿ����\\test_01.jpg");
	resize(img, m_mat_Img, Size(img.cols / 2, img.rows / 2), 0, 0, INTER_LINEAR);
}


// �ҶȻ�
void CImgOperation::__ImgToGrayImg()
{
	// �ҶȻ�
	cvtColor(m_mat_Img, m_mat_GrayImg, CV_BGR2GRAY);
}


// ��ֵ��
void CImgOperation::__GrayImgToBinImg()
{
	// �����Ӧ�㷨��ֵ��
	adaptiveThreshold(m_mat_GrayImg, m_mat_BinImg, 255, 0, 1, 101, 10);
}


Mat CImgOperation::GetImg()
{
	return Mat(m_mat_Img);
}


Mat CImgOperation::GetGrayImg()
{
	return Mat(m_mat_GrayImg);
}


Mat CImgOperation::GetBinImg()
{
	return Mat(m_mat_BinImg);
}


void CImgOperation::ShowImg()
{
	imshow("ͼƬ(�������ⰴ���˳�)", m_mat_Img);
	waitKey(0);
}


void CImgOperation::ShowGrayImg()
{
	imshow("�ҶȻ�(�������ⰴ���˳�)", m_mat_GrayImg);
	waitKey(0);
}


void CImgOperation::ShowBinImg()
{
	imshow("��ֵ��(�������ⰴ���˳�)", m_mat_BinImg);
	waitKey(0);
}


Mat CImgOperation::GetChoiceQuestionImg()
{
	return Mat(m_mat_ChoiceQuestionImg);
}


void CImgOperation::ShowChoiceQuestionImg()
{
	imshow("ѡ����(�������ⰴ���˳�)", m_mat_ChoiceQuestionImg);
	waitKey(0);
}


// ���������ͨ��
Mat CImgOperation::__GenMaximumConnectedDomain(Mat src)
{
	//������ͨ��
	vector<vector<Point>>contours;
	findContours(src, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	// contours:��⵽��������ÿ�����������Ե���������ʽ���д洢��ʹ��point���͵�vector��ʾ 
	// mode��������ģʽ������ͨ��cv::RetrievalModes()�鿴��ϸ��Ϣ
	// RETR_EXTERNAL:��ʾֻ����������������������������hierarchy[i][2]=hierarchy[i][3]=-1 
	// RETR_LIST:��ȡ������������������list�У����������������ȼ���ϵ
	// RETR_CCOMP : ��ȡ��������������������֯��˫��ṹ(two - level hierarchy), ����Ϊ��ͨ�����Χ�߽磬�β�λ�ڲ�߽�
	// RETR_TREE : ��ȡ�������������½�����״�����ṹ
	// RETR_FLOODFILL������û�н��ܣ�Ӧ���Ǻ�ˮ��䷨
	// method:�������Ʒ�������ͨ��cv::ContourApproximationModes()�鿴��ϸ��Ϣ 
	// CHAIN_APPROX_NONE����ȡÿ��������ÿ�����أ����ڵ������������λ�ò����1
	// CHAIN_APPROX_SIMPLE��ѹ��ˮƽ���򣬴�ֱ���򣬶Խ��߷����Ԫ�أ�ֵ�����÷�����ص����꣬���һ����������ֻ��4����������������Ϣ
	// CHAIN_APPROX_TC89_L1��CHAIN_APPROX_TC89_KCOSʹ��Teh - Chinl���ƽ��㷨�е�һ��

	//�����ͨ��
	vector<Point>maxContour;
	double maxArea = 0.0;
	// size_t��һ��������ص��޷������ͣ�������Ƶ��㹻���Ա��ܹ��ڴ����������Ĵ�С��
	for (size_t i = 0; i < contours.size(); i++)
	{
		// contourArea()����������
		double area = contourArea(contours[i]);
		if (area > maxArea)
		{
			maxArea = area;
			maxContour = contours[i];
		}
	}
	//ת��Ϊ���ο�(boundingbox) ���������Ĵ�ֱ�߽���С���Σ���������ͼ�����±߽�ƽ�е�
	Rect maxRect = boundingRect(maxContour);
	// ѡȡ�ֲ����ο�
	Mat result = src(maxRect);
	return Mat(result);
}


// �ҵ�ѡ��������
void CImgOperation::__ROI_ChoiceQuestion()
{
	Mat ROIBox = __GenMaximumConnectedDomain(m_mat_BinImg);
	Mat ROIChoiceBox = __GenMaximumConnectedDomain(~ROIBox);
	ROIChoiceBox = ~ROIChoiceBox;
	//ѡ���ⲿ�ֱ߽紦��
	m_mat_ChoiceQuestionImg = ROIChoiceBox(Rect(0, 75, ROIChoiceBox.cols - 20, ROIChoiceBox.rows - 130));
}


void CImgOperation::__AnswerOfChoiceQuestion()
{
	vector<vector<Point>> contours, answer;
	// ���ҵ����е�����
	findContours(m_mat_ChoiceQuestionImg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	Mat copyImg = m_mat_ChoiceQuestionImg.clone();
	// ������������
	drawContours(copyImg, contours, -1, Scalar(0, 255, 0), 1, 8);
	//������ȷ������Ӿ��μ���С��Ӿ��� 
	//����Rect���͵�vector����boundRect�������Ӿ��Σ���ʼ����СΪcontours.size()����������  
	vector<Rect> boundRect(contours.size());
	//����ÿ������  
	for (size_t i = 0; i < contours.size(); i++)
	{
		double area = contourArea(contours[i]);
		if (area > 300 && area < 1500)
		{
			answer.push_back(contours[i]);
			//���������㼯��ȷ��������Ӿ��β�����
			boundRect[i] = boundingRect(Mat(contours[i]));
			//���������Ӿ��ε����Ͻ����꼰���  
			int width = boundRect[i].width;
			int height = boundRect[i].height;
			int x = boundRect[i].x;
			int y = boundRect[i].y;
			//�û����η�����������Ӿ���  
			rectangle(copyImg, Rect(x, y, width, height), Scalar(255, 0, 0), -1, 8, 0);
		}
	}
	/*imshow("��Ӿ���(�������ⰴ���˳�)", copyImg);
	waitKey(0);*/
	//�洢ͿĨ���ĵ�
	vector<Moments> mu(answer.size());
	for (int i = 0; i < answer.size(); i++)
	{
		mu[i] = moments(answer[i], false);

	}
	//  �������ľ�:
	vector<Point2f> mc(answer.size());
	for (int i = 0; i < answer.size(); i++)
	{
		mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
	}
	// todo ����ѡ����
	int y = copyImg.rows / 15;
	int x = copyImg.cols / 20;
	int answers[15][20] = { 0 };
	for (size_t j = 0; j < mc.size(); j++)
	{
		int x_index = mc[j].x / x;
		int y_index = mc[j].y / y;
		answers[y_index][x_index] = 1;
	}
	for (int i = 0; i < 15; i++)
	{
		if (i < 5)
		{
			for (int j = 1; j < 5; j++)
			{
				if (answers[i][j] == 1)
				{
					m_int_Answer[i][j - 1] = 1;
				}
			}
			for (int k = 6; k < 10; k++)
			{
				if (answers[i][k] == 1)
				{
					m_int_Answer[i + 5][k - 6] = 1;
				}
			}
			for (int l = 11; l < 15; l++)
			{
				if (answers[i][l] == 1)
				{
					m_int_Answer[i + 10][l - 11] = 1;
				}
			}
			for (int m = 16; m < 20; m++)
			{
				if (answers[i][m] == 1)
				{
					m_int_Answer[i + 15][m - 16] = 1;
				}
			}
		}
		else if (i < 10)
		{
			for (int j = 1; j < 5; j++)
			{
				if (answers[i][j] == 1)
				{
					m_int_Answer[i + 15][j - 1] = 1;
				}
			}
			for (int k = 6; k < 10; k++)
			{
				if (answers[i][k] == 1)
				{
					m_int_Answer[i + 20][k - 6] = 1;
				}
			}
			for (int l = 11; l < 15; l++)
			{
				if (answers[i][l] == 1)
				{
					m_int_Answer[i + 25][l - 11] = 1;
				}
			}
			for (int m = 16; m < 20; m++)
			{
				if (answers[i][m] == 1)
				{
					m_int_Answer[i + 30][m - 16] = 1;
				}
			}
		}
		else
		{
			for (int j = 1; j < 5; j++)
			{
				if (answers[i][j] == 1)
				{
					m_int_Answer[i + 30][j - 1] = 1;
				}
			}
			for (int k = 6; k < 10; k++)
			{
				if (answers[i][k] == 1)
				{
					m_int_Answer[i + 35][k - 6] = 1;
				}
			}
		}
	}
}


void CImgOperation::ShowAnswers()
{
	for (int i = 0; i < 50; i++)
	{
		if (i < 9)
			cout << i + 1 << " :";
		else
			cout << i + 1 << ":";
		if (m_int_Answer[i][0])
			cout << "A ";
		if (m_int_Answer[i][1])
			cout << "B ";
		if (m_int_Answer[i][2])
			cout << "C ";
		if (m_int_Answer[i][3])
			cout << "D ";
		cout << "    ";
		if ((i + 1) % 5 == 0)
		{
			cout << endl;
		}
	}
}


void CImgOperation::__loadImgByCamera()
{
	VideoCapture capture;
	Mat frame;
	//ע����������Ӳ��֣�admin:123���˺����룩��
	//@����֮����Ǿ�����ip��ַ(��app�󣬵���·�����IP����ͷ����������������ʾ������ip)
	//����http://<USERNAME>:<PASSWORD>@<IP_ADDRESS>/<the value of src>
	capture.open("http://admin:admin@192.168.1.101:8081");
	//ѭ����ʾÿһ֡
	bool bstop = false;
	while (!bstop)
	{
		capture >> frame;//��ȡ��ǰÿһ֡����
		m_mat_Img = frame;
		GaussianBlur(frame, frame, Size(5, 5), 1.0, 1.5); //��˹�˲�
		Mat edge;
		Canny(frame, edge, 0, 30, 3);					//��Ե����
		imshow("��ȡ��Ƶ", edge);//��ʾ��ǰ֡
		imwrite("C:\\Users\\10614\\Desktop\\test.jpg", edge);
		if (cv::waitKey(30) >= 0)		//30ms  �ȴ�����
		{
			bstop = true;
		}
	}
	if (capture.isOpened())
		capture.release();
}

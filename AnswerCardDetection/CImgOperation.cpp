#include "pch.h"
#include "CImgOperation.h"


CImgOperation::CImgOperation()
{
	// 两个load不可同时使用
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


// 载入图片
void CImgOperation::__IoadImg()
{
	cout << "input image's path" << endl;
	cin >> m_s_ImgPath;
	// 读入一张图片    测试用例
	Mat img = imread(m_s_ImgPath);
	//Mat img = imread("C:\\Users\\10614\Desktop\\视觉检测\\test_01.jpg");
	resize(img, m_mat_Img, Size(img.cols / 2, img.rows / 2), 0, 0, INTER_LINEAR);
}


// 灰度化
void CImgOperation::__ImgToGrayImg()
{
	// 灰度化
	cvtColor(m_mat_Img, m_mat_GrayImg, CV_BGR2GRAY);
}


// 二值化
void CImgOperation::__GrayImgToBinImg()
{
	// 最佳适应算法二值化
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
	imshow("图片(按下任意按键退出)", m_mat_Img);
	waitKey(0);
}


void CImgOperation::ShowGrayImg()
{
	imshow("灰度化(按下任意按键退出)", m_mat_GrayImg);
	waitKey(0);
}


void CImgOperation::ShowBinImg()
{
	imshow("二值化(按下任意按键退出)", m_mat_BinImg);
	waitKey(0);
}


Mat CImgOperation::GetChoiceQuestionImg()
{
	return Mat(m_mat_ChoiceQuestionImg);
}


void CImgOperation::ShowChoiceQuestionImg()
{
	imshow("选择题(按下任意按键退出)", m_mat_ChoiceQuestionImg);
	waitKey(0);
}


// 生成最大连通域
Mat CImgOperation::__GenMaximumConnectedDomain(Mat src)
{
	//查找连通域
	vector<vector<Point>>contours;
	findContours(src, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	// contours:检测到的轮廓，每个轮廓都是以点向量的形式进行存储即使用point类型的vector表示 
	// mode轮廓检索模式，可以通过cv::RetrievalModes()查看详细信息
	// RETR_EXTERNAL:表示只检测最外层轮廓，对所有轮廓设置hierarchy[i][2]=hierarchy[i][3]=-1 
	// RETR_LIST:提取所有轮廓，并放置在list中，检测的轮廓不建立等级关系
	// RETR_CCOMP : 提取所有轮廓，并将轮廓组织成双层结构(two - level hierarchy), 顶层为连通域的外围边界，次层位内层边界
	// RETR_TREE : 提取所有轮廓并重新建立网状轮廓结构
	// RETR_FLOODFILL：官网没有介绍，应该是洪水填充法
	// method:轮廓近似方法可以通过cv::ContourApproximationModes()查看详细信息 
	// CHAIN_APPROX_NONE：获取每个轮廓的每个像素，相邻的两个点的像素位置差不超过1
	// CHAIN_APPROX_SIMPLE：压缩水平方向，垂直方向，对角线方向的元素，值保留该方向的重点坐标，如果一个矩形轮廓只需4个点来保存轮廓信息
	// CHAIN_APPROX_TC89_L1和CHAIN_APPROX_TC89_KCOS使用Teh - Chinl链逼近算法中的一种

	//最大连通域
	vector<Point>maxContour;
	double maxArea = 0.0;
	// size_t是一种数据相关的无符号类型，它被设计得足够大以便能够内存中任意对象的大小。
	for (size_t i = 0; i < contours.size(); i++)
	{
		// contourArea()轮廓面积检测
		double area = contourArea(contours[i]);
		if (area > maxArea)
		{
			maxArea = area;
			maxContour = contours[i];
		}
	}
	//转换为矩形框(boundingbox) 计算轮廓的垂直边界最小矩形，矩形是与图像上下边界平行的
	Rect maxRect = boundingRect(maxContour);
	// 选取局部矩形框
	Mat result = src(maxRect);
	return Mat(result);
}


// 找到选择题区域
void CImgOperation::__ROI_ChoiceQuestion()
{
	Mat ROIBox = __GenMaximumConnectedDomain(m_mat_BinImg);
	Mat ROIChoiceBox = __GenMaximumConnectedDomain(~ROIBox);
	ROIChoiceBox = ~ROIChoiceBox;
	//选择题部分边界处理
	m_mat_ChoiceQuestionImg = ROIChoiceBox(Rect(0, 75, ROIChoiceBox.cols - 20, ROIChoiceBox.rows - 130));
}


void CImgOperation::__AnswerOfChoiceQuestion()
{
	vector<vector<Point>> contours, answer;
	// 查找到所有的轮廓
	findContours(m_mat_ChoiceQuestionImg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	Mat copyImg = m_mat_ChoiceQuestionImg.clone();
	// 绘制所有轮廓
	drawContours(copyImg, contours, -1, Scalar(0, 255, 0), 1, 8);
	//由轮廓确定正外接矩形及最小外接矩形 
	//定义Rect类型的vector容器boundRect存放正外接矩形，初始化大小为contours.size()即轮廓个数  
	vector<Rect> boundRect(contours.size());
	//遍历每个轮廓  
	for (size_t i = 0; i < contours.size(); i++)
	{
		double area = contourArea(contours[i]);
		if (area > 300 && area < 1500)
		{
			answer.push_back(contours[i]);
			//由轮廓（点集）确定出正外接矩形并绘制
			boundRect[i] = boundingRect(Mat(contours[i]));
			//、获得正外接矩形的左上角坐标及宽高  
			int width = boundRect[i].width;
			int height = boundRect[i].height;
			int x = boundRect[i].x;
			int y = boundRect[i].y;
			//用画矩形方法绘制正外接矩形  
			rectangle(copyImg, Rect(x, y, width, height), Scalar(255, 0, 0), -1, 8, 0);
		}
	}
	/*imshow("外接矩形(按下任意按键退出)", copyImg);
	waitKey(0);*/
	//存储涂抹中心点
	vector<Moments> mu(answer.size());
	for (int i = 0; i < answer.size(); i++)
	{
		mu[i] = moments(answer[i], false);

	}
	//  计算中心矩:
	vector<Point2f> mc(answer.size());
	for (int i = 0; i < answer.size(); i++)
	{
		mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
	}
	// todo 处理选择题
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
	//注意下面的连接部分，admin:123（账号密码），
	//@符号之后的是局域网ip地址(打开app后，点击下方“打开IP摄像头服务器”，会有显示局域网ip)
	//即：http://<USERNAME>:<PASSWORD>@<IP_ADDRESS>/<the value of src>
	capture.open("http://admin:admin@192.168.1.101:8081");
	//循环显示每一帧
	bool bstop = false;
	while (!bstop)
	{
		capture >> frame;//读取当前每一帧画面
		m_mat_Img = frame;
		GaussianBlur(frame, frame, Size(5, 5), 1.0, 1.5); //高斯滤波
		Mat edge;
		Canny(frame, edge, 0, 30, 3);					//边缘处理
		imshow("读取视频", edge);//显示当前帧
		imwrite("C:\\Users\\10614\\Desktop\\test.jpg", edge);
		if (cv::waitKey(30) >= 0)		//30ms  等待按键
		{
			bstop = true;
		}
	}
	if (capture.isOpened())
		capture.release();
}

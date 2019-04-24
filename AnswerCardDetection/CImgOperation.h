#pragma once
class CImgOperation
{
public:
	CImgOperation();
	~CImgOperation();
private:
	Mat m_mat_Img;
	Mat m_mat_GrayImg;
	Mat m_mat_BinImg;
	Mat m_mat_ChoiceQuestionImg;
	string m_s_ImgPath;
	int m_int_Answer[50][4] = {0};
private:
	// 载入图片
	void __IoadImg();
	// 灰度化
	void __ImgToGrayImg();
	// 二值化
	void __GrayImgToBinImg();
	// 生成最大连通域
	Mat __GenMaximumConnectedDomain(Mat src);
	// 找到选择题区域
	void __ROI_ChoiceQuestion();
	void __AnswerOfChoiceQuestion();
	void __loadImgByCamera();
public:
	Mat GetImg();
	Mat GetGrayImg();
	Mat GetBinImg();
	void ShowImg();
	void ShowGrayImg();
	void ShowBinImg();
	Mat GetChoiceQuestionImg();
	void ShowChoiceQuestionImg();
	void ShowAnswers();
};


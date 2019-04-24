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
	// ����ͼƬ
	void __IoadImg();
	// �ҶȻ�
	void __ImgToGrayImg();
	// ��ֵ��
	void __GrayImgToBinImg();
	// ���������ͨ��
	Mat __GenMaximumConnectedDomain(Mat src);
	// �ҵ�ѡ��������
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


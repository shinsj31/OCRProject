#pragma once
#include "opencv2\opencv.hpp"
#include <iostream>
#define MAX_DATA_COUNT 2000
#define COLOR_CHECK_RANGE 50

using namespace cv;
using namespace std;
struct  Letter
{
	//���� ������ �ϳ��� ��Ÿ���� ����ü. �츮�� 32*48�� ũ��� �����ϳ��� �����Ѵ�.
	char value;
	unsigned char img[32 * 48];
};
struct my_Rect
{
	Point start;
	Point end;
};
struct Data
{
	Letter letter;
	my_Rect rect;
	bool isFixed;
	bool nextIsSpace;	//�� ���� ������ ���⸦ �߰��ؾ��ϴ°�?
};
struct AllData {
	//�̹��� ���� ��� ���� �����͸� �����ϴ� ����ü
	int count;
	Data data[MAX_DATA_COUNT];
};

class OCR
{
private:
	Mat img;
	AllData all;
public:
	OCR();
	~OCR();

	void setImg(String path);
	//void CreateStandard(Mat image);
	void ParsingStepFirst();
	void ParsingStepSecond(int top, int bottom);
	void ParsingStepThird(my_Rect* rect);
	void AddSpaces(int startCount, int endCount, int stdGap);
	void ShowResultImage(int height, int width, int top, int startX);
};
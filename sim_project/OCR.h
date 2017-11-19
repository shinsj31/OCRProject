#pragma once
#include "opencv2\opencv.hpp"
#include <iostream>
#define MAX_DATA_COUNT 2000
#define COLOR_CHECK_RANGE 50

using namespace cv;
using namespace std;
struct  Letter
{
	//글자 데이터 하나를 나타내는 구조체. 우리는 32*48의 크기로 글자하나를 저장한다.
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
	bool nextIsSpace;	//이 글자 다음에 띄어쓰기를 추가해야하는가?
};
struct AllData {
	//이미지 내의 모든 글자 데이터를 저장하는 구조체
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
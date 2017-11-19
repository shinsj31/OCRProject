#include "OCR.h"



OCR::OCR()
{
}


OCR::~OCR()
{
}

void OCR::setImg(String path)
{
	img = imread(path, IMREAD_GRAYSCALE);
}

/*Step1. 전체 이미지에 대하여 라인 구분(세로선 찾기)*/
void OCR::ParsingStepFirst()
{
	all.count = 0;

	int nWidth = img.cols;
	int nHeight = img.rows;
	int top, bottom;

	unsigned char pixelVal;

	bool isLetter;
	bool prevLineState = false;

	//이미지의 가로 전체를 탐색하지 않고 중간만 탐색해서 대략적인 높이를 알아낸다.
	int xStart = 0;
	int xEnd = nWidth;

	for (int y = 0; y < nHeight; y++)
	{
		isLetter = false;
		for (int x = xStart; x < xEnd; x++)
		{
			pixelVal = img.at<unsigned char>(y, x);

			if (pixelVal < COLOR_CHECK_RANGE)
			{//글자가 있는 라인 발견
				isLetter = true;
				break;
			}
		}
		if (isLetter)
		{//글자라인일 때
			if (!prevLineState)
			{//prevLineState는 이전 라인이 배경일 때 false이다.
			 //따라서 이전라인이 배경이고, 글자라인을 찾았다면, 이 라인은 글자 상단에 해당하는 라인이 된다.
				top = y;
			}
		}
		else
		{
			if (prevLineState)
			{//이전 라인 상태가 글자라인이었고, 현재 배경이면 이 라인의 전 라인이 글자 하단에 해당하는 라인이 된다.
				bottom = y-1;

				//한 줄의 top과 bottom을 찾았으므로 가로경계를 찾아내는 단계 진행
				ParsingStepSecond(top, bottom);
			}
		}
		prevLineState = isLetter;
	}
}

/*Step2. 하나의 라인 내에서 가로선 구분하기.
가로선 구분 후 마지막에는 줄바꿈을 추가해준다.(라인이 바뀌는 것을 표시하기 위해)*/
void OCR::ParsingStepSecond(int top, int bottom)
{
	int nWidth = img.cols;

	unsigned char pixelVal;
	bool isLetter;
	bool prevLineState = false;


	for (int x = 0; x < nWidth; x++)
	{
		isLetter = false;
		for (int y = top; y <= bottom; y++)
		{
			pixelVal = img.at<unsigned char>(y, x);
			
			if (pixelVal <COLOR_CHECK_RANGE)
			{//글자영역 찾음
				isLetter = true;
				break;
			}
		}
		if (isLetter)
		{
			if (!prevLineState)
			{
				all.data[all.count].rect.start.x = x;
				all.data[all.count].rect.start.y = top;
			}
		}
		else
		{
			if (prevLineState)
			{
				all.data[all.count].rect.end.x = x - 1;
				all.data[all.count].rect.end.y = bottom;

				//경계선 보정작업
				ParsingStepThird(&all.data[all.count].rect);
				ShowResultImage(all.data[all.count].rect.end.y - all.data[all.count].rect.start.y, all.data[all.count].rect.end.x - all.data[all.count].rect.start.x, all.data[all.count].rect.start.y, all.data[all.count].rect.start.x);
				all.count+=1;
			}
		}
		prevLineState = isLetter;
	}
	//줄바꿈 문자 추가하기
	all.data[all.count].isFixed = true;
	all.data[all.count++].letter.value = '\n';
}

void OCR::ParsingStepThird(my_Rect * rect)
{
	bool isLetter;
	unsigned char pixelVal;

	//데이터의 윗쪽영역 탐색 필요하다면 축소
	for (int y = rect->start.y;; y++)
	{
		isLetter = false;
		for (int x = rect->start.x; x <= rect->end.x; x++)
		{
			pixelVal = img.at<unsigned char>(y, x);
			if (pixelVal < COLOR_CHECK_RANGE)
			{
				//문자라인 발견시 top을 변경해준다.
				rect->start.y = y;
				isLetter = true;
				break;
			}
		}
		if (isLetter)	//이미 글자라인이라면 탐색할 필요 없음
			break;
	}

	//데이터의 아래쪽 영역 탐색 및 축소
	for (int y = rect->end.y;; y--)
	{
		isLetter = false;
		for (int x = rect->start.x; x <= rect->end.x; x++)
		{
			pixelVal = img.at<unsigned char>(y, x);
			if (pixelVal < COLOR_CHECK_RANGE)
			{
				//문자라인 발견시 bottom을 변경해준다.
				rect->end.y = y;
				isLetter = true;
				break;
			}
		}
		if (isLetter)	//글자라인이 아니라면 더이상 탐색할 필요가 없다.
			break;
	}
}

void OCR::ShowResultImage(int height, int width, int top, int startX)
{
	Mat output = Mat(height, width, CV_8UC1);

	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			output.at<unsigned char>(h, w) = img.at<unsigned char>(top + h, startX + w);
		}
	}
	
	imwrite(to_string(all.count) +".jpg", output);
	//waitKey(0);
}

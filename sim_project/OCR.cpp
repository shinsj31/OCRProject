#include "OCR.h"



OCR::OCR()
{
	//������ �ʱ�ȭ
	all.count = 0;
	for (int i = 0; i < MAX_DATA_COUNT; i++)
	{
		all.data[i].isFixed = false;
		all.data[i].nextIsSpace = false;
	}
}


OCR::~OCR()
{
}

void OCR::setImg(String path)
{
	img = imread(path, IMREAD_GRAYSCALE);
}

/*Step1. ��ü �̹����� ���Ͽ� ���� ����(���μ� ã��)*/
void OCR::ParsingStepFirst()
{
	all.count = 0;

	int nWidth = img.cols;
	int nHeight = img.rows;
	int top, bottom;

	unsigned char pixelVal;

	bool isLetter;
	bool prevLineState = false;

	//�̹����� ���� ��ü�� Ž������ �ʰ� �߰��� Ž���ؼ� �뷫���� ���̸� �˾Ƴ���.
	int xStart = 0;
	int xEnd = nWidth;

	for (int y = 0; y < nHeight; y++)
	{
		isLetter = false;
		for (int x = xStart; x < xEnd; x++)
		{
			pixelVal = img.at<unsigned char>(y, x);

			if (pixelVal < COLOR_CHECK_RANGE)
			{//���ڰ� �ִ� ���� �߰�
				isLetter = true;
				break;
			}
		}
		if (isLetter)
		{//���ڶ����� ��
			if (!prevLineState)
			{//prevLineState�� ���� ������ ����� �� false�̴�.
			 //���� ���������� ����̰�, ���ڶ����� ã�Ҵٸ�, �� ������ ���� ��ܿ� �ش��ϴ� ������ �ȴ�.
				top = y;
			}
		}
		else
		{
			if (prevLineState)
			{//���� ���� ���°� ���ڶ����̾���, ���� ����̸� �� ������ �� ������ ���� �ϴܿ� �ش��ϴ� ������ �ȴ�.
				bottom = y-1;

				//�� ���� top�� bottom�� ã�����Ƿ� ���ΰ�踦 ã�Ƴ��� �ܰ� ����
				ParsingStepSecond(top, bottom);
			}
		}
		prevLineState = isLetter;
	}
}

/*Step2. �ϳ��� ���� ������ �� ������ ���μ� �����ϱ�.
���μ� ���� �� ���������� �ٹٲ��� �߰����ش�.(������ �ٲ�� ���� ǥ���ϱ� ����)
�� ���� ������ ���ݵ� �� ���� ū ������ ���� �� �� ������ 0.7��ŭ�� ���� ���� �������� ����.*/
void OCR::ParsingStepSecond(int top, int bottom)
{
	int nWidth = img.cols;

	unsigned char pixelVal;
	bool isLetter;
	bool prevLineState = false;

	int gapSpace = 0;
	int prevX = -1;

	int startCount = all.count;

	for (int x = 0; x < nWidth; x++)
	{
		isLetter = false;
		for (int y = top; y <= bottom; y++)
		{
			pixelVal = img.at<unsigned char>(y, x);
			
			if (pixelVal <COLOR_CHECK_RANGE)
			{//���ڿ��� ã��
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
				//�� ������ ���ο��� ã�� �Ϸ�.
				all.data[all.count].rect.end.x = x - 1;
				all.data[all.count].rect.end.y = bottom;

				if (prevX > 0)
				{//������ ù ���ڸ� �����ϱ� ���ؼ�!
					if (gapSpace < (all.data[all.count].rect.start.x) - prevX)
						gapSpace = (all.data[all.count].rect.start.x) - prevX;
				}
				prevX = x - 1;

				//��輱 �����۾�
				ParsingStepThird(&all.data[all.count].rect);
				ShowResultImage(all.data[all.count].rect.end.y - all.data[all.count].rect.start.y, all.data[all.count].rect.end.x - all.data[all.count].rect.start.x, all.data[all.count].rect.start.y, all.data[all.count].rect.start.x);
				all.count+=1;
			}
		}
		prevLineState = isLetter;
	}
	//���� �߰��ϱ�
	AddSpaces(startCount, all.count - 1, gapSpace*0.7);
	//�ٹٲ� ���� �߰��ϱ�
	all.data[all.count].isFixed = true;
	all.data[all.count++].letter.value = '\n';
}

void OCR::ParsingStepThird(my_Rect * rect)
{
	bool isLetter;
	unsigned char pixelVal;

	//�������� ���ʿ��� Ž�� �ʿ��ϴٸ� ���
	for (int y = rect->start.y;; y++)
	{
		isLetter = false;
		for (int x = rect->start.x; x <= rect->end.x; x++)
		{
			pixelVal = img.at<unsigned char>(y, x);
			if (pixelVal < COLOR_CHECK_RANGE)
			{
				//���ڶ��� �߽߰� top�� �������ش�.
				rect->start.y = y;
				isLetter = true;
				break;
			}
		}
		if (isLetter)	//�̹� ���ڶ����̶�� Ž���� �ʿ� ����
			break;
	}

	//�������� �Ʒ��� ���� Ž�� �� ���
	for (int y = rect->end.y;; y--)
	{
		isLetter = false;
		for (int x = rect->start.x; x <= rect->end.x; x++)
		{
			pixelVal = img.at<unsigned char>(y, x);
			if (pixelVal < COLOR_CHECK_RANGE)
			{
				//���ڶ��� �߽߰� bottom�� �������ش�.
				rect->end.y = y;
				isLetter = true;
				break;
			}
		}
		if (isLetter)	//���ڶ����� �ƴ϶�� ���̻� Ž���� �ʿ䰡 ����.
			break;
	}
}

/*����: ������ ���� ī��Ʈ, �� ī��Ʈ, ���� ����
���� ���ڿ��� �Ÿ��� ���ذŸ����� ũ�� ����� �ν�, �ش� �������� nextIsSpace�� true�� �ٲ��ش�.*/
void OCR::AddSpaces(int startCount, int endCount, int stdGap)
{
	int gap;
	
	for (int i = startCount; i < endCount; i++)
	{
		gap = (all.data[i + 1].rect.start.x) - (all.data[i].rect.end.x);
		if (gap > stdGap)
			all.data[i].nextIsSpace = true;
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

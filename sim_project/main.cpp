#include <iostream>
#include "OCR.h"

int main(void)
{
	OCR* ocr = new OCR();
	ocr->setImg("test_img2.jpg");
	//Mat img=imread("test_img.jpg", IMREAD_GRAYSCALE);

	//imshow("display",img);
	ocr->ParsingStepFirst();
	//waitKey(0);
}
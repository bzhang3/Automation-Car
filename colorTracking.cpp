#include<iostream>
#include "cv.h"
#include "highgui.h"
#include <ctype.h>
using namespace std;
using namespace cv;

void colorFilter(CvMat *inputImage, CvMat *&outputImage)  
{  
    int i, j;  
    IplImage* image = cvCreateImage(cvGetSize(inputImage), 8, 3);  
    cvGetImage(inputImage, image);      
    IplImage* hsv = cvCreateImage( cvGetSize(image), 8, 3 );    
      
    cvCvtColor(image,hsv,CV_BGR2HSV);  
    int width = hsv->width;  
    int height = hsv->height;  
    for (i = 0; i < height; i++)  
        for (j = 0; j < width; j++)  
        {  
            CvScalar s_hsv = cvGet2D(hsv, i, j);//获取像素点为（j, i）点的HSV的值   
            /* 
                opencv 的H范围是0~180，红色的H范围大概是(0~8)∪(160,180)  
                S是饱和度，一般是大于一个值,S过低就是灰色（参考值S>80)， 
                V是亮度，过低就是黑色，过高就是白色(参考值220>V>50)。 
            */  
            CvScalar s;  
            if (!(((s_hsv.val[0]>0)&&(s_hsv.val[0]<8)) || (s_hsv.val[0]>120)&&(s_hsv.val[0]<180)))  
            {  
                s.val[0] =0;  
                s.val[1]=0;  
                s.val[2]=0;  
                cvSet2D(hsv, i ,j, s);  
            }  
              
              
        }  
    outputImage = cvCreateMat( hsv->height, hsv->width, CV_8UC3 );  
    cvConvert(hsv, outputImage);  
    cvNamedWindow("filter");  
    cvShowImage("filter", hsv);  
    waitKey(0);  
    cvReleaseImage(&hsv);  
}  


int main()
{
	IplImage *image,*blackImage, *hsv;//图像空间
	image=cvLoadImage("IMG_2753.jpg");//打开图像源图像

	hsv=cvCreateImage(cvGetSize(image),8,3);//给hsv色系的图像申请空间

	cvNamedWindow("image",CV_WINDOW_AUTOSIZE);//用于显示图像的窗口
	cvNamedWindow("hsv",CV_WINDOW_AUTOSIZE);
	
	cvCvtColor(image,hsv,CV_BGR2HSV);//将RGB色系转为HSV色系

	colorFilter(image, blackImage);
	
	cvShowImage("image",blackImage);
	cvShowImage("hsv",hsv);

	cvWaitKey(0);
	cvDestroyWindow("image");
	cvDestroyWindow("hsv");

	return 0;
}
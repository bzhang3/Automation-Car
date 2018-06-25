#define _GLIBCXX_USE_CXX11_ABI 0
#include<opencv2/imgproc/imgproc.hpp>  
#include<opencv2/highgui/highgui.hpp>  
#include<opencv2/core/core.hpp>
#include<opencv2/opencv.hpp>
#include<math.h>

using namespace cv;
using namespace std;

#define thredValue 	60
#define	white	255
#define black	0
#define PI 	3.141592654

double convertToAngle(Point orign, Point top, Point bottom){
	double angle = 0;
	double angle_tanValue  = 0;
	double angle_atanValue = 0;
	double angle_sinValue  = 0;

	angle_tanValue  = sqrt(pow(top.x - orign.x, 2) + pow(top.y - orign.y , 2))/
					  sqrt(pow(top.x - bottom.x, 2) + pow(top.y - bottom.y, 2));

	angle_atanValue = atan(angle_tanValue);
	angle_sinValue  = sin(angle_atanValue);
	angle = angle_atanValue * 180 / PI;

	return angle;
}

/****************************************************
函数名称：霍夫巡线
作者：	 张兵	
日期：	 2018/04/28
****************************************************/
Mat houghAlgorithm(Mat inputImage, Mat outputImage){
	vector<Vec4i> Lines;  
	HoughLinesP(inputImage, Lines, 1, CV_PI / 360, 170,40,50);
	//画线	
	for (size_t i = 0; i < Lines.size(); i++)  
	{  
		line(outputImage, Point(Lines[i][0], Lines[i][1]), Point(Lines[i][2], Lines[i][3]), Scalar(0, 255, 0), 2, 8);  
	}

	imwrite("lineCheck.jpg", outputImage);	
	return outputImage;
}

/****************************************************
函数名称：寻找轨道算法
作者：	 张兵	
日期：	 2018/04/28
****************************************************/
Mat middArea(Mat src){
	int partNode[6] = {0, 80, 160, 240, 320, 400};
	unsigned int theta[5]	= {0};
	int midCol[5]	= {0};	
	int xAxis[5]	= {0};
	int rowNum 		= 250;	
	int startPoint  = 0;
	int endPoint 	= 0;
	int arraySize	= 0;
	int rows 		= src.rows;
	int cols 		= src.cols;
	int rowCount	= rowNum;
	double angle 	= 0;
	Point start;
	Point end;
	Point origin;
	Point top;
	Point bottom;

	origin.x		= rows / 2;
	origin.y 		= cols;
	start.x 		= rows;
	start.y 		= cols;
	end.x 			= 0;
	end.y 			= 0;	

	arraySize = sizeof(partNode)/sizeof(partNode[0]);
	int yAxis = rowNum - rowCount;			
	for(int j = 0; j < arraySize - 1; j++){
		int thetaNum = 0;
		for(int i = partNode[j]; i < partNode[j+1] - 1; i++){
			if(0 == rowCount){
				rowCount = rowNum;
			}
			yAxis = rowNum - rowCount;
			if((black == src.at<uchar>(yAxis, i)) && ( black == src.at<uchar>(yAxis - 1, i+1))){
				if( 0 == ((rows / 2) - i)){
					theta[j] += 1;
				} else if ( 0 == ((cols - rowNum))){
					theta[j] += 0;
				} else {
					theta[j] += ( rowNum % (i - rows / 2) );
				}
			
				thetaNum++;
				midCol[j] += i;
				xAxis[j]  += yAxis;
			} else {
				theta[j] += 0;
			}
			rowCount--;
		}
		// printf("theta NUM: %d ", thetaNum);
		if(0 == thetaNum){
			theta[j] = 0;
			midCol[j] = 1;
			xAxis[j] = 0;
		} else {
			theta[j] = theta[j] / thetaNum;
			midCol[j] = midCol[j] / thetaNum;
			xAxis[j] = xAxis[j] / thetaNum;
		}
	
	
		//printf("midCol: %d ", midCol[j]);
		if(theta[j] != 0){
//			if( theta[j] < -1 | theta[j] > 1){
				start.x = cols/2;
				start.y = rows;
				end.x = midCol[j];
				end.y = xAxis[j];

				top.x = midCol[j];
				top.y = xAxis[j];
				bottom.x = rows;
				bottom.y = xAxis[j];
				angle = convertToAngle(origin, top, bottom);
	
				printf("angle: %f \n", angle);
				line(src, start, end, CV_RGB(black, black, black) ,3);
//			}			
		}	 
	}  
	return src;	
}

// void firstFilterlineCheck(){

// 	return first; 
// }

// void secondFilterlineCheck(){

// 	return first;
// }

void directionControl(){

	
}

/****************************************************
函数名称：主函数
作者：	 张兵	
日期：	 2018/04/28
****************************************************/

int main()  {  
	Mat Image;
	Mat DstImg;  
	Mat CannyImg;
	Mat dst;
	Mat res, gray;
	Mat thedImg;

	VideoCapture capture("135224.avi");
	if(!capture.isOpened()){
		cout<<"No video"<< endl;
		return 0;
	}

	while(capture.isOpened()){

		capture >> Image;
		resize(Image, Image, Size(400,400), 1, 1); 
		res = Image.clone();
		thedImg = Image > thredValue;
		cvtColor(thedImg, gray, CV_BGR2GRAY);
		GaussianBlur(gray, DstImg, Size(5,5), 1);
		Canny(gray, CannyImg, 80, 210, 3); 
		DstImg = houghAlgorithm(CannyImg, DstImg);
		res = middArea(DstImg);

		imshow("Video", res);	//	imwrite("Canny.jpg", CannyImg);

		if(waitKey() == 'q'){
			break;
		}
		waitKey(10);
	}
	capture.release();

    return 0;  
}  

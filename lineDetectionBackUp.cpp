#define _GLIBCXX_USE_CXX11_ABI 0
#include<opencv2/imgproc/imgproc.hpp>  
#include<opencv2/highgui/highgui.hpp>  
#include<opencv2/core/core.hpp>
#include<opencv2/opencv.hpp>

using namespace cv;
using namespace std;

#define thredValue 	60
#define	white	255
#define black	0

/****************************************************
函数名称：分水岭算法封装
作者：	 张兵	
日期：	 2018/04/28
****************************************************/
class WatershedSegmenter {  
  private:  
      cv::Mat markers;  
  
  public:  
      void setMarkers(const cv::Mat& markerImage) {  
        // Convert to image of ints  
        markerImage.convertTo(markers,CV_32S);  
      }  
      cv::Mat process(const cv::Mat &image) {  
        // Apply watershed  
        cv::watershed(image,markers);  
        return markers;  
      }  
      // Return result in the form of an image  
      cv::Mat getSegmentation() {  
        cv::Mat tmp;  
        // all segment with label higher than 255  
        // will be assigned value 255  
        markers.convertTo(tmp,CV_8U);  
        return tmp;  
      }  
      // Return watershed in the form of an image以图像的形式返回分水岭  
      cv::Mat getWatersheds() {  
        cv::Mat tmp;  
        //在变换前，把每个像素p转换为255p+255（在conertTo中实现）  
        markers.convertTo(tmp,CV_8U,255,255);  
        return tmp;  
      }  
};  
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
函数名称：画线算法
作者：	 张兵	
日期：	 2018/04/28
****************************************************/
void contours(Mat src){

	int countRect 	= 0;	
	int maxArea 	= 0;
	Mat 	resAllRect;
    Mat 	resMaxRect;
    Rect 	maxRect;
	vector<Point> 			maxContour;	
	vector<vector<Point> > 	contours;

	findContours(src, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	for(size_t i = 0; i < contours.size(); i++){
        int area = (int)cv::contourArea(contours[i]);
        if (area > maxArea){
            maxArea = area;
            maxContour = contours[i];		
       }
		countRect++;
    }
    // 将轮廓转为矩形框
    maxRect = cv::boundingRect(maxContour);
 
    src.copyTo(resAllRect);
    src.copyTo(resMaxRect);
    
    for (size_t i = 0; i < contours.size(); i++)
    {
        cv::Rect r = cv::boundingRect(contours[i]);
        cv::rectangle(resAllRect, r, cv::Scalar(255));
    }

    cv::rectangle(resAllRect, maxRect, cv::Scalar(255));

	imwrite("contours.jpg", resAllRect);
	return;
}
/****************************************************
函数名称：分水岭算法
作者：	 张兵	
日期：	 2018/04/28
****************************************************/
Mat waterFilter(Mat src, Mat dst, Mat origImg){

	Mat maskColor = src.clone();
	Mat maskImage;
	Mat markers1;
	Mat fg1;
	Mat bg1;
    Mat element5(1,1,CV_8U,Scalar(1));//5*5正方形，8位uchar型，全1结构元素 
    int numChannel;

	threshold(src, dst, 85, 255, CV_THRESH_BINARY);	 
    morphologyEx(dst, fg1,cv::MORPH_CLOSE,element5,Point(-1,-1),1);// 闭运算填充物体内细小空洞、连接邻近物体  
    dilate(dst,bg1,cv::Mat(),cv::Point(-1,-1),4);//膨胀4次，锚点为结构元素中心点  
   	threshold(bg1,bg1,1,128,cv::THRESH_BINARY_INV);//>=1的像素设置为128（即背景）

    markers1 = fg1 + bg1; //使用Mat类的重载运算符+来合并图像。  
	WatershedSegmenter segmenter1;  //实例化一个分水岭分割方法的对象  
	segmenter1.setMarkers(markers1);//设置算法的标记图像，使得水淹过程从这组预先定义好的标记像素开始  
    segmenter1.process(origImg);     //传入待分割原图  
	maskImage = segmenter1.getSegmentation();  
	//dst = maskImage;
	
    threshold(maskImage,maskImage,250,1,THRESH_BINARY); 
    cvtColor(maskImage,maskImage,COLOR_GRAY2BGR);  
    maskImage = origImg.mul(maskImage);  

    numChannel= maskImage.cols * maskImage.channels(); // total number of elements per line  

    for (int j=0; j<maskImage.rows; j++){  
        uchar* data= maskImage.ptr<uchar>(j);  
        for (int i=0; i<numChannel; i++){  
            if (*data==0){ //将背景由黑色改为白色显示  
                *data = white;
            }  
            data++;//指针操作：如为uchar型指针则移动1个字节，即移动到下1列  
        }  
     } 
	//	maskImage = maskImage & maskImage;
	//	imwrite("waterFilter.jpg",maskImage);
	//	imwrite("waterFilterTest.jpg",maskColor);

	return maskImage;
}
/****************************************************
函数名称：寻找轨道算法
作者：	 张兵	
日期：	 2018/04/28
****************************************************/
Mat middArea(Mat src){
	int partNode[6] = {0, 80, 160, 240, 320, 400};
	int theta[5]	= {0};
	int midCol[5]	= {0};	
	int rowNum 		= 200;	
	int startPoint  = 0;
	int endPoint 	= 0;
	int arraySize	= 0;
	int rows 		= src.rows;
	int cols 		= src.cols;
	int rowCount	= rowNum;
	Point start;
	Point end;

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
			} else {
				theta[j] += 0;
			}
			rowCount--;
		}
		// printf("theta NUM: %d ", thetaNum);
		if(0 == thetaNum){
			theta[j] = 0;
			midCol[j] = 1;
		} else {
			theta[j] = theta[j] / thetaNum;
			midCol[j] = midCol[j] / thetaNum;
		}	
		//printf("theta: %d \n", theta[j]);
		//printf("midCol: %d ", midCol[j]);
		if(theta[j] != 0){
			if( theta[j] < -4 | theta[j] > 4){
				start.x = cols/2;
				start.y = rows;
				end.x = midCol[j];
				end.y = rowNum;
				line(src, start, end, CV_RGB(black, black, black) ,3);
			}			
		}	 
	}  
	return src;	
}

void firstFilterlineCheck(){

	return first; 
}

void secondFilterlineCheck(){

	return first;
}

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

	//Image = imread("IMG_2760.jpg");
		resize(Image, Image, Size(400,400), 1, 1); 
		res = Image.clone();
		thedImg = Image > thredValue;
		cvtColor(thedImg, gray, CV_BGR2GRAY);
		cv::GaussianBlur(gray, DstImg, Size(5,5), 1);
		//thed = DstImg > 60;
		//res = waterFilter(gray, res, Image);		
		Canny(gray, CannyImg, 80, 210, 3); 
		DstImg = houghAlgorithm(CannyImg, DstImg);

	//	imwrite("ThresHold.jpg", res);	
	//	dst = middArea(bg1);
		res = middArea(DstImg);

		imshow("Video", res);	//	imwrite("Canny.jpg", CannyImg);

		if(waitKey(10) == 'q'){
			break;
		}
		waitKey(10);
	}
	capture.release();

    return 0;  
}  

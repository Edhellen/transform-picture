#include <string>
#include <iostream>
#include "opencv/cv.h"
#include "opencv2/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include "opencv2/core.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>

using namespace std;
using namespace cv;
#define NO_IMAGES 1000

Mat rotateImage(const Mat& source, double angle)
{
    Point2f src_center(source.cols/2.0F, source.rows/2.0F);
    Mat rot_mat = getRotationMatrix2D(src_center, angle, 1.0);

    Rect bbox = cv::RotatedRect(src_center,source.size(), angle).boundingRect();
    rot_mat.at<double>(0,2) += bbox.width/2.0 - src_center.x;
    rot_mat.at<double>(1,2) += bbox.height/2.0 - src_center.y;


    Mat dst;
    warpAffine(source, dst, rot_mat, bbox.size());
    return dst;
}

Mat scaleImage(const Mat& source, double scaleX, double scaleY)
{
    Mat dst;
    resize(source, dst, Size(0,0)/*new dimensions*/,scaleX, scaleY, INTER_CUBIC/* interpolation method*/);
    return dst;
}

Mat affineTransform(Mat src, int xSrc[], int ySrc[], float xDest[], float yDest[])
{
    string str1 = "Source image";
    string str2 = "Warp";


    Point2f srcTri[3];
    Point2f dstTri[3];


       Mat warp_mat( 2, 3, CV_32FC1 );
       Mat  warp_dst;

       warp_dst = Mat::zeros( src.rows, src.cols, src.type() );

       srcTri[0] = Point2f( xSrc[0],ySrc[0] );
       srcTri[1] = Point2f( xSrc[1], ySrc[1] );
       srcTri[2] = Point2f( xSrc[2], ySrc[2] );

       dstTri[0] = Point2f( src.cols*xDest[0], src.rows*yDest[0] );
       dstTri[1] = Point2f( src.cols*xDest[1], src.rows*yDest[1] );
       dstTri[2] = Point2f( src.cols*xDest[2], src.rows*yDest[2] );

       /// Get the Affine Transform
       warp_mat = getAffineTransform( srcTri, dstTri );

       /// Apply the Affine Transform just found to the src image
       warpAffine( src, warp_dst, warp_mat, warp_dst.size() );

       return warp_dst;
       /** Rotating the image after Warp */
}

Mat perspectiveTransform(Mat src, float xDest[], float yDest[])
{
       Point2f srcTri[4];
       Point2f dstTri[4];

       Mat warp_mat( 3, 3, CV_32FC1 );
       Mat  warp_dst;

       /// Load the image

       /// Set the dst image the same type and size as src
       warp_dst = Mat::zeros( src.rows, src.cols, src.type() );

       /// Set your 3 points to calculate the  Affine Transform
       srcTri[0] = Point2f( 0,0 );
       srcTri[1] = Point2f( src.cols - 1, 0 );
       srcTri[2] = Point2f( 0, src.rows - 1 );
       srcTri[3] = Point2f( src.cols -1 , src.rows - 1 );


       dstTri[0] = Point2f( src.cols*xDest[0], src.rows*yDest[0] );
       dstTri[1] = Point2f( src.cols*xDest[1], src.rows*yDest[1] );
       dstTri[2] = Point2f( src.cols*xDest[2], src.rows*yDest[2] );
       dstTri[3] = Point2f( src.cols*xDest[3], src.rows*yDest[3] );

       /// Get the Affine Transform
       warp_mat = getPerspectiveTransform( srcTri, dstTri );

       /// Apply the Affine Transform just found to the src image
       warpPerspective( src, warp_dst, warp_mat, warp_dst.size() );
       return warp_dst;
       /// Show what you got

}

void generateRotatedImages()
{
    cout << "==Rotating Started=="<<endl;
    for(int i =1; i <= NO_IMAGES; i++)
    {
		string str = "D:\\data\\original\\" + to_string(i) + ".jpg";
        Mat m = imread(str);

        int counter = 1;

        for(int j = 10; j <=180; j = j+10)
        {
            cout << "generated " << j << endl;
            Mat r = rotateImage(m,j);
			string str1 = "D:\\data\\modified\\rotated\\" + to_string(i) + "_" + to_string(counter) + ".jpg";
            imwrite(str1,r);
            counter++;
        }
        cout << endl <<"=============================" << endl << "File " << i << " proceed" << endl;

    }
    cout << "==Rotating completed=="<<endl;
}

void generatePerspectiveImages()
{
    float xDest[4], yDest[4];
    cout << "==Perspective Started=="<<endl;

    for(int i = 1; i <= NO_IMAGES; i++)
    {
		string str = "D:\\data\\original\\" + to_string(i) + ".jpg";
        Mat m = imread(str);

        int counter = 0;
        xDest[0] = 0; xDest[1] = 0.0; xDest[2] = 0; xDest[3] = 0.0;
        yDest[0] = 0.0; yDest[1] = 0.0; yDest[2] = 0.0; yDest[3] = 0.0;

        for (float j = 0.15; j <=1; j+=0.05)
        {
            cout << "generated " << j  <<endl;
            xDest[0] = 0.0; xDest[1] = 1; xDest[2] = 0.0; xDest[3] = 1;
            yDest[0] = 0.0; yDest[1] = 0; yDest[2] = yDest[2]+0.05; yDest[3]=yDest[3]+0.05;
            Mat r1 = perspectiveTransform(m,xDest,yDest);
			string str1 = "D:\\data\\modified\\perspectived\\" + to_string(i) + "_" + to_string(counter) + ".jpg";
            imwrite(str1, r1);
            counter++;

        }
        cout << endl <<"=============================" << endl << "File " << i << " proceed" << endl;

    }
    cout << "==Perspective Completed=="<<endl;
}

void generateScaledImages()
{
    cout << "==Scaling Started=="<<endl;
    for(int i = 1; i <= NO_IMAGES; i++)
    {
        string str = "D:\\data\\original\\" + to_string(i) + ".jpg";
        Mat m = imread(str);

        int counter = 1;
        for (double j = 0.1; j <= 1; j+=0.05)
        {
            cout << "generated " << j << endl;
			string str1 = "D:\\data\\modified\\scaled\\" + to_string(i) + "_" + to_string(counter) + ".jpg";
            imwrite(str1, scaleImage(m,j,j));

            counter++;
        }
        cout << endl <<"=============================" << endl << "File " << i << " proceed" << endl;

    }
    cout << "==Scaling Completed=="<<endl;
}

void AddGaussianNoise(double Mean=0.0)
{
    cout << "==Noising started==" << endl;

    for(int i =1; i <= NO_IMAGES; i++)
    {
		string str = "D:\\data\\original\\" + to_string(i) + ".jpg";
        Mat mSrc = imread(str);

        int counter = 1;
        //Увеличиваем standart deviation
        for (float j=0; j<=300; j+=30)
        {
            Mat mSrc_16SC;
            Mat mGaussian_noise = Mat(mSrc.size(),CV_16SC3);
            randn(mGaussian_noise,Scalar::all(Mean), Scalar::all(j));

            Mat mDst;
            mSrc.convertTo(mSrc_16SC,CV_16SC3);
            addWeighted(mSrc_16SC, 1.0, mGaussian_noise, 1.0, 0.0, mSrc_16SC);
            mSrc_16SC.convertTo(mDst,mSrc.type());
            string str1 ="D:\\data\\modified\\noised\\" + to_string(i) + "_" + to_string(counter) + ".jpg";
            imwrite(str1, mDst);
            cout << "generated " << j << endl;

            counter++;
        }
        cout << endl <<"=============================" << endl << "File " << i << " proceed" << endl;
    }
    cout << "==Noising completed==";
}

void generateBluredImages()
{
	cout << "==Bluring started==" << endl;

	for (int i = 1; i <= 1000; i++)
	{
		string str = "D:\\data\\original\\" + to_string(i) + ".jpg";
		Mat mSrc = imread(str);

		int counter = 1;

		for (int j = 1; j < 20; j = j + 2)
		{
			Mat mDst;
			medianBlur(mSrc, mDst, j);
			string str1 = "D:\\data\\modified\\blured\\" + to_string(i) + "_" + to_string(counter) + ".jpg";
			imwrite(str1, mDst);
			cout << "generated " << j << endl;
			counter++;
		}

		cout << endl << "=============================" << endl << "File " << i << " proceed" << endl;
	}
	cout << "==Bluring completed==";
}

void showImage(){
    Mat img = imread("D:\\data\\original\\1.jpg",CV_LOAD_IMAGE_COLOR);
    if(img.empty()){
        std::cout << "Incorrect path or wrong filename, image not read!" << std::endl;
        return;
    }
    imshow("opencvtest",addRecoImage(img));
    waitKey(0);

    return;
}

int main(int argc, char *argv[])
{
    showImage();
	generateBluredImages();
    AddGaussianNoise();
    generateRotatedImages();
    generatePerspectiveImages();
    generateScaledImages();

    cvWaitKey(0);
    return 0;
}

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using std::cout;
int edgeThresh = 1;
int lowThreshold = 50;
int const max_lowThreshold = 100;
int ratio = 3;
int stateL = 0, prev1 = 0;
int kernel_size = 3;
int main()
{
  cv::VideoCapture cap(0);
  const char* windowName = "Fingerprint detection";
  while (1)
  {
      Mat3b frame;
      Mat3b dst;
      Mat detected_edges,src;
      cap >> frame;

      /* THRESHOLD ON HSV*/
      cvtColor(frame, dst, CV_BGR2HSV);
      GaussianBlur(dst, dst, Size(7,7), 1, 1);
      for(int r=0; r<dst.rows; ++r){
          for(int c=0; c<dst.cols; ++c)
              // 0<H<0.25  -   0.15<S<0.9    -    0.2<V<0.95
              if( (dst(r,c)[0]>5) && (dst(r,c)[0] < 17) && (dst(r,c)[1]>38) && (dst(r,c)[1]<250) && (dst(r,c)[2]>51) && (dst(r,c)[2]<242) ); //skin colour
              else for(int i=0; i<3; ++i)	dst(r,c)[i] = 0;
      }

      /* BGR CONVERSION AND THRESHOLD */
      Mat1b frame_gray;
      cvtColor(dst, dst, CV_HSV2BGR);
      cvtColor(dst, frame_gray, CV_BGR2GRAY);

      threshold(frame_gray, frame_gray, 60, 255, CV_THRESH_BINARY);

      dilate(frame_gray, frame_gray, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)));
      erode(frame_gray, frame_gray, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)));
      erode(frame_gray, frame_gray, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)));
      dilate(frame_gray, detected_edges, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)));
        GaussianBlur(detected_edges, detected_edges, Size(7,7), 1, 1);
       cv::imshow("Greyscale", frame_gray);
       if (cv::waitKey(30) >= 0) break;


       /* Canny detector */
               Canny(detected_edges, detected_edges, lowThreshold,lowThreshold *ratio, kernel_size);
               dilate(detected_edges, detected_edges, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
               imshow("Canny Edges", detected_edges);


               src.create(detected_edges.size(), detected_edges.type());
               src = Scalar::all(0); //empty image



               frame.copyTo(src, detected_edges); //edges copy to dst matrix
               dilate(src, src, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)));





      // Contour detection
      std::vector<std::vector<cv::Point> > contours;
      std::vector<cv::Vec4i> hierarchy;
      cv::findContours(detected_edges, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
      cv::Scalar color = cv::Scalar(0, 100, 0);


      // Find the rotated rectangles for each contour
       std::vector<RotatedRect> minRect( contours.size() );
       for( size_t i = 0; i< contours.size(); i++ )
       {
          minRect[i] = minAreaRect( Mat(contours[i]) );
          if(contours[i].size()> 120) { //To keep largest contour
                        // rotated rectangle
                       Point2f rect_points[4];
                       minRect[i].points( rect_points );

                       if(minRect[i].size.height>minRect[i].size.width){
                           minRect[i].size.height =(float)(1.5)*minRect[i].size.width;
                           minRect[i].center = (rect_points[1]+rect_points[2])/2 + (rect_points[0]-rect_points[1])/6;
                       } else {
                           minRect[i].size.width =(float)(1.5)*minRect[i].size.height;
                           minRect[i].center = (rect_points[2]+rect_points[3])/2 + (rect_points[0]-rect_points[3])/6;
                       }
                  //draw the contours to the original
                       minRect[i].points( rect_points );
                       for( int j = 0; j < 4; j++ )
                           line( frame, rect_points[j], rect_points[(j+1)%4], color, 2, 8 );


          }


       }




     /*  Mat drawing = Mat::zeros( frame_gray.size(), CV_8UC3 );
       for( int i = 0; i< contours.size(); i++ )
       {
         cv::Scalar color = cv::Scalar(0, 100, 0);
         // draw contour to the original
         cv::drawContours( frame, contours, i, color, 1, 8, std::vector<Vec4i>(), 0, Point() );
         // rotated rectangle
         Point2f rect_points[4]; minRect[i].points( rect_points );
         for( int j = 0; j < 4; j++ )
          line( frame, rect_points[j], rect_points[(j+1)%4], color, 2, 8 );
        }*/



     /*Display the fingerprint*/
      cv::imshow(windowName, frame);
      if (cv::waitKey(30) >= 0) break;
  }
  return 0;
}

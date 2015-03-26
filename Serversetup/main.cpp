#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

/** Function Headers */
void detectAndSave(Mat frame, string filename);

/** Global variables */
String face_cascade_name = "haarcascade_frontalface_alt.xml";
String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);

/** function main */
int main(int argc, const char** argv){
	Mat frame;

	//Load the cascades
	if (!face_cascade.load(face_cascade_name)){ printf("--(!)Error loading\n"); return -1; };
	if (!eyes_cascade.load(eyes_cascade_name)){ printf("--(!)Error loading\n"); return -1; };

	// Read in image
	frame = imread(argv[1], CV_LOAD_IMAGE_COLOR);

	//Detect Face in image and save result
	detectAndSave(frame, argv[2]);
	waitKey();

}

/** function detectAndSave */
void detectAndSave(Mat frame, string filename){
	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//-- Detect faces
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));
	int locx, locy, area, eye1x, eye1y, eye2x, eye2y;
	locx = 0;
	locy = 0;
	eye1x = 0;
	eye1y = 0;
	eye2x = 0;
	eye2y = 0;
	area = 0;
	if (faces.size() == 1){
		Point center(faces[0].x + faces[0].width*0.5, faces[0].y + faces[0].height*0.5);
		ellipse(frame, center, Size(faces[0].width*0.5, faces[0].height*0.5), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);

		area = faces[0].width * faces[0].height;
		locx = center.x;
		locy = center.y;

		//Detect eyes and bound locations
		Mat faceROI = frame_gray(faces[0]);
		std::vector<Rect> eyes;

		//Eye detection
		eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

		if (eyes.size() == 2){	
			center = Point(faces[0].x + eyes[0].x + eyes[0].width*0.5, faces[0].y + eyes[0].y + eyes[0].height*0.5);
			int radius = cvRound((eyes[0].width + eyes[0].height)*0.25);
			circle(frame, center, radius, Scalar(255, 0, 0), 4, 8, 0);
			eye1x = center.x;
			eye1y = center.y;

			center = Point(faces[0].x + eyes[1].x + eyes[1].width*0.5, faces[0].y + eyes[1].y + eyes[1].height*0.5);
			radius = cvRound((eyes[1].width + eyes[1].height)*0.25);
			circle(frame, center, radius, Scalar(255, 0, 0), 4, 8, 0);
			eye2x = center.x;
			eye2y = center.y;
		}	
	}
	cout << locx << "," << locy << "," << area << "," << eye1x << "," << eye1y << "," << eye2x << "," << eye2y << endl;
	//Show what you got
	
	//imshow(window_name, frame);
	imwrite(filename, frame);
}

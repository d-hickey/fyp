#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

/** Function Headers */
void detectAndSave(Mat frame);
void handDetect(Mat frame, Mat output);
void histAndBackproj(Mat frame, Mat output);
void morphOps(Mat &thresh);
bool checkOverlap(Rect a, Rect b);

/** Global variables */
String face_cascade_name = "haarcascade_frontalface_alt.xml";
String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);

Rect facearea;
int foundface;

int locx, locy, area, eye1x, eye1y, eye2x, eye2y;
string hand;

int bins = 15;

/** function main */
int main(int argc, const char** argv){
	Mat frame, framecopy;
	hand = "none";

	//Load the cascades
	if (!face_cascade.load(face_cascade_name)){ printf("--(!)Error loading\n"); return -1; };
	if (!eyes_cascade.load(eyes_cascade_name)){ printf("--(!)Error loading\n"); return -1; };

	// Read in image
	frame = imread(argv[1], CV_LOAD_IMAGE_COLOR);
	framecopy = frame.clone();

	//Detect Face in image and save result
	detectAndSave(frame);

	//hand detection, not used as default due to likelihood of false positives.
	if (foundface){
		//only one of these should be used
		//handDetect(framecopy, frame);
		//histAndBackproj(framecopy, frame);
	}

	imwrite(argv[2], frame);

	//output locations
	cout << locx << "," << locy << "," << area << "," << eye1x << "," << eye1y << "," << eye2x << "," << eye2y << "," << hand << endl;
	//waitKey();

}

/** function detectAndSave */
void detectAndSave(Mat frame){
	vector<Rect> faces;
	Mat frame_gray;

	foundface = 0;
	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//-- Detect faces
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

	Rect face;
	locx = 0;
	locy = 0;
	eye1x = 0;
	eye1y = 0;
	eye2x = 0;
	eye2y = 0;
	area = 0;

	if (faces.size() > 0){
		foundface = 1;
		face = faces[0];
		for (int i = 1; i < faces.size(); i++){
			if ((faces[i].width * faces[i].height) > (face.width * face.height)){
				face = faces[i];
			}
		}
		Point center(face.x + face.width*0.5, face.y + face.height*0.5);
		ellipse(frame, center, Size(face.width*0.5, face.height*0.5), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);

		area = face.width * face.height;
		locx = center.x;
		locy = center.y;

		//Detect eyes and bound locations
		facearea = face;
		Mat faceROI = frame_gray(face);
		vector<Rect> eyes;

		//Eye detection
		eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));
		if (eyes.size() > 2){
			for (int i = 0; i < eyes.size(); i++){
				for (int j = eyes.size() - 1; j > i; j--){
					if (checkOverlap(eyes[i], eyes[j])){
						eyes.erase(eyes.begin() + j);
					}
				}
			}
		}
		if (eyes.size() == 2){
			center = Point(face.x + eyes[0].x + eyes[0].width*0.5, face.y + eyes[0].y + eyes[0].height*0.5);
			int radius = cvRound((eyes[0].width + eyes[0].height)*0.25);
			circle(frame, center, radius, Scalar(255, 0, 0), 4, 8, 0);
			eye1x = center.x;
			eye1y = center.y;

			center = Point(face.x + eyes[1].x + eyes[1].width*0.5, face.y + eyes[1].y + eyes[1].height*0.5);
			radius = cvRound((eyes[1].width + eyes[1].height)*0.25);
			circle(frame, center, radius, Scalar(255, 0, 0), 4, 8, 0);
			eye2x = center.x;
			eye2y = center.y;
		}
	}
}

void handDetect(Mat frame, Mat output){
	Mat hsv_image, range_image;
	vector<Mat> splitimage;

	cvtColor(frame, hsv_image, CV_BGR2HSV);

	split(hsv_image, splitimage);
	inRange(hsv_image, Scalar(0, 0, 124), Scalar(188, 65, 200), range_image);

	morphOps(range_image);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	/// Find contours
	findContours(range_image, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	for (int i = contours.size() - 1; i >= 0; i--){
		if (contourArea(contours[i]) < 2000){
			contours.erase(contours.begin() + i);
		}
	}

	/// Approximate contours to polygons + get bounding rects and circles
	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());

	for (int i = 0; i < contours.size(); i++){
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));
	}

	//remove rectangles overlapping with face area
	for (int i = contours.size() - 1; i >= 0; i--){
		if (checkOverlap(boundRect[i], facearea)){
			contours.erase(contours.begin() + i);
			boundRect.erase(boundRect.begin() + i);
		}
	}

	/// Draw bounding rectangle
	for (int i = 0; i< contours.size(); i++){
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		rectangle(output, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
	}

	if (boundRect.size() == 1){
		if (boundRect[0].x < facearea.x){
			hand = "right";
		}
		else{
			hand = "left";
		}
	}

}

void histAndBackproj(Mat frame, Mat output){
	MatND hist;
	Mat hue, src, thresh, morphed, faceroi;
	vector<Mat> splitimage;

	int histSize = bins;
	float hue_range[] = { 0, 180 };
	const float* ranges = { hue_range };

	/// Get the Histogram and normalize it
	faceroi = frame(facearea);
	src = faceroi(Rect(35, 90, 100, 30));

	cvtColor(src, hue, CV_BGR2HSV);
	split(hue, splitimage);
	hue = splitimage[0];
	calcHist(&hue, 1, 0, Mat(), hist, 1, &histSize, &ranges, true, false);
	normalize(hist, hist, 0, 255, NORM_MINMAX, -1, Mat());

	// Get Backprojection
	cvtColor(frame, hue, CV_BGR2HSV);
	split(hue, splitimage);
	hue = splitimage[0];
	MatND backproj;
	calcBackProject(&hue, 1, 0, hist, backproj, &ranges, 1, true);

	// get binary image from backproj
	threshold(backproj, thresh, 130, 255, 0);

	morphOps(thresh);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	// Find contours
	findContours(thresh, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	for (int i = contours.size() - 1; i >= 0; i--){
		if (contourArea(contours[i]) < 2000){
			contours.erase(contours.begin() + i);
		}
	}

	/// Approximate contours to polygons + get bounding rects and circles
	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());

	for (int i = 0; i < contours.size(); i++){
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));
	}

	//remove rectangles overlapping with face area
	for (int i = contours.size() - 1; i >= 0; i--){
		if (checkOverlap(boundRect[i], facearea)){
			contours.erase(contours.begin() + i);
			boundRect.erase(boundRect.begin() + i);
		}
	}

	/// Draw bounding rectangle
	for (int i = 0; i< contours.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		rectangle(output, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
	}


	if (boundRect.size() == 1){
		if (boundRect[0].x < facearea.x){
			hand = "right";
		}
		else{
			hand = "left";
		}
	}
}

//performs morphology operations on an image
void morphOps(Mat &thresh){

	//create structuring elements that will be used to "dilate" and "erode" image.
	//the element chosen here for erode is a 3px by 3px rectangle
	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(3, 3));
	//dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(8, 8));

	erode(thresh, thresh, erodeElement);
	dilate(thresh, thresh, dilateElement);
}

//checks if two given rectangles overlap
bool checkOverlap(Rect a, Rect b){
	if (
		a.x < (b.x + b.width) &&
		(a.x + a.width) > b.x &&
		a.y < (b.y + b.height) &&
		(a.y + a.height) > b.y
		)
	{
		return true;
	}
	return false;
}

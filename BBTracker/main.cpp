#include <iostream>
#include <fstream>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <string>
#include <ctime>

using namespace std;
using namespace cv;
void detect_people(Mat &frame);

int main() {

	float start, end;


	VideoCapture cap("1.mp4");
	if (!cap.isOpened()) {
		cout << "Open video file failed!" << endl;
		system("pause");
		return -1;
	}
	Mat frame;
	Mat frame1;
	while (true) {
		cap >> frame;
		if (frame.empty()) {
			break;
		}
		int height;
		// 将图像进行缩放
		height = (frame.size().height * 800) / frame.size().width;
		resize(frame, frame, Size(800, height));
		start = clock();
		detect_people(frame);
		end = clock();

		cout << "Human detect time is " << (end - start) / CLOCKS_PER_SEC << "s" << endl;

		imshow("People detect", frame);
		waitKey(1);
	}
	return 0;
}

void detect_people(Mat &frame) {
	HOGDescriptor hog;
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
	vector<Rect> detected, detected_filtered;
	hog.detectMultiScale(frame, detected, 0, Size(8, 8), Size(16, 16), 1.06, 2);
	size_t i, j;
	/*checking for the distinctly detected human in a frame*/
	for (i = 0; i < detected.size(); i++) {
		Rect r = detected[i];
		for (j = 0; j < detected.size(); j++)
			if (j != i && (r & detected[j]) == r)
				break;
		if (j == detected.size())
			detected_filtered.push_back(r);
	}
	// draw the rectangle
	for (i = 0; i < detected_filtered.size(); i++) {
		Rect r = detected_filtered[i];
		r.x += cvRound(r.width*0.1);
		r.width = cvRound(r.width*0.8);
		r.y += cvRound(r.height*0.07);
		r.height = cvRound(r.height*0.8);
		rectangle(frame, r.tl(), r.br(), Scalar(0, 0, 255), 2);
	}
}
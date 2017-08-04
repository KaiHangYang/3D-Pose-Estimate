#include <iostream>
#include <fstream>
#include <stdio.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/ml.hpp>

#define POS_NUM 2400
#define NEG_NUM 1200
#define HARD_NEG_NUM 900
#define AUG_POS_NUM 900

#define TRAIN false
#define CENTRAL_CROP false

using namespace std;
using namespace cv;
using namespace cv::ml;

int main() {



	 //创建HOG描述子
	HOGDescriptor hog(cv::Size(64, 128), cv::Size(16, 16), cv::Size(8, 8), cv::Size(8, 8), 9);
	Ptr<SVM> svm = SVM::create();

	if (TRAIN) {
		string imgName;
		ifstream finPos("E:/INRIAPerson/train_64x128_H96/pos.lst");
		ifstream finNeg("E:/INRIAPerson/train_64x128_H96/neg.lst");
		if (!finPos.is_open() || !finNeg.is_open()) {
			cout << "Image File list reading failed!" << endl;
			system("pause");
			return -1;
		}

		Mat sampleFeatureMat;
		Mat sampleLabelMat;
		size_t descriptorDim;

		for (int num = 0; num < POS_NUM && getline(finPos, imgName); ++num) {

			cout << "Procession positive image :" << imgName << " " << num << endl;
			Mat src = imread(imgName);

			if (CENTRAL_CROP) {
				src = src(Rect(16, 16, 64, 128));
			}

			vector<float> descriptors;
			hog.compute(src, descriptors, Size(8, 8));

			if (0 == num) {
				descriptorDim = descriptors.size();
				sampleFeatureMat = Mat::zeros(POS_NUM + AUG_POS_NUM + NEG_NUM + HARD_NEG_NUM, descriptorDim, CV_32FC1);
				sampleLabelMat = Mat::zeros(POS_NUM + AUG_POS_NUM + NEG_NUM + HARD_NEG_NUM, 1, CV_32SC1);
			}
			for (int i = 0; i < descriptorDim; ++i) {
				sampleFeatureMat.at<float>(num, i) = descriptors[i];
			}
			sampleLabelMat.at<int>(num, 0) = 1;
		}
		finPos.close();

		if (AUG_POS_NUM > 0) {
			string augName;
			ifstream finAug("E:/INRIAPerson/Train/pos.lst");
			if (!finAug.is_open()) {
				cout << "Open augment file list failed !" << endl;
				return -1;
			}
			for (int num = 0; num < AUG_POS_NUM && getline(finAug, augName); ++num) {
				cout << "Processing aug image:" << augName << endl;
				Mat src = imread(augName);
				vector<float> descriptors;
				hog.compute(src, descriptors, Size(8, 8));
				for (int i = 0; i < descriptorDim; ++i) {
					sampleFeatureMat.at<float>(num + POS_NUM, i) = descriptors[i];
				}
				
				sampleLabelMat.at<int>(num + POS_NUM, 0) = 1;
			}
			finAug.close();
		}

		for (int num = 0; num < NEG_NUM && getline(finNeg, imgName); ++num) {
			Mat src = imread(imgName);
			vector<float> descriptors;
			hog.compute(src, descriptors, Size(8, 8));

			for (int i = 0; i < descriptorDim; ++i) {
				sampleFeatureMat.at<float>(num + POS_NUM + AUG_POS_NUM, i) = descriptors[i];
			}
			sampleLabelMat.at<int>(num + POS_NUM + AUG_POS_NUM, 0) = -1;

		}
		finNeg.close();

		if (HARD_NEG_NUM > 0) {
			ifstream finHard("E:/INRIAPerson/Train/neg.lst");
			if (!finHard.is_open()) {
				cout << "Open Hard neg file failed!" << endl;
			}

			for (int num = 0; num < HARD_NEG_NUM && getline(finHard, imgName); ++num) {
				Mat src = imread(imgName);
				vector<float> descriptors;

				hog.compute(src, descriptors, Size(8, 8));

				for (uint i = 0; i < descriptorDim; ++i) {
					sampleFeatureMat.at<float>(num + POS_NUM + AUG_POS_NUM + NEG_NUM, i) = descriptors[i];
				}
				sampleLabelMat.at<int>(num + POS_NUM + AUG_POS_NUM + NEG_NUM, 0) = -1;
			}
			finHard.close();
		}

		svm->setType(SVM::C_SVC);
		svm->setC(0.01);
		svm->setKernel(SVM::LINEAR);
		svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 3000, 1e-6));
		
		cout << "Starting training..." << endl;
		svm->train(sampleFeatureMat, ROW_SAMPLE, sampleLabelMat);
		cout << "Finishing training..." << endl;
		svm->save("SVM_HOG.xml");
		Mat svecsmat = svm->getSupportVectors(); // 
		int svdim = svm->getVarCount(); // 特征向量数

		int numofsv = svecsmat.rows;

		Mat alphamat = Mat::zeros(numofsv, svdim, CV_32F);
		Mat svindex = Mat::zeros(1, numofsv, CV_64F);

		Mat Result;
		double rho = svm->getDecisionFunction(0, alphamat, svindex);
		alphamat.convertTo(alphamat, CV_32F);
		Result = -1 * alphamat * svecsmat;

		vector<float> vec;
		for (int i = 0; i < svdim; ++i) {
			vec.push_back(Result.at<float>(0, i));
		}
		vec.push_back(rho);
		ofstream fout("HOGDetectorForOpenCV.txt");
		for (int i = 0; i < vec.size(); ++i) {
			fout << vec[i] << endl;
		}
		fout.close();
		//svm = SVM::load("SVM_HOG.xml");
	}
	else {
		// 测试
		vector<float> vec;
		ifstream vecFile("HOGDetectorForOpenCV.txt");
		float vecElm;
		string line;
		while (getline(vecFile, line)) {
			sscanf(line.c_str(), "%f", &vecElm);
			vec.push_back(vecElm);
		}
		cout << vec.size() << endl;
		HOGDescriptor hog_test;
		hog_test.setSVMDetector(vec);

		Mat src = imread("E:/INRIAPerson/Test/pos/crop001604.png", 1);
		if (src.size() == Size(0, 0)) {
			cout << "Image read failed!" << endl;
			return -1;
		}

		vector<Rect> found, found_filtered;
		hog_test.detectMultiScale(src, found, 0, Size(8, 8), Size(32, 32), 1.05, 2);
		cout << "found.size:" << found.size() << endl;
		for (int i = 0; i < found.size(); i++) {
			Rect r = found[i];
			int j = 0;
			for (; j < found.size(); j++)
				if (j != i && (r & found[j]) == r)
					break;
			if (j == found.size())
				found_filtered.push_back(r);
		}

		for (int i = 0; i < found_filtered.size(); i++) {
			Rect r = found_filtered[i];
			r.x += cvRound(r.width*0.1);
			r.width = cvRound(r.width*0.8);
			r.y += cvRound(r.height*0.07);
			r.height = cvRound(r.height*0.8);
			rectangle(src, r.tl(), r.br(), Scalar(0, 255, 0), 3);
		}
		imwrite("ImgProcessed.jpg", src);
		namedWindow("src", 0);
		imshow("src", src);
		waitKey();
	}
	return 0;
}

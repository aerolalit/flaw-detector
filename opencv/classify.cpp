#include <iostream>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>

using namespace cv;
using namespace std;

double max_difference(Mat reference, Mat image, const char* image_path) {
	Mat diff = (reference - image) + 128;
	char * save_path = new char[strlen(image_path) + 32];
	save_path[0]='\0';
	strcat(save_path, image_path);
	strcat(save_path, ".diff.png");
    imwrite(save_path, diff);
    Mat blurred = Mat(reference.rows, reference.cols, CV_32FC3, Scalar(.0,.0,.0));
	blur(diff, blurred, Size(5,5), Point(-1,-1), BORDER_DEFAULT);
	save_path[0]='\0';
	strcat(save_path, image_path);
	strcat(save_path, ".diff.blur.png");
    imwrite(save_path, blurred);
	
	double min, max;
	minMaxLoc(blurred.reshape(1), &min, &max);
	return max;
}

int main (const int argc, const char * argv[]) {
	const char* reference_image_path = argv[argc - 1];
	
	const char** input_image_paths = new const char*[argc-1];
	
	for (int i=1; i<argc-1; i++) {
		input_image_paths[i-1] = argv[i];
	}
	
	int input_count = argc - 2;
	
	cerr<<"total images " << input_count << endl;
	
    Mat reference_image_byte = imread(reference_image_path,1);
	Mat reference_image = Mat(reference_image_byte.rows, reference_image_byte.cols, CV_32FC3, Scalar(.0,.0,.0));
	reference_image_byte.convertTo(reference_image, CV_32FC3);
	
	for (int i=0 ; i < input_count ; i++) {
		Mat image = imread(input_image_paths[i], 1);
		
		Mat image_float = Mat(image.rows, image.cols, CV_32FC3, Scalar(.0,.0,.0));
		image.convertTo(image_float, CV_32FC3);
    	
    	double max_diff = max_difference(reference_image, image_float, input_image_paths[i]);
    	
    	cout<<input_image_paths[i] << "\t" << max_diff << endl;
	}
}

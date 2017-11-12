#include <iostream>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>

using namespace cv;
using namespace std;


int main (const int argc, const char * argv[]) {
	const char* reference_image_path = argv[argc - 2];
	const char* output_image_path = argv[argc - 1];
	
	const char** input_image_paths = new const char*[argc-1];
	
	for (int i=1; i<argc-2; i++) {
		input_image_paths[i-1] = argv[i];
	}
	
	int input_count = argc - 3;
	
    Mat reference_image_byte = imread(reference_image_path,1);
	Mat reference_image = Mat(reference_image_byte.rows, reference_image_byte.cols, CV_32FC3, Scalar(.0,.0,.0));
	reference_image_byte.convertTo(reference_image, CV_32FC3);
	
	cout<< "helo" << endl;
	
	Mat average_diff = Mat(reference_image.rows, reference_image.cols, CV_32FC3, Scalar(.0,.0,.0));
	
	for (int i=0 ; i < input_count ; i++) {
	
		cerr<<"processing " << i << " of " << input_count <<endl;
		Mat image_bytes = imread(input_image_paths[i], 1);
		
		Mat image = Mat(image.rows, image.cols, CV_32FC3, Scalar(.0,.0,.0));
		image_bytes.convertTo(image, CV_32FC3);
		
		Mat increment = Mat(reference_image_byte.rows, reference_image_byte.cols, CV_32FC3, Scalar(.0,.0,.0));
		pow(reference_image - image, 4, increment);
		average_diff += increment;
	}
	
	Mat result = Mat(reference_image_byte.rows, reference_image_byte.cols, CV_32FC3, Scalar(.0,.0,.0));
	pow(average_diff, 1.0/4, result);
	
	
    imwrite(output_image_path, result);
}

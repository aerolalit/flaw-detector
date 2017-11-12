#include <iostream>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>

using namespace cv;
using namespace std;


int main (const int argc, const char * argv[]) {
	const char* output_image_path = argv[argc - 1];
	
	const char** input_image_paths = new const char*[argc-1];
	
	for (int i=1; i<argc-1; i++) {
		input_image_paths[i-1] = argv[i];
	}
	
	int input_count = argc - 2;
	
	cout<<"total images " << input_count << endl;
	
    Mat first_image = imread(input_image_paths[0],1);
    Mat average = Mat(first_image.rows, first_image.cols, CV_32FC3, Scalar(.0,.0,.0));
    first_image.convertTo(average, CV_32FC3);
    Rect roi(0, 0, first_image.rows - 1, first_image.cols - 1);
    
    cout<<"image " << input_image_paths[0] << " width " << first_image.rows << "; height " << first_image.cols << endl;
	
	for (int i=1 ; i < input_count ; i++) {
		Mat image = imread(input_image_paths[i], 1);
		cout<<"processing image " << i << " of " << "input_count" << endl;
		
		Mat image_float = Mat(image.rows, image.cols, CV_32FC3, Scalar(.0,.0,.0));
		image.convertTo(image_float, CV_32FC3);
    	//cout<<"uncropped image width " << image.rows << "; height " << image.cols;
		//Mat image_cropped = image(roi);
    	//cout<<"cropped image width " << image_cropped.rows << "; height " << image_cropped.cols;
		average += image_float;
	}
	
	average /= input_count;
	
	
    imwrite(output_image_path, average);
}

#include <iostream>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>

using namespace cv;
using namespace std;


//#define SAVE_INTERMEDIATE_IMAGES

double max_difference(Mat reference, Mat image, Mat roi_mask, const char* image_path) {
	Mat diff = abs(reference - image);
#ifdef SAVE_INTERMEDIATE_IMAGES
	char * save_path = new char[strlen(image_path) + 32];
	save_path[0]='\0';
	strcat(save_path, image_path);
	strcat(save_path, ".diff.png");
    imwrite(save_path, diff);
#endif
    
    Mat masked = diff.mul(roi_mask);
    
    Mat blurred = Mat(reference.rows, reference.cols, CV_32FC3, Scalar(.0,.0,.0));
	blur(masked, blurred, Size(5,5), Point(-1,-1), BORDER_DEFAULT);
#ifdef SAVE_INTERMEDIATE_IMAGES    
	save_path[0]='\0';
	strcat(save_path, image_path);
	strcat(save_path, ".diff.blur.png");
    imwrite(save_path, blurred);
#endif
	
	double min, max;
	minMaxLoc(blurred.reshape(1), &min, &max);
	return max;
}

int main (const int argc, const char * argv[]) {
	const char* reference_image_path = argv[argc - 3];
	const char* average_diff_path = argv[argc - 2];
	const char* roi_mask_path = argv[argc - 1];
	
	const char** input_image_paths = new const char*[argc-3];
	
	for (int i=1; i<argc-3; i++) {
		input_image_paths[i-1] = argv[i];
	}
	
	int input_count = argc - 4;
	
	cerr<<"total images " << input_count << endl;
	
	double min, max;
    Mat reference_image_byte = imread(reference_image_path,1);
	Mat reference_image = Mat(reference_image_byte.rows, reference_image_byte.cols, CV_32FC3, Scalar(.0,.0,.0));
	reference_image_byte.convertTo(reference_image, CV_32FC3);
	
    Mat average_diff_byte = imread(average_diff_path,1);
	Mat average_diff = Mat(reference_image_byte.rows, reference_image_byte.cols, CV_32FC3, Scalar(.0,.0,.0));
	average_diff_byte.convertTo(average_diff, CV_32FC3);
	
	minMaxLoc(average_diff, &min, &max);
	cerr<<"average_diff min " << min << " max " << max << endl;
	
    Mat roi_mask_bytes = imread(roi_mask_path,1);
	minMaxLoc(roi_mask_bytes, &min, &max);
	cerr<<"roi_mask_bytes " << min << " max " << max << " mean " << mean(roi_mask_bytes) << endl;
	Mat roi_mask_float = Mat(reference_image_byte.rows, reference_image_byte.cols, CV_32FC3, Scalar(.0,.0,.0));
	roi_mask_bytes.convertTo(roi_mask_float, CV_32FC3);
	Mat roi_mask = roi_mask_float / 255.0;
	minMaxLoc(roi_mask, &min, &max);
	cerr<<"roi_mask min " << min << " max " << max << " mean " << mean(roi_mask) << endl;
	
	Mat common_defect_mask = 1 - (average_diff/255.0);
	
	minMaxLoc(common_defect_mask, &min, &max);
	cerr<<"common_defect_mask min " << min << " max " << max << " mean " << mean(common_defect_mask) << endl;
	
	Mat combined_mask = roi_mask.mul(common_defect_mask);
	
	minMaxLoc(combined_mask, &min, &max);
	cerr<<"combined_mask min " << min << " max " << max << " mean " << mean(combined_mask) << endl;
	
	combined_mask /= max;
	minMaxLoc(combined_mask, &min, &max);
	cerr<<"combined_mask normalized min " << min << " max " << max << " mean " << mean(combined_mask) << endl;
	
	imwrite("combined_mask_normlized.png", combined_mask * 255);
	
	imwrite("roi_mask_rewritten.png", roi_mask * 255);
	
	for (int i=0 ; i < input_count ; i++) {
		Mat image = imread(input_image_paths[i], 1);
		
		Mat image_float = Mat(image.rows, image.cols, CV_32FC3, Scalar(.0,.0,.0));
		image.convertTo(image_float, CV_32FC3);
    	
    	double max_diff = max_difference(reference_image, image_float, combined_mask, input_image_paths[i]);
    	
    	cout<<input_image_paths[i] << "\t" << max_diff << endl;
	}
}

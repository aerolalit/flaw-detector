#include <iostream>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>

using namespace cv;
using namespace std;

#define ECC_ITERATIONS 50
#define ECC_EPSILON	0.1

#define SMALL_IMG_WIDTH 512
#define SMALL_IMG_HEIGHT 425

//#define RESIZE_BEFORE_ALIGNING
//#define SAVE_INTERMEDIATE_IMAGES
#define TIME_STATS

#define ECC_MOTION MOTION_EUCLIDEAN

double max_difference(Mat reference, Mat image, Mat roi_mask) {
	Mat diff = abs(reference - image);
    Mat masked = diff.mul(roi_mask);
    
    
    Mat blurred = Mat(reference.rows, reference.cols, CV_32FC3);
	blur(masked, blurred, Size(5,5), Point(-1,-1), BORDER_DEFAULT);
	
	double min, max;
	minMaxLoc(blurred.reshape(1), &min, &max);
	return max;
}

Mat alignImage(Mat template_image_small, Mat input_image) {
	Mat input_image_mono = input_image.reshape(1);
    
    Mat warp_matrix = Mat::eye(2, 3, CV_32F);
    warp_matrix.rows = 2;
#ifdef RESIZE_BEFORE_ALIGNING    
    Mat input_image_small = Mat(SMALL_IMG_HEIGHT, SMALL_IMG_WIDTH, CV_32FC3);
    resize(input_image_mono, input_image_small, Size(SMALL_IMG_HEIGHT, SMALL_IMG_WIDTH));
#else
	Mat input_image_small = input_image_mono;
#endif
    int number_of_iterations = ECC_ITERATIONS;
    int termination_eps = ECC_EPSILON;
    
    double cc = findTransformECC (template_image_small, input_image_small, warp_matrix, ECC_MOTION,
        TermCriteria (TermCriteria::COUNT+TermCriteria::EPS,
        number_of_iterations, termination_eps));
    
    Mat warped_image = Mat(input_image.rows, input_image.cols, CV_32FC3);
    
    warpAffine(input_image, warped_image, warp_matrix, warped_image.size(),
        INTER_LINEAR + WARP_INVERSE_MAP);
    
    return warped_image;
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
	Mat reference_image = Mat(reference_image_byte.rows, reference_image_byte.cols, CV_32FC3);
	reference_image_byte.convertTo(reference_image, CV_32FC3);
	
    Mat average_diff_byte = imread(average_diff_path,1);
	Mat average_diff = Mat(reference_image_byte.rows, reference_image_byte.cols, CV_32FC3);
	average_diff_byte.convertTo(average_diff, CV_32FC3);
	
    Mat roi_mask_bytes = imread(roi_mask_path,1);
	Mat roi_mask_float = Mat(reference_image_byte.rows, reference_image_byte.cols, CV_32FC3);
	roi_mask_bytes.convertTo(roi_mask_float, CV_32FC3);
	Mat roi_mask = roi_mask_float / 255.0;
	
	Mat common_defect_mask = 1 - (average_diff/255.0);
	
	Mat combined_mask = roi_mask.mul(common_defect_mask);
	
	minMaxLoc(combined_mask, &min, &max);
	
	combined_mask /= max;

#ifdef RESIZE_BEFORE_ALIGNING	
	Mat template_image_mono = reference_image.reshape(1);
    Mat template_image_small = Mat(SMALL_IMG_HEIGHT, SMALL_IMG_WIDTH, CV_32FC3);
    resize(template_image_mono, template_image_small, Size(SMALL_IMG_HEIGHT, SMALL_IMG_WIDTH));
#else
	Mat template_image_small = reference_image.reshape(1);
#endif
	
	for (int i=0 ; i < input_count ; i++) {
#ifdef TIME_STATS
    	const double tic_start = (double) getTickCount ();
#endif
		Mat image = imread(input_image_paths[i], 1);
#ifdef TIME_STATS
		const double tic_read_end = (double) getTickCount ();
		const double read_time = (tic_read_end-tic_start)/(getTickFrequency());
#endif
		Mat image_float = Mat(image.rows, image.cols, CV_32FC3);
		image.convertTo(image_float, CV_32FC3);
		
    	Mat aligned_image = alignImage(template_image_small, image_float);
#ifdef TIME_STATS
		const double tic_align_end = (double) getTickCount ();
		const double align_time = (tic_align_end-tic_read_end)/(getTickFrequency());
#endif
#ifdef SAVE_INTERMEDIATE_IMAGES
		imwrite("last_aligned_image.png", aligned_image);
#endif
    	
    	double max_diff = max_difference(reference_image, aligned_image, combined_mask);
    	
#ifdef TIME_STATS
		const double tic_process_end = (double) getTickCount ();
		const double process_time = (tic_process_end-tic_align_end)/(getTickFrequency());
    	cout<<input_image_paths[i] << "\t" << max_diff << "\t" << read_time << "\t" << align_time << "\t" << process_time << endl;
#else
    	cout<<input_image_paths[i] << "\t" << max_diff << endl;
#endif
	}
}

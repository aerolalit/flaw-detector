
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>

using namespace cv;
using namespace std;

const std::string keys =
    "{@inputImage    | ../data/fruits.jpg | input image filename }"
    "{@templateImage |               | template image filename (optional)}"
    "{@outputImage   |               | input warp (matrix) filename (optional)}"
    "{n numOfIter    | 50            | ECC's iterations }"
    "{e epsilon      | 0.0001        | ECC's convergence epsilon }"
;

int main (const int argc, const char * argv[])
{

    CommandLineParser parser(argc, argv, keys);
    
    
    string input_image_path = parser.get<string>(0);
    string template_image_path = parser.get<string>(1);
    string output_image_path = parser.get<string>(2);
    
    if (!parser.check())
    {
        parser.printErrors();
        return -1;
    }
    
    Mat template_image = imread(template_image_path,0);
    Mat input_image = imread(input_image_path,0);
    
    Mat warp_matrix = Mat::eye(2, 3, CV_32F);
    
        warp_matrix.rows = 2;
    
    int number_of_iterations = parser.get<int>("n");
    int termination_eps = parser.get<int>("e");
    
    double cc = findTransformECC (template_image, input_image, warp_matrix, MOTION_AFFINE,
        TermCriteria (TermCriteria::COUNT+TermCriteria::EPS,
        number_of_iterations, termination_eps));
    
    Mat warped_image = Mat(template_image.rows, template_image.cols, CV_32FC3);
    
    Mat input_image_color = imread(input_image_path,1);
    
    warpAffine(input_image_color, warped_image, warp_matrix, warped_image.size(),
        INTER_LINEAR + WARP_INVERSE_MAP);;

    imwrite(output_image_path, warped_image);
}

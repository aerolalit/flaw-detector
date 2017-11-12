# flaw-detector-opencv

Assigns a score to each input image (printed on standard output). The higher the score stronger the maximum difference between the sample and the average over a 5x5 pixel window.

The maximum score is 255 (very visible defect), but in practice any values above 80 can be considered defective.

## Usage

Each C++ produces a seperate executable, these are meant to be run in the following order (offline preprosessing and online processing parts are kept separate):

### Offline Processing

The following steps are preparations steps that help make the online portion faster a more reliable.

#### `align_image.cpp` and `align_all.sh`:

Compile and put your `align_image` binary on the system PATH (required by the `align_all.sh` script.

    IMAGE_DIR=/path/to/dataset/ OUTPUT_DIR=/output/path/aligned bash align_all.sh

The goal of this step is to pre-align all images for the rest of the offline processing portion. Online processing (`align_and_classify.cpp`) does re-align new images.

#### `compute_average.cpp`

Iterates over all input (aligned) images, and computes their average.

    compute_average /path/to/aligned/dataset/* average.png

#### `compute_average_diff.cpp`

Computes a image that represents the areas that have a lot of variation from sample to sample. This image is used to help ignore common variations or differences due to imperfect alignment.

    compute_average_diff /path/to/aligned/dataset/* average.png average_diff.png

### Online Processing

This is where the actual detection occurs. Two tools are available, one that assumes images are pre-aligned, and another that aligns images on the fly (using OpenCV's `findTransformECC` function).

An image mask can be used to ignore variations in the background, or other irrelevent areas of the picture.

#### `align_and_classify.cpp`

Aligns the input file and compares it to the average printing out a score between 0 and 255 for the input file(s)

    align_and_classify input_image1.tif input_image2.png ...  average.png average_diff.png roi-mask-inner.tif

Example output:
> `input_image1.tif	   18.5836`

The above image is OK, this means that the maximum pixel difference over any 5x5 patch of the image is 18 (of 255).

If compile with the `TIME_STATS` macre, the output will contain three extra colums: time taken to open the file, time taken to align the file, and the time taken to compute the score (in seconds).

### `classify_w_avg_diff.cpp`

This is the same as `align_and_classify` except that:
  * it does not align the input file (assumes pre-aligned file)
  * can be used to preview intermediate processing steps (image files are written in the same directory as the original file for debugging purposes)




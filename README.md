Flaw Detector
============


## What does it do
It detects any flaws in the packages in packaging line and reports it. 
The flaw that it can identify are misplaced, missing or extra characters, any dark spots packages, mistakes in QR codes, etc.
## How we built it
We followed following steps in our algorithm to acheive the results:

*	Aligned images with reference to a base image and then found the **mean** of large random subset of images(our case: **400 images**)


*	We  **cropp**ed out the black background out of image to reduce the computation & noise and used it to make a **mask** which is then further shrinked to remove noise due to missalignment of edges in images.


*	then, We calculate the **weight matrix** which assigns **higher score** to **stable regions** in the image and **lower score** to **unstable regions**(due to misalignment problems, Eg.the BOBST logo, edges of package)

*	Using the above set of precalculated **mean**, **mask** and **weight matrix**, we calculate the squares of the difference between the mean image and test image. This matrix essentially represents the difference(noise) in the image.

*	If the above calculated the noise is greater than the threshold, the image is classified as a image of flawed packaging.

## Challenges we ran into
*	The noise added due to changing number at the button of the image (packaging), misaliged BOBST logo, etc was a causing a hurdle in our project
* **Solution:** We overcome this issue by using the mask and weight matrix.


## Accomplishments that we're proud of
*	**Project:**	Our code successfully detects not just the misalgned, misplaced, missing characters but also defects like dark spots, torn out packaging, and improper color.
*	**Multiple Implementaion** Our team did  pair programming and parallel programming to deliver successful **implementaion in two  languages (Matab & C++)**
*	**Team:** Culturally diverse team with team members from **3** different countries and **3** different universities
			worked together to successfully complete the project

	

## What we learned

We learned some skills in **OpenCV**, **Matlab** & **Image processig** and most importantly working in pairs.


## What's next for flaw-detector

We have used matlab for this project. We plant to optimize the code and implement it in C++ to improve the speed.

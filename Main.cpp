/*** Include ***/
/* for general */
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <chrono>

/* for OpenCV */
#include <opencv2/opencv.hpp>

/* for My modules */
#include "ImageProcessor.h"

/*** Macro ***/
#define IMAGE_NAME   RESOURCE_DIR"/body_female.jpg"
#define WORK_DIR     RESOURCE_DIR
#define LOOP_NUM_FOR_TIME_MEASUREMENT 0


int32_t main()
{
	/*** Initialize ***/
	/* Initialize image processor library */
	INPUT_PARAM inputParam;
	snprintf(inputParam.workDir, sizeof(inputParam.workDir), WORK_DIR);
	inputParam.numThreads = 4;
	ImageProcessor_initialize(&inputParam);

	/* Initialize camera */
	int32_t originalImageWidth = 640;
	int32_t originalImageHeight = 480;

	static cv::VideoCapture cap;
#ifdef _WIN32
	cap = cv::VideoCapture(cv::CAP_DSHOW + 0);
#else
	cap = cv::VideoCapture(0);
#endif
	cap.set(cv::CAP_PROP_FRAME_WIDTH, originalImageWidth);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, originalImageHeight);
	// cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('B', 'G', 'R', '3'));
	cap.set(cv::CAP_PROP_BUFFERSIZE, 1);

	char command[32] = "";

	while (1) {
		const auto& timeAll0 = std::chrono::steady_clock::now();
		/*** Read image ***/
		const auto& timeCap0 = std::chrono::steady_clock::now();
		cv::Mat originalImage;
		cap.read(originalImage);
		const auto& timeCap1 = std::chrono::steady_clock::now();

		/* Call image processor library */
		const auto& timeProcess0 = std::chrono::steady_clock::now();
		OUTPUT_PARAM outputParam;
		ImageProcessor_process(&originalImage, &outputParam);
		const auto& timeProcess1 = std::chrono::steady_clock::now();

		cv::imshow("test", originalImage);
		if (cv::waitKey(1) == 'q') break;

		if (outputParam.command[0] != 0 && strncmp(command, outputParam.command, sizeof(command)) != 0) {
			strncpy(command, outputParam.command, sizeof(command));
			printf("CMD = %s\n", command);
		}

		const auto& timeAll1 = std::chrono::steady_clock::now();
		//printf("Total time = %.3lf [msec]\n", (timeAll1 - timeAll0).count() / 1000000.0);
		//printf("Capture time = %.3lf [msec]\n", (timeCap1 - timeCap0).count() / 1000000.0);
		//printf("Image processing time = %.3lf [msec]\n", (timeProcess1 - timeProcess0).count() / 1000000.0);
		//printf("========\n");
	}

	/* Fianlize image processor library */
	ImageProcessor_finalize();

	return 0;
}

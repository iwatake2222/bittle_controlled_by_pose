/* Copyright 2021 iwatake2222

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

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
#include "Uart.h"

/*** Macro ***/
#define WORK_DIR     RESOURCE_DIR

int32_t main()
{
	/*** Initialize ***/
	/* Initialize uart */
	Uart uart;
	if (uart.initialize("/dev/serial0") < 0) {
		printf("[ERR] uart.initialize\n");
	}

	/* Initialize image processor library */
	INPUT_PARAM inputParam;
	snprintf(inputParam.workDir, sizeof(inputParam.workDir), WORK_DIR);
	inputParam.numThreads = 4;
	ImageProcessor_initialize(&inputParam);

	/* Initialize camera */
	static cv::VideoCapture cap;
#ifdef _WIN32
	cap = cv::VideoCapture(cv::CAP_DSHOW + 0);
#else
	cap = cv::VideoCapture(0);
#endif
	cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
	// cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('B', 'G', 'R', '3'));
	cap.set(cv::CAP_PROP_BUFFERSIZE, 1);

	char command[32] = "";

	while (1) {
		/* Read image */
		cv::Mat originalImage;
		cap.read(originalImage);

		/* Call image processor library */
		OUTPUT_PARAM outputParam;
		ImageProcessor_process(&originalImage, &outputParam);

		/* Display the processed image */
		cv::imshow("test", originalImage);
		if (cv::waitKey(1) == 'q') break;

		/* Send command when it's updated */
		if (outputParam.command[0] != 0 && strncmp(command, outputParam.command, sizeof(command)) != 0) {
			strncpy(command, outputParam.command, sizeof(command));
			printf("CMD = %s\n", command);
			if (uart.send(command) < 0) {
				printf("[ERR] uart.send\n");
			}
		}
	}

	/* Fianlize image processor library */
	ImageProcessor_finalize();
	uart.finalize();

	return 0;
}

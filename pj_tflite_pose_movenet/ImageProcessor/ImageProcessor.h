#ifndef IMAGE_PROCESSOR_H_
#define IMAGE_PROCESSOR_H_

/* for general */
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include <array>

namespace cv {
	class Mat;
};


typedef struct {
	char     workDir[256];
	int32_t  numThreads;
} INPUT_PARAM;

typedef struct {
	double timePreProcess;   // [msec]
	double timeInference;    // [msec]
	double timePostProcess;  // [msec]
} OUTPUT_PARAM;

int32_t ImageProcessor_initialize(const INPUT_PARAM* inputParam);
int32_t ImageProcessor_process(cv::Mat* mat, OUTPUT_PARAM* outputParam);
int32_t ImageProcessor_finalize(void);
int32_t ImageProcessor_command(int32_t cmd);

#endif

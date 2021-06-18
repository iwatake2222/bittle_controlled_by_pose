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
#include <fstream>
#include <memory>

/* for OpenCV */
#include <opencv2/opencv.hpp>

/* for My modules */
#include "CommonHelper.h"
#include "PoseEngine.h"
#include "PoseAnalyzer.h"
#include "CommandDecider.h"
#include "ImageProcessor.h"

/*** Macro ***/
#define TAG "ImageProcessor"
#define PRINT(...)   COMMON_HELPER_PRINT(TAG, __VA_ARGS__)
#define PRINT_E(...) COMMON_HELPER_PRINT_E(TAG, __VA_ARGS__)

static const std::vector<std::pair<int32_t, int32_t>> jointLineList {
	/* face */
	{0, 2},
	{2, 4},
	{0, 1},
	{1, 3},
	/* body */
	{6, 5},
	{5, 11},
	{11, 12},
	{12, 6},
	/* arm */
	{6, 8},
	{8, 10},
	{5, 7},
	{7, 9},
	/* leg */
	{12, 14},
	{14, 16},
	{11, 13},
	{13, 15},
};

/*** Global variable ***/
std::unique_ptr<PoseEngine> s_poseEngine;
PoseAnalyzer s_poseAnalyzer;
CommandDecider s_commandDecider;

/*** Function ***/
static cv::Scalar createCvColor(int32_t b, int32_t g, int32_t r) {
#ifdef CV_COLOR_IS_RGB
	return cv::Scalar(r, g, b);
#else
	return cv::Scalar(b, g, r);
#endif
}

void drawPose(cv::Mat& mat, const std::vector<std::pair<float, float>> jointList, std::vector<float> scoreList);

int32_t ImageProcessor_initialize(const INPUT_PARAM* inputParam)
{
	if (s_poseEngine) {
		PRINT_E("Already initialized\n");
		return -1;
	}

	s_poseEngine.reset(new PoseEngine());
	if (s_poseEngine->initialize(inputParam->workDir, inputParam->numThreads) != PoseEngine::RET_OK) {
		return -1;
	}
	return 0;
}

int32_t ImageProcessor_finalize(void)
{
	if (!s_poseEngine) {
		PRINT_E("Not initialized\n");
		return -1;
	}

	if (s_poseEngine->finalize() != PoseEngine::RET_OK) {
		return -1;
	}

	return 0;
}


int32_t ImageProcessor_command(int32_t cmd)
{
	if (!s_poseEngine) {
		PRINT_E("Not initialized\n");
		return -1;
	}

	switch (cmd) {
	case 0:
	default:
		PRINT_E("command(%d) is not supported\n", cmd);
		return -1;
	}
}


int32_t ImageProcessor_process(cv::Mat* mat, OUTPUT_PARAM* outputParam)
{
	if (!s_poseEngine) {
		PRINT_E("Not initialized\n");
		return -1;
	}

	/* Run inference */
	cv::Mat& originalMat = *mat;
	PoseEngine::RESULT result;
	if (s_poseEngine->invoke(originalMat, result) != PoseEngine::RET_OK) {
		return -1;
	}

	/* note: we have only one body with this model */
	const auto& scoreList = result.poseKeypointScores[0];
	const auto& jointList = result.poseKeypointCoords[0];

	/* Analyze Pose */
	PoseAnalyzer::RESULT poseResult;
	(void)s_poseAnalyzer.analyze(jointList, scoreList, poseResult);
	std::string command = s_commandDecider.decide(poseResult);

	/* Draw the result */
	drawPose(originalMat, jointList, scoreList);

	char text[32];
	snprintf(text, sizeof(text), "score = %.3f, x = %.2f", poseResult.faceScore, poseResult.x);
	cv::putText(originalMat, text, cv::Point(50, 20), cv::FONT_HERSHEY_SIMPLEX, 0.8, createCvColor(255, 0, 0), 2);
	snprintf(text, sizeof(text), "raise = %d %d", poseResult.armLeftRaised, poseResult.armRightRaised);
	cv::putText(originalMat, text, cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 0.8, createCvColor(255, 0, 0), 2);
	snprintf(text, sizeof(text), "spread = %d %d", poseResult.armLeftSpread, poseResult.armRightSpread);
	cv::putText(originalMat, text, cv::Point(50, 80), cv::FONT_HERSHEY_SIMPLEX, 0.8, createCvColor(255, 0, 0), 2);
	snprintf(text, sizeof(text), "forward = %d %d", poseResult.armLeftForward, poseResult.armRightForward);
	cv::putText(originalMat, text, cv::Point(50, 110), cv::FONT_HERSHEY_SIMPLEX, 0.8, createCvColor(255, 0, 0), 2);
	snprintf(text, sizeof(text), "crunching = %d", poseResult.crunching);
	cv::putText(originalMat, text, cv::Point(50, 140), cv::FONT_HERSHEY_SIMPLEX, 0.8, createCvColor(255, 0, 0), 2);
	cv::putText(originalMat, command.c_str(), cv::Point(50, 200), cv::FONT_HERSHEY_SIMPLEX, 1.0, createCvColor(255, 0, 0), 2);

	/* Return the results */
	outputParam->timePreProcess = result.timePreProcess;
	outputParam->timeInference = result.timeInference;
	outputParam->timePostProcess = result.timePostProcess;
	snprintf(outputParam->command, sizeof(outputParam->command), "%s", command.c_str());

	return 0;
}

void drawPose(cv::Mat& mat, const std::vector<std::pair<float, float>> jointList, std::vector<float> scoreList)
{
	constexpr float scoreThreshold = 0.2f;
	int32_t jointNum = static_cast<int32_t>(jointList.size());

	for (const auto& jointLine : jointLineList) {
		if (scoreList[jointLine.first] >= scoreThreshold && scoreList[jointLine.second] >= scoreThreshold) {
			int32_t x0 = static_cast<int32_t>(jointList[jointLine.first].first * mat.cols);
			int32_t y0 = static_cast<int32_t>(jointList[jointLine.first].second * mat.rows);
			int32_t x1 = static_cast<int32_t>(jointList[jointLine.second].first * mat.cols);
			int32_t y1 = static_cast<int32_t>(jointList[jointLine.second].second * mat.rows);
			cv::line(mat, cv::Point(x0, y0), cv::Point(x1, y1), createCvColor(200, 200, 200), 2);
		}
	}

	for (int32_t i = 0; i < jointNum; i++) {
		int32_t x = static_cast<int32_t>(jointList[i].first * mat.cols);
		int32_t y = static_cast<int32_t>(jointList[i].second * mat.rows);
		float score = scoreList[i];
		if (score >= scoreThreshold) {
			cv::circle(mat, cv::Point(x, y), 5, createCvColor(0, 255, 0), -1);
			char text[4];
			snprintf(text, sizeof(text), "%d", i);
			cv::putText(mat, text, cv::Point(x, y), cv::FONT_HERSHEY_SIMPLEX, 0.5, createCvColor(255, 0, 0), 1);
		}
	}
}

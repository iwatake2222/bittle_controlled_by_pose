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

/* for My modules */
#include "CommonHelper.h"
#include "PoseAnalyzer.h"

/*** Macro ***/
#define TAG "PoseAnalyzer"
#define PRINT(...)   COMMON_HELPER_PRINT(TAG, __VA_ARGS__)
#define PRINT_E(...) COMMON_HELPER_PRINT_E(TAG, __VA_ARGS__)

#define THRESHOLD_SCORE 0.2f
#define GET_X_POS(index) ((scoreList[index] < THRESHOLD_SCORE) ? -1 : partList[index].first)
#define GET_Y_POS(index) ((scoreList[index] < THRESHOLD_SCORE) ? -1 : partList[index].second)

/*** Function ***/

int32_t PoseAnalyzer::analyze(const std::vector<std::pair<float, float>> partList, std::vector<float> scoreList, int32_t& arm, int32_t& body)
{
    arm = POSE_NORMAL;
    body = BODY_NORMAL;

    for (int32_t i = 0; i < 17; i++) {
        printf("%f %f\n", GET_X_POS(i), GET_Y_POS(i));
    }

    return RET_OK;
}


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

static const std::vector<std::pair<int32_t, int32_t>> ARM_LIST = {
    {10, 8},
    {8, 6},
    {9, 7},
    {7, 5},
};

static const std::vector<std::pair<int32_t, int32_t>> BODY_LIST = {
    {6, 5},
    {5, 11},
    {11, 12},
    {12, 6},
};

/*** Function ***/
int32_t PoseAnalyzer::analyze(const std::vector<std::pair<float, float>> partList, std::vector<float> scoreList, RESULT& result)
{
    RESULT currentResult;

    float armLength = calcualteAverageLength(partList, scoreList, ARM_LIST);
    float bodyLength = calcualteAverageLength(partList, scoreList, BODY_LIST);
    if (armLength < 0) armLength = bodyLength;
    if (bodyLength < 0) bodyLength = armLength;
    const float armDistanceThreshold = armLength / 3;
    const float bodyDistanceThreshold = bodyLength / 3;

    if ((scoreList[10] > THRESHOLD_SCORE && scoreList[8] > THRESHOLD_SCORE && scoreList[6] > THRESHOLD_SCORE)
        && (GET_Y_POS(10) + armDistanceThreshold < GET_Y_POS(6))) {
        currentResult.armLeftRaised = true;
    }

    if ((scoreList[9] > THRESHOLD_SCORE && scoreList[7] > THRESHOLD_SCORE && scoreList[5] > THRESHOLD_SCORE)
        && (GET_Y_POS(9) + armDistanceThreshold < GET_Y_POS(5))) {
        currentResult.armRightRaised = true;
    }

    if ((scoreList[10] > THRESHOLD_SCORE && scoreList[8] > THRESHOLD_SCORE && scoreList[6] > THRESHOLD_SCORE)
        && (GET_X_POS(10) + armDistanceThreshold < GET_X_POS(8))
        && (GET_X_POS(8) + armDistanceThreshold < GET_X_POS(6))
        ) {
        currentResult.armLeftSpread = true;
    }

    if ((scoreList[9] > THRESHOLD_SCORE && scoreList[7] > THRESHOLD_SCORE && scoreList[5] > THRESHOLD_SCORE)
        && (GET_X_POS(9) > GET_X_POS(7) + armDistanceThreshold)
        && (GET_X_POS(7) > GET_X_POS(5) + armDistanceThreshold)
        ) {
        currentResult.armRightSpread = true;
    }

    if ((scoreList[10] > THRESHOLD_SCORE && scoreList[6] > THRESHOLD_SCORE)
        && ((std::abs)(GET_Y_POS(10) - GET_Y_POS(6)) < bodyDistanceThreshold && (std::abs)(GET_X_POS(10) - GET_X_POS(6)) < bodyDistanceThreshold)) {
        currentResult.armLeftForward = true;
    }

    if ((scoreList[9] > THRESHOLD_SCORE && scoreList[5] > THRESHOLD_SCORE)
        && ((std::abs)(GET_Y_POS(9) - GET_Y_POS(5)) < bodyDistanceThreshold && (std::abs)(GET_X_POS(9) - GET_X_POS(5)) < bodyDistanceThreshold)) {
        currentResult.armRightForward = true;
    }

    float waistY = -1;
    if (scoreList[12] > THRESHOLD_SCORE) {
        waistY = GET_Y_POS(12);
    }
    
    if (scoreList[11] > THRESHOLD_SCORE) {
        waistY += GET_Y_POS(11);
        waistY /= 2;
    }
    if ( ((scoreList[14] > THRESHOLD_SCORE) && (GET_Y_POS(14) < waistY + bodyDistanceThreshold))
        || ((scoreList[13] > THRESHOLD_SCORE) && (GET_Y_POS(13) < waistY + bodyDistanceThreshold))) {
        currentResult.crunching = true;
    }

    
    currentResult.x = GET_X_POS(0);
    currentResult.y = GET_Y_POS(0);
    if (currentResult.x < 0) {
        if (scoreList[6] > THRESHOLD_SCORE && scoreList[5] > THRESHOLD_SCORE) {
            currentResult.x = (GET_X_POS(6) + GET_X_POS(5)) / 2;
            currentResult.y = (GET_Y_POS(6) + GET_Y_POS(5)) / 2;
        }
    }
    if (currentResult.x >= 0) {
        currentResult.x = (currentResult.x - 0.5) * 2;
        currentResult.y = (currentResult.y - 0.5) * 2;
    } else {
        currentResult.x = 0;
        currentResult.y = 0;
    }

    filterResult(currentResult, result);

    return RET_OK;
}

void PoseAnalyzer::filterResult(const RESULT& currentResult, RESULT& result)
{
    m_resultList.push_back(currentResult);
    if (m_resultList.size() > NUM_FILTERING) {
        m_resultList.pop_front();
    }

    const int32_t NUM_THRESHOLD = m_resultList.size() * 0.8;
    int32_t numArmLeftRaised = 0;
    int32_t numArmRightRaised = 0;
    int32_t numArmLeftSpread = 0;
    int32_t numArmRightSpread = 0;
    int32_t numArmLeftForward = 0;
    int32_t numArmRightForward = 0;
    int32_t numCrunching = 0;

    for (const auto& r : m_resultList) {
        if (r.armLeftRaised) numArmLeftRaised++;
        if (r.armRightRaised) numArmRightRaised++;
        if (r.armLeftSpread) numArmLeftSpread++;
        if (r.armRightSpread) numArmRightSpread++;
        if (r.armLeftForward) numArmLeftForward++;
        if (r.armRightForward) numArmRightForward++;
        if (r.crunching) numCrunching++;
    }

    if (numArmLeftRaised >= NUM_THRESHOLD) result.armLeftRaised = true;
    if (numArmRightRaised >= NUM_THRESHOLD) result.armRightRaised = true;
    if (numArmLeftSpread >= NUM_THRESHOLD) result.armLeftSpread = true;
    if (numArmRightSpread >= NUM_THRESHOLD) result.armRightSpread = true;
    if (numArmLeftForward >= NUM_THRESHOLD) result.armLeftForward = true;
    if (numArmRightForward >= NUM_THRESHOLD) result.armRightForward = true;
    if (numCrunching >= NUM_THRESHOLD) result.crunching = true;

    result.x = currentResult.x;
    result.y = currentResult.y;
}

float PoseAnalyzer::calculateLength(const std::vector<std::pair<float, float>> partList, std::vector<float> scoreList, int32_t index0, int32_t index1)
{
    float length = -1;
    if (scoreList[index0] > THRESHOLD_SCORE && scoreList[index1] > THRESHOLD_SCORE) {
        length = std::powf(GET_X_POS(index0) - GET_X_POS(index1), 2) + std::powf(GET_Y_POS(index0) - GET_Y_POS(index1), 2);
        length = std::sqrtf(length);
    }
    return length;
}

float PoseAnalyzer::calcualteAverageLength(const std::vector<std::pair<float, float>> partList, std::vector<float> scoreList, std::vector<std::pair<int32_t, int32_t>> indexPairList)
{
    std::vector<float> lengthList;
    for (const auto& indexPair : indexPairList) {
        lengthList.push_back(calculateLength(partList, scoreList, indexPair.first, indexPair.second));
    }

    float sum = 0;
    int32_t num = 0;
    for (const auto& length : lengthList) {
        if (length > 0) {
            sum += length;
            num++;
        }
    }

    if (num == 0) {
        return -1;
    } else {
        return sum / num;
    }
}
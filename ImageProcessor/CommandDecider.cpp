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
#include "CommandDecider.h"

/*** Macro ***/
#define TAG "CommandDecider"
#define PRINT(...)   COMMON_HELPER_PRINT(TAG, __VA_ARGS__)
#define PRINT_E(...) COMMON_HELPER_PRINT_E(TAG, __VA_ARGS__)


/*** Function ***/
std::string CommandDecider::decide(PoseAnalyzer::RESULT& poseResult)
{
    /*** Decide a status candidate using the curent pose ***/
    int32_t status = m_status;
    switch (m_status) {
    default:
    case STATUS_NONE:
    case STATUS_MOVING_FORWARD:
        if (poseResult.faceScore > 0.3) {
            status = STATUS_MOVING_FORWARD;
        }
        if (poseResult.faceScore <= 0.3) {
            status = STATUS_NONE;
        } 
        if (poseResult.x > 0.5) {
            status = STATUS_MOVING_RIGHT;
        }
        if (poseResult.x < -0.5) {
            status = STATUS_MOVING_LEFT;
        }
        if (poseResult.armLeftRaised) {
            status = STATUS_MOVING_BACKWARD;
        }
        if (poseResult.armLeftSpread) {
            status = STATUS_ACTION_SIT;
        }
        if (poseResult.armLeftForward) {
            status = STATUS_MOVING_STOP;
        }
        if (poseResult.crunching) {
            status = STATUS_MOVING_STOP;
        }
        break;
    case STATUS_MOVING_RIGHT:
    case STATUS_MOVING_LEFT:
        if (-0.25 < poseResult.x && poseResult.x < 0.25) {
            status = STATUS_NONE;
        }
        if (poseResult.armLeftRaised) {
            status = STATUS_MOVING_BACKWARD;
        }
        if (poseResult.armLeftForward) {
            status = STATUS_MOVING_STOP;
        }
        if (poseResult.crunching) {
            status = STATUS_MOVING_STOP;
        }
        break;
    case STATUS_MOVING_BACKWARD:
        if (!poseResult.armLeftRaised) {
            status = STATUS_NONE;
        }
        break;
    case STATUS_ACTION_SIT:
        if (!poseResult.armLeftSpread) {
            status = STATUS_NONE;
        }
        break;
    case STATUS_MOVING_STOP:
    case STATUS_ACTION_HI:
        if (poseResult.crunching) {
            if (poseResult.armLeftForward) {
                status = STATUS_ACTION_HI;
            } else {
                status = STATUS_MOVING_STOP;
            }
        } else {
            if (!poseResult.armLeftForward) {
                status = STATUS_NONE;
            }
        }
        break;
    }
    if (poseResult.faceScore < 0.3) {
        status = STATUS_NONE;
    }

    /*** Filter status ***/
    m_statusHistory.push_back(status);
    if (m_statusHistory.size() > NUM_FILTERING) {
        m_statusHistory.pop_front();
    }

    bool isStatusStable = true;
    for (const auto& h : m_statusHistory) {
        if (status != h) {
            isStatusStable = false;
            break;
        }
    }

    /*** Send command if the status stable and the status changed ***/
    if (isStatusStable /* && (m_status != status) */) {
        m_status = status;

        std::string cmd = "";
        switch (m_status) {
        default:
        case STATUS_NONE:
            cmd = "kbalance";
            break;
        case STATUS_MOVING_FORWARD:
            cmd = "kcrF";
            break;
        case STATUS_MOVING_BACKWARD:
            cmd = "kbk";
            break;
        case STATUS_MOVING_STOP:
            cmd = "kbalance";
            break;
        case STATUS_MOVING_LEFT:
            cmd = "kwkL";
            break;
        case STATUS_MOVING_RIGHT:
            cmd = "kwkR";
            break;
        case STATUS_ACTION_SIT:
            cmd = "ksit";
            break;
        case STATUS_ACTION_HI:
            cmd = "khi";
            break;
        }
        //printf("%s\n", cmd.c_str());
        return cmd;
    }

    return "";
}

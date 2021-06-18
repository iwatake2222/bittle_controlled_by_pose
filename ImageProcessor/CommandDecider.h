#ifndef COMMAND_DECIDER_
#define COMMAND_DECIDER_

/* for general */
#include <cstdint>
#include <cmath>
#include <string>
#include <deque>
#include <array>
#include <memory>

#include "PoseAnalyzer.h"

class CommandDecider {

private:
	static constexpr int32_t NUM_FILTERING = 10;

	enum {
		STATUS_NONE = 0,
		STATUS_MOVING_FORWARD,
		STATUS_MOVING_BACKWARD,
		STATUS_MOVING_STOP,
		STATUS_MOVING_LEFT,
		STATUS_MOVING_RIGHT,
		STATUS_ACTION_SIT,
		STATUS_ACTION_HI,
	};

public:
	CommandDecider() 
		: m_status(STATUS_NONE)
	{}
	~CommandDecider() {}
	
	std::string decide(PoseAnalyzer::RESULT& poseResult);

private:
	int32_t m_status;
	std::deque<int32_t> m_statusHistory;
};

#endif

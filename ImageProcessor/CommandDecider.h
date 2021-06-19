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

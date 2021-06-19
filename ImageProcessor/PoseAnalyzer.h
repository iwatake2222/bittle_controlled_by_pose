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

#ifndef POSE_ANALYZER_
#define POSE_ANALYZER_

/* for general */
#include <cstdint>
#include <cmath>
#include <string>
#include <deque>
#include <array>
#include <memory>

class PoseAnalyzer {

public:
	static constexpr int32_t NUM_FILTERING = 10;

	enum {
		RET_OK = 0,
		RET_ERR = -1,
	};


	typedef struct RESULT_ {
		bool armLeftRaised;
		bool armRightRaised;
		bool armLeftSpread;
		bool armRightSpread;
		bool armLeftForward;
		bool armRightForward;
		bool crunching;
		float x;	// -1.0 ~ 0.0(center) ~ 1.0
		float y;	// -1.0 ~ 0.0(center) ~ 1.0
		float faceScore;
		RESULT_()
			: armLeftRaised(false)
			, armRightRaised(false)
			, armLeftSpread(false)
			, armRightSpread(false)
			, armLeftForward(false)
			, armRightForward(false)
			, crunching(false)
			, x(0)
			, y(0)
			, faceScore(0)
		{}
	} RESULT;

public:
	PoseAnalyzer() {}
	~PoseAnalyzer() {}
	
	int32_t analyze(const std::vector<std::pair<float, float>> jointList, std::vector<float> scoreList, PoseAnalyzer::RESULT& result);

private:
	float calculateLength(const std::vector<std::pair<float, float>> jointList, std::vector<float> scoreList, int32_t index0, int32_t index1);
	float calcualteAverageLength(const std::vector<std::pair<float, float>> jointList, std::vector<float> scoreList, std::vector<std::pair<int32_t, int32_t>> indexPairList);
	void  filterResult(const PoseAnalyzer::RESULT& currentResult, PoseAnalyzer::RESULT& result);

private:
	std::deque<RESULT> m_resultList;
};

#endif

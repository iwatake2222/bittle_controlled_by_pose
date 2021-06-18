#ifndef POSE_ANALYZER_
#define POSE_ANALYZER_

/* for general */
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include <array>
#include <memory>

class PoseAnalyzer {
public:
	enum {
		POSE_NORMAL = 0,
		POSE_ARM_FORWARD_LEFT,	/* Put a left arm forward */
		POSE_ARM_FORWARD_RIGHT,	/* Put a right arm forward */
		POSE_ARM_FORWARD_BOTH,	/* Put both armss forward */
		POSE_ARM_SPREAD_LEFT,	/* Spread a left arm out to the left */
		POSE_ARM_SPREAD_RIGHT,	/* Spread a right arm out to the right */
		POSE_ARM_SPREAD_BOTH,	/* Spread both arms out to the side */
		POSE_ARM_RAISE_LEFT,	/* Raise a left arm */
		POSE_ARM_RAISE_RIGHT,	/* Raise a right arm */
		POSE_ARM_RAISE_BOTH,	/* Raise both arms */
	};
	
	enum {
		BODY_NORMAL = 0,
		BODY_CROUCHING,
	};

	enum {
		RET_OK = 0,
		RET_ERR = -1,
	};

public:
	PoseAnalyzer() {}
	~PoseAnalyzer() {}
	
	int32_t analyze(const std::vector<std::pair<float, float>> partList, std::vector<float> scoreList, int32_t& arm, int32_t& body);
};

#endif

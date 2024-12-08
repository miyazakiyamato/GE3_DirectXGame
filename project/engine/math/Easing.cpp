#include "Easing.h"
#include <cmath>
#include <numbers>

float Easing::EaseInSine(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	float easeIn = 1.0f - cosf((time * std::numbers::pi_v<float>) / 2);
	return (1.0f - easeIn) * startPos + easeIn * endPos;
}
float Easing::EaseOutSine(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	float easeOut = sinf((time * std::numbers::pi_v<float>) / 2);
	return (1.0f - easeOut) * startPos + easeOut * endPos;
}
float Easing::EaseInOutSine(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	float easeInOut = -(cosf((std::numbers::pi_v<float>)*time) - 1) / 2;
	return (1.0f - easeInOut) * startPos + easeInOut * endPos;
}
float Easing::EaseInQuad(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	float easeIn = time * time;
	return (1.0f - easeIn) * startPos + easeIn * endPos;
}
float Easing::EaseOutQuad(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	float easeOut = 1.0f - (1.0f - time) * (1.0f - time);
	return (1.0f - easeOut) * startPos + easeOut * endPos;
}
float Easing::EaseInOutQuad(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	float easeInOut = time < 0.5 ? 2 * time * time : 1 - powf(-2 * time + 2, 2) / 2;
	return (1.0f - easeInOut) * startPos + easeInOut * endPos;
}
float Easing::EaseInCubic(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	float easeIn = time * time * time;
	return (1.0f - easeIn) * startPos + easeIn * endPos;
}
float Easing::EaseOutCubic(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	float easeOut = 1.0f - powf(1 - time, 3);
	return (1.0f - easeOut) * startPos + easeOut * endPos;
}
float Easing::EaseInOutCubic(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	float easeInOut = time < 0.5f ? 4 * time * time * time : 1.0f - powf(-2 * time + 2, 3) / 2;
	return (1.0f - easeInOut) * startPos + easeInOut * endPos;
}
float Easing::EaseInQuart(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	float easeIn = time * time * time * time;
	return (1.0f - easeIn) * startPos + easeIn * endPos;
}
float Easing::EaseOutQuart(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	float easeOut = 1.0f - powf(1 - time, 4);
	return (1.0f - easeOut) * startPos + easeOut * endPos;
}
float Easing::EaseInOutQuart(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	float easeInOut = time < 0.5f ? 8 * time * time * time * time : 1.0f - powf(-2 * time + 2, 4) / 2;
	return (1.0f - easeInOut) * startPos + easeInOut * endPos;
}
float Easing::EaseInQuint(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	float easeIn = time * time * time * time * time;
	return (1.0f - easeIn) * startPos + easeIn * endPos;
}
float Easing::EaseOutQuint(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	float easeOut = 1.0f - powf(1 - time, 5);
	return (1.0f - easeOut) * startPos + easeOut * endPos;
}
float Easing::EaseInOutQuint(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	float easeInOut = time < 0.5f ? 16 * time * time * time * time * time : 1.0f - powf(-2 * time + 2, 5) / 2;
	return (1.0f - easeInOut) * startPos + easeInOut * endPos;
}
float Easing::EaseInExpo(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	float easeIn = time == 0 ? 0 : powf(2, 10 * time - 10);
	return (1.0f - easeIn) * startPos + easeIn * endPos;
}
float Easing::EaseOutExpo(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	float easeOut = time == 1 ? 1 : 1 - powf(2, -10 * time);
	return (1.0f - easeOut) * startPos + easeOut * endPos;
}
float Easing::EaseInOutExpo(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	float easeInOut = time == 0 ? 0 : time == 1 ? 1 : time < 0.5 ? powf(2, 20 * time - 10) / 2 : (2 - powf(2, -20 * time + 10)) / 2;
	return (1.0f - easeInOut) * startPos + easeInOut * endPos;
}
float Easing::EaseInCirc(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	float easeIn = 1 - sqrtf(1 - powf(time, 2));
	return (1.0f - easeIn) * startPos + easeIn * endPos;
}
float Easing::EaseOutCirc(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	float easeOut = sqrtf(1 - powf(time - 1, 2));
	return (1.0f - easeOut) * startPos + easeOut * endPos;
}
float Easing::EaseInOutCirc(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	float easeInOut = time < 0.5 ? (1 - sqrtf(1 - powf(2 * time, 2))) / 2 : (sqrtf(1 - powf(-2 * time + 2, 2)) + 1) / 2;
	return (1.0f - easeInOut) * startPos + easeInOut * endPos;
}
float Easing::EaseInBack(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	const float c1 = 1.70158f;
	const float c3 = c1 + 1;
	float easeIn = c3 * time * time * time - c1 * time * time;
	return (1.0f - easeIn) * startPos + easeIn * endPos;
}
float Easing::EaseOutBack(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	const float c1 = 1.70158f;
	const float c3 = c1 + 1;
	float easeOut = 1 + c3 * powf(time - 1, 3) + c1 * powf(time - 1, 2);
	return (1.0f - easeOut) * startPos + easeOut * endPos;
}
float Easing::EaseInOutBack(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	const float c1 = 1.70158f;
	const float c2 = c1 * 1.525f;
	float easeInOut = time < 0.5f ? (powf(2 * time, 2) * ((c2 + 1) * 2 * time - c2)) / 2 : (powf(2 * time - 2, 2) * ((c2 + 1) * (time * 2 - 2) + c2) + 2) / 2;
	return (1.0f - easeInOut) * startPos + easeInOut * endPos;
}
float Easing::EaseInElastic(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	const float c4 = (2 * std::numbers::pi_v<float>) / 3;
	float easeIn = time == 0 ? 0 : time == 1 ? 1 : -powf(2, 10 * time - 10) * sinf((time * 10 - 10.75f) * c4);
	return (1.0f - easeIn) * startPos + easeIn * endPos;
}
float Easing::EaseOutElastic(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	const float c4 = (2 * std::numbers::pi_v<float>) / 3;
	float easeOut = time == 0 ? 0 : time == 1 ? 1 : powf(2, -10 * time) * sinf((time * 10 - 0.75f) * c4) + 1;
	return (1.0f - easeOut) * startPos + easeOut * endPos;
}
float Easing::EaseInOutElastic(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	const float c5 = (2 * std::numbers::pi_v<float>) / 4.5f;
	float easeInOut = time == 0
		? 0
        : time == 1 
		? 1
        : time < 0.5f
		? -(powf(2, 20 * time - 10) * sinf((20 * time - 11.125f) * c5)) / 2 
		: (powf(2, -20 * time + 10) * sinf((20 * time - 11.125f) * c5)) / 2 + 1;
return (1.0f - easeInOut) * startPos + easeInOut * endPos;
}
float Easing::EaseInBounce(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	const float n1 = 7.5625f;
	const float d1 = 2.75f;
	float easeOutTime = 1.0f - time;
	float easeOut;
	if (easeOutTime < 1 / d1) {
		easeOut = n1 * easeOutTime * easeOutTime;
	} else if (easeOutTime < 2 / d1) {
		easeOut = n1 * (easeOutTime -= 1.5f / d1) * easeOutTime + 0.75f;
	} else if (easeOutTime < 2.5f / d1) {
		easeOut = n1 * (easeOutTime -= 2.25f / d1) * easeOutTime + 0.9375f;
	} else {
		easeOut = n1 * (easeOutTime -= 2.625f / d1) * easeOutTime + 0.984375f;
	}
		float easeIn = 1 - easeOut;
	return (1.0f - easeIn) * startPos + easeIn * endPos;
}
float Easing::EaseOutBounce(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	const float n1 = 7.5625f;
	const float d1 = 2.75f;
	float easeOut;
	if (time < 1 / d1) {
		easeOut = n1 * time * time;
	} else if (time < 2 / d1) {
		easeOut = n1 * (time -= 1.5f / d1) * time + 0.75f;
	} else if (time < 2.5f / d1) {
		easeOut = n1 * (time -= 2.25f / d1) * time + 0.9375f;
	} else {
		easeOut = n1 * (time -= 2.625f / d1) * time + 0.984375f;
	}
	return (1.0f - easeOut) * startPos + easeOut * endPos;
}
float Easing::EaseInOutBounce(float time, float startPos, float endPos) {
	if (time >= 1.0f) {return endPos;}
	const float n1 = 7.5625f;
	const float d1 = 2.75f;
	float easeOutTime = time < 0.5f ? 1 - 2 * time : 2 * time - 1;
	float easeOut;
	if (easeOutTime < 1 / d1) {
		easeOut = n1 * easeOutTime * easeOutTime;
	} else if (easeOutTime < 2 / d1) {
		easeOut = n1 * (easeOutTime -= 1.5f / d1) * easeOutTime + 0.75f;
	} else if (easeOutTime < 2.5f / d1) {
		easeOut = n1 * (easeOutTime -= 2.25f / d1) * easeOutTime + 0.9375f;
	} else {
		easeOut = n1 * (easeOutTime -= 2.625f / d1) * easeOutTime + 0.984375f;
	}
	float easeInOut = time < 0.5f ? (1 - easeOut) / 2 : (1 + easeOut) / 2;
	return (1.0f - easeInOut) * startPos + easeInOut * endPos;
}
#pragma once
class Easing {
public:
	static float EaseInSine(float time, float startPos, float endPos);
	static float EaseOutSine(float time, float startPos, float endPos);
	static float EaseInOutSine(float time, float startPos, float endPos);
	static float EaseInQuad(float time, float startPos, float endPos);
	static float EaseOutQuad(float time, float startPos, float endPos);
	static float EaseInOutQuad(float time, float startPos, float endPos);
	static float EaseInCubic(float time, float startPos, float endPos);
	static float EaseOutCubic(float time, float startPos, float endPos);
	static float EaseInOutCubic(float time, float startPos, float endPos);
	static float EaseInQuart(float time, float startPos, float endPos);
	static float EaseOutQuart(float time, float startPos, float endPos);
	static float EaseInOutQuart(float time, float startPos, float endPos);
	static float EaseInQuint(float time, float startPos, float endPos);
	static float EaseOutQuint(float time, float startPos, float endPos);
	static float EaseInOutQuint(float time, float startPos, float endPos);
	static float EaseInExpo(float time, float startPos, float endPos);
	static float EaseOutExpo(float time, float startPos, float endPos);
	static float EaseInOutExpo(float time, float startPos, float endPos);
	static float EaseInCirc(float time, float startPos, float endPos);
	static float EaseOutCirc(float time, float startPos, float endPos);
	static float EaseInOutCirc(float time, float startPos, float endPos);
	static float EaseInBack(float time, float startPos, float endPos);
	static float EaseOutBack(float time, float startPos, float endPos);
	static float EaseInOutBack(float time, float startPos, float endPos);
	static float EaseInElastic(float time, float startPos, float endPos);
	static float EaseOutElastic(float time, float startPos, float endPos);
	static float EaseInOutElastic(float time, float startPos, float endPos);
	static float EaseInBounce(float time, float startPos, float endPos);
	static float EaseOutBounce(float time, float startPos, float endPos);
	static float EaseInOutBounce(float time, float startPos, float endPos);
};

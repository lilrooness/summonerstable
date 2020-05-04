#pragma once

#include <iostream>

// TODO: implement EXPO_IN and EXPO_OUT interpolation functions

enum AnimationFunction {LINEAR, EXPO_IN, EXPO_OUT};

struct FloatAnimation {
	float from, to;
	float durationTicks;
	float started;
	AnimationFunction function;
	bool done{false};
};

FloatAnimation createAnimation(float from, float to, float durationTicks, float currentTime) {
	FloatAnimation  animation;
	animation.from = from;
	animation.started = currentTime;
	animation.durationTicks = durationTicks;
	animation.function = AnimationFunction::LINEAR;

	return animation;
}

float getCurrentAnimationValue(FloatAnimation& animation, float currentTime) {

	if (currentTime - animation.started >= animation.durationTicks) {
		animation.done = true;
		return animation.to;
	}

	float t = (currentTime - animation.started) / animation.durationTicks;

	return animation.from + t * (animation.to - animation.from);
}
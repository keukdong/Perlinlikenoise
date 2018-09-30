#pragma once
template<typename T>
T linearInterpolation(T &leftSample, T &rightSample, T blend) {
	return (1.0 - blend)*leftSample + blend * rightSample;
}
template<typename T>
T SmoothStepInterpolation(T &leftSample, T &rightSample, T blend) {
	T smoothBlend = blend * blend * (3 - 2 * blend);
	return(1.0 - smoothBlend)*leftSample + smoothBlend * rightSample;
}

template<typename T>
T getInterpolation(T(*pFunc)(T &, T &, T), T &leftsample,
	T &rightsample, T blend) {
	return pFunc(leftsample, rightsample, blend);
}

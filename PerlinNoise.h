#pragma once
#include "stdafx.h"
#include "Interpolation.h"

template <typename T>
class CPerlinNoise
{
private:
	T * PerlinNoise1D;
	T **PerlinNoise2D;
	T *seedNoise1D;
	T **seedNoise2D;
private:
	int i1Dresolution;
	int i2Dresolution_x;
	int i2Dresolution_y;
	int moctaves;
public:
	CPerlinNoise() {
		PerlinNoise1D = nullptr;
		PerlinNoise2D = nullptr;
	}
	~CPerlinNoise() {
		if (PerlinNoise1D != nullptr) {
			delete[] PerlinNoise1D;
			delete[] seedNoise1D;
		}
		if (!PerlinNoise2D) {
			for (int row = 0; row < i2Dresolution_x; row++) {
				delete[] PerlinNoise2D[row];
			}
			for (int row = 0; row < i2Dresolution_x; row++) {
				delete[] seedNoise2D[row];
			}
			delete[] PerlinNoise2D;
			delete[] seedNoise2D;			
		}
	}
	void Init1Dperlin(int resolution);
	void Init2Dperlin(int x_resol, int y_resol);

public:
	void setOctaves(int octaves) {
		moctaves = octaves;
	}
public:
	void PerlinNoiselike1D();
	int getNoiseofX1D(int x) const {
		if (x < 0 || x >= i1Dresolution)
			assert("false");
		return PerlinNoise1D[x];
	}
	int getResolof1D() const {
		return i1Dresolution;
	}
	const T findmax1D() const;
	const T findmin1D() const;

	template <typename T>
	friend int heightColor1D(const CPerlinNoise<T>& perlin,int currX);

public:
	void PerlinNoiselike2D();

	int getResolof2D() const {
		return i2Dresolution_x;;
	}

	const T findmax2D() const;
	const T findmin2D() const;

	template <typename T>
	friend int heightColor2D(const CPerlinNoise<T>& perlin, int currX, int currY);

public:
	void drawPerlin();
};

template <typename T>
void CPerlinNoise<T>::Init1Dperlin(int resolution) {
	srand(time(0));
	PerlinNoise1D = new T[resolution];
	seedNoise1D = new T[resolution];
	
	//initial seednoise
	for (int i = 0; i < resolution; i++) {
		seedNoise1D[i] = (T)rand() / RAND_MAX; // 0~1
		PerlinNoise1D[i] = 0;
	}
	i1Dresolution = resolution;
}

template <typename T>
void CPerlinNoise<T>::Init2Dperlin(int x_resol, int y_resol) {
	srand(time(0));
	PerlinNoise2D = new T*[y_resol];
	for (int row = 0; row < y_resol; row++) {
		PerlinNoise2D[row] = new T[x_resol];
	}
	seedNoise2D = new T*[y_resol];;
	for (int row = 0; row < y_resol; row++) {
		seedNoise2D[row] = new T[x_resol];
	}
	i2Dresolution_x = x_resol;
	i2Dresolution_y = y_resol;
	//initial seednoise
	for (int i = 0; i < y_resol; i++) {
		for (int j = 0; j < x_resol; j++) {
			seedNoise2D[i][j] = (T)rand() / RAND_MAX;
			PerlinNoise2D[i][j] = 0;
		}
	}
}

template <typename T>
void CPerlinNoise<T>::PerlinNoiselike1D() {
	if (moctaves == 0) return;
	int resolution = i1Dresolution;
	int octaves = moctaves;

	for (int x = 0; x < resolution; x++) {
		T xNoise = 0.0f;
		T scale = 1;
		T scaleAcc = 0;

		// if resoulution = 256
		// CurOctave = 0 pitch = 256 / CurOctave = 1 pitch = 128
		for (int CurOctave = 0; CurOctave < octaves; CurOctave++) {
			int pitch = resolution >> CurOctave;
			if (pitch == 0)
				assert("false");

			int leftSample = (x / pitch) * pitch;
			int rightSample = (leftSample + pitch) % resolution;

			T blend = (T)(x - leftSample) / (T)pitch;

			xNoise += getInterpolation(SmoothStepInterpolation,
				seedNoise1D[leftSample], seedNoise1D[rightSample], blend)*scale;
			scaleAcc += scale;
			scale /= 2;
		}
		PerlinNoise1D[x] = xNoise / scaleAcc;
	}
}

template<typename T>
void CPerlinNoise<T>::PerlinNoiselike2D()
{
	if (moctaves == 0) return;
	int xRes = i2Dresolution_x;
	int yRes = i2Dresolution_y;
	int octaves = moctaves;

	for (int i = 0; i < yRes; i++) {
		for (int j = 0; j < xRes; j++) {
			//seedNoise[i][j]

			T pointInterpolation = 0.0f;
			T scale = 1;
			T scaleAcc = 0;

			for (int CurOctave = 0; CurOctave < octaves; CurOctave++) {
				int xpitch = xRes >> CurOctave;
				int ypitch = yRes >> CurOctave;

				if (!xpitch || !ypitch)
					assert("false");
				//¡á¡à¡à¡à¡á ...1 
				//¡à¢Ã¡à¡à¡à ...2 
				//¡à¡à¡à¡à¡à
				//¡à¡à¡à¡à¡à
				//¡á¡à¡à¡à¡á ...1 
				
				int xleftSample = (j / xpitch) * xpitch;
				int xrightSample = (xleftSample + xpitch) % i2Dresolution_x;
				int yupSample = (i / ypitch) * ypitch;
				int ydownSample = (yupSample + ypitch) % i2Dresolution_y;;

				T xblend = (T)(j - xleftSample) / (T)xpitch;
				T firstInterpolation = getInterpolation(SmoothStepInterpolation,
					seedNoise2D[xleftSample][yupSample], seedNoise2D[xrightSample][yupSample], xblend)*scale;
				
				T secondInterpolation = getInterpolation(SmoothStepInterpolation,
					seedNoise2D[xleftSample][ydownSample], seedNoise2D[xrightSample][ydownSample], xblend)*scale;
				
				T yblend = (T)(i - yupSample) / (T)ypitch;

				pointInterpolation += getInterpolation(SmoothStepInterpolation,
					firstInterpolation, secondInterpolation, yblend)*scale;
				scaleAcc += scale;
				scale /= 2;
			}
			PerlinNoise2D[i][j] = pointInterpolation / scaleAcc;
		}
	}
}

template<typename T>
const T CPerlinNoise<T>::findmax1D() const {
	//time complexity:o(n)
	T max=0;
	for (int i = 0; i < i1Dresolution; i++) {
		if (PerlinNoise1D[i] >= max) max = PerlinNoise1D[i];
	}
	return max;
}
template<typename T>
const T CPerlinNoise<T>::findmin1D() const {
	//time complexity:o(n)
	T min = 0;
	for (int i = 0; i < i1Dresolution; i++) {
		if (PerlinNoise1D[i] <= min) min = PerlinNoise1D[i];
	}
	return min;
}

template<typename T>
const T CPerlinNoise<T>::findmax2D() const
{
	T max = 0;
	for (int i = 0; i < i2Dresolution_y; i++) {
		for (int j = 0; j < i2Dresolution_x; j++) {
			if (max < PerlinNoise2D[i][j]) max = PerlinNoise2D[i][j];
		}
	}
	return max;
}

template<typename T>
const T CPerlinNoise<T>::findmin2D() const
{
	T min = 0;
	for (int i = 0; i < i2Dresolution_y; i++) {
		for (int j = 0; j < i2Dresolution_x; j++) {
			if (min > PerlinNoise2D[i][j]) min = PerlinNoise2D[i][j];
		}
	}
	return min;
}

template <typename T>
void CPerlinNoise<T>::drawPerlin() {
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

	if (PerlinNoise1D != nullptr) {
		for (int i = 0; i < i1Dresolution; i++) {
			SetConsoleTextAttribute(h, heightColor1D(*this,i));
			std::cout << std::fixed;
			// just for number std::cout.precision(3);
			std::cout.width(1);
			std::cout << "";
		}
		std::cout << std::endl;
	}
	if (PerlinNoise2D) {
		for (int i = 0; i < i2Dresolution_y; i++) {
			for (int j = 0; j < i2Dresolution_x; j++) {
				SetConsoleTextAttribute(h, heightColor2D(*this, i,j));
				std::cout << std::fixed;
				std::cout.width(1);
				std::cout << "  ";
			}
			SetConsoleTextAttribute(h, 240);
			std::cout << std::endl;
		}
	}
}

template <typename T>
int heightColor1D(const CPerlinNoise<T>& perlin,int currX) {
	T max = perlin.findmax1D();
	T min = perlin.findmin1D();
	T res = max - min;
	T temp = perlin.PerlinNoise1D[currX];

	if (temp >= min+(res * 15 / 16)) {
		return 240;
	}
	else if (temp < min+(res * 15 / 16) && temp >= min+(res * 14 / 16)) {
		return 224;
	}
	else if (temp < min+(res * 14 / 16) && temp >= min+(res * 13 / 16)) {
		return 208;
	}
	else if (temp < min+(res * 13 / 16) && temp >= min+(res * 12 / 16)) {
		return 192;
	}
	else if (temp < min+(res * 12 / 16) && temp >= min+(res * 11 / 16)) {
		return 176;
	}
	else if (temp < min+(res * 11 / 16) && temp >= min+(res * 10 / 16)) {
		return 160;
	}
	else if (temp < min+(res * 10 / 16) && temp >= min+(res * 9 / 16)) {
		return 144;
	}
	else if (temp < min+(res * 9 / 16) && temp >= min+(res * 8 / 16)) {
		return 112;
	}
	else if (temp < min+(res * 8 / 16) && temp >= min+(res * 7 / 16)) {
		return 96;
	}
	else if (temp < min+(res * 7 / 16) && temp >= min+(res * 6 / 16)) {
		return 80;
	}
	else if (temp < min+(res * 6 / 16) && temp >= min+(res * 5 / 16)) {
		return 64;
	}
	else if (temp < min+(res * 5 / 16) && temp >= min+(res * 4 / 16)) {
		return 48;
	}
	else if (temp < min+(res * 4 / 16) && temp >= min+(res * 3 / 16)) {
		return 32;
	}
	else if (temp < min+(res * 3 / 16) && temp >= min+(res * 2 / 16)) {
		return 16;
	}
	else if (temp < min+(res * 2 / 16) && temp >= min+(res * 1 / 16)) {
		return 240;
	}
	else {
		return 1;
	}
}
/*
template<typename T>
int heightColor2D(const CPerlinNoise<T>& perlin, int currX, int currY)
{
	T max = perlin.findmax2D();
	T min = perlin.findmin2D();
	T res = max - min;
	T temp = perlin.PerlinNoise2D[currX][currY];

	if (temp >= min + (res * 15 / 16)) {
		return 240;
	}
	else if (temp < min + (res * 15 / 16) && temp >= min + (res * 14 / 16)) {
		return 224;
	}
	else if (temp < min + (res * 14 / 16) && temp >= min + (res * 13 / 16)) {
		return 208;
	}
	else if (temp < min + (res * 13 / 16) && temp >= min + (res * 12 / 16)) {
		return 192;
	}
	else if (temp < min + (res * 12 / 16) && temp >= min + (res * 11 / 16)) {
		return 176;
	}
	else if (temp < min + (res * 11 / 16) && temp >= min + (res * 10 / 16)) {
		return 160;
	}
	else if (temp < min + (res * 10 / 16) && temp >= min + (res * 9 / 16)) {
		return 144;
	}
	else if (temp < min + (res * 9 / 16) && temp >= min + (res * 8 / 16)) {
		return 112;
	}
	else if (temp < min + (res * 8 / 16) && temp >= min + (res * 7 / 16)) {
		return 96;
	}
	else if (temp < min + (res * 7 / 16) && temp >= min + (res * 6 / 16)) {
		return 80;
	}
	else if (temp < min + (res * 6 / 16) && temp >= min + (res * 5 / 16)) {
		return 64;
	}
	else if (temp < min + (res * 5 / 16) && temp >= min + (res * 4 / 16)) {
		return 48;
	}
	else if (temp < min + (res * 4 / 16) && temp >= min + (res * 3 / 16)) {
		return 32;
	}
	else if (temp < min + (res * 3 / 16) && temp >= min + (res * 2 / 16)) {
		return 16;
	}
	else if (temp < min + (res * 2 / 16) && temp >= min + (res * 1 / 16)) {
		return 240;
	}
	else {
		return 1;
	}
}
*/

template<typename T>
int heightColor2D(const CPerlinNoise<T>& perlin, int currX, int currY)
{
	T max = perlin.findmax2D();
	T min = perlin.findmin2D();
	T res = max - min;
	T temp = perlin.PerlinNoise2D[currX][currY];

	if (temp >= min + (res * 31 / 32)) {
		return 16;
	}
	else if (temp < min + (res * 31 / 32) && temp >= min + (res * 30 / 32)) {
		return 240;
	}
	else if (temp < min + (res * 30 / 32) && temp >= min + (res * 29 / 32)) {
		return 224;
	}
	else if (temp < min + (res * 29 / 32) && temp >= min + (res * 28 / 32)) {
		return 208;
	}
	else if (temp < min + (res * 28 / 32) && temp >= min + (res * 27 / 32)) {
		return 192;
	}
	else if (temp < min + (res * 27 / 32) && temp >= min + (res * 26 / 32)) {
		return 176;
	}
	else if (temp < min + (res * 26 / 32) && temp >= min + (res * 25 / 32)) {
		return 160;
	}
	else if (temp < min + (res * 25 / 32) && temp >= min + (res * 24 / 32)) {
		return 144;
	}
	else if (temp < min + (res * 24 / 32) && temp >= min + (res * 23/ 32)) {
		return 112;
	}
	else if (temp < min + (res * 23 / 32) && temp >= min + (res * 22/ 32)) {
		return 96;
	}
	else if (temp < min + (res * 22 / 32) && temp >= min + (res * 21/ 32)) {
		return 80;
	}
	else if (temp < min + (res * 21 / 32) && temp >= min + (res * 20/ 32)) {
		return 64;
	}
	else if (temp < min + (res * 20 / 32) && temp >= min + (res * 19/ 32)) {
		return 48;
	}
	else if (temp < min + (res * 19 / 32) && temp >= min + (res * 18/ 32)) {
		return 32;
	}
	else if (temp < min + (res * 18 / 32) && temp >= min + (res * 17 / 32)) {
		return 16;
	}
	else if (temp < min + (res * 17 / 32) && temp >= min + (res * 16 / 32)) {
		return 240;
	}
	else if (temp < min + (res * 16 / 32) && temp >= min + (res * 15 / 32)) {
		return 240;
	}
	else if (temp < min + (res * 15 / 32) && temp >= min + (res * 14 / 32)) {
		return 224;
	}
	else if (temp < min + (res * 14 / 32) && temp >= min + (res * 13 / 32)) {
		return 208;
	}
	else if (temp < min + (res * 13 / 32) && temp >= min + (res * 12 / 32)) {
		return 192;
	}
	else if (temp < min + (res * 12 / 32) && temp >= min + (res * 11 / 32)) {
		return 176;
	}
	else if (temp < min + (res * 11 / 32) && temp >= min + (res * 10 / 32)) {
		return 160;
	}
	else if (temp < min + (res * 10 / 32) && temp >= min + (res * 9 / 32)) {
		return 144;
	}
	else if (temp < min + (res * 9 / 32) && temp >= min + (res * 8 / 32)) {
		return 112;
	}
	else if (temp < min + (res * 8 / 32) && temp >= min + (res * 7 / 32)) {
		return 96;
	}
	else if (temp < min + (res * 7 / 32) && temp >= min + (res * 6 / 32)) {
		return 80;
	}
	else if (temp < min + (res * 6 / 32) && temp >= min + (res * 5 / 32)) {
		return 64;
	}
	else if (temp < min + (res * 5 / 32) && temp >= min + (res * 4 / 32)) {
		return 48;
	}
	else if (temp < min + (res * 4 / 32) && temp >= min + (res * 3 / 32)) {
		return 32;
	}
	else if (temp < min + (res * 3 / 32) && temp >= min + (res * 2 / 32)) {
		return 16;
	}
	else if (temp < min + (res * 2 / 32) && temp >= min + (res * 1 / 32)) {
		return 240;
	}
	else {
		return 1;
	}
}


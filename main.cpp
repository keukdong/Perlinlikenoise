#include "stdafx.cpp"
#include "PerlinNoise.h"
#include <math.h>
int main(void) {
	/*
	CPerlinNoise<float> newPerlin;
	newPerlin.Init1Dperlin(pow(2,10));
	int temp = 0;
	
	for (int i = newPerlin.getResolof1D(); ; temp++) {
		i = i >> 1;
		if (i == 0) break;
	}

	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

	for (int i = 0; i < temp; i++) {
		newPerlin.setOctaves(i);
		newPerlin.PerlinNoiselike1D();
		newPerlin.drawPerlin();

		SetConsoleTextAttribute(h, 240);
		std::cout << i << " : " << temp << std::endl;
		char next;
		std::cin >> next;

		SetConsoleTextAttribute(h, 240);
		system("cls");
	}
	*/
	
	CPerlinNoise<float> newPerlin;
	//newPerlin.Init2Dperlin(pow(2, 5), pow(2, 5));
	newPerlin.Init2Dperlin(44, 44);
	int octave = 0;
	for (int i = newPerlin.getResolof2D(); ; octave++) {
		i = i >> 1;
		if (i == 0) break;
	}
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

	for (int i = 0; i < octave; i++) {
		newPerlin.setOctaves(i);
		newPerlin.PerlinNoiselike2D();
		newPerlin.drawPerlin();

		SetConsoleTextAttribute(h, 240);
		std::cout << i << " : " << octave << std::endl;
		
		char next;
		std::cin >> next;
		SetConsoleTextAttribute(h, 240);
		system("cls");
	}
	
	SetConsoleTextAttribute(h, 240);
	return 0;
}
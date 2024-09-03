#pragma once

#include <vector>

class Simulation
{
private:
	unsigned int gridXSize, gridYSize;

	unsigned int* grid;
	unsigned int* oldGrid;

	unsigned int stepsPerFrame;
	unsigned int ruleIndex;

	std::vector<float> spots;

	float cellWidth, cellHeight;

	unsigned int generation = 0;

public:
	Simulation(unsigned int gridXSize, unsigned int gridYSize, unsigned int stepsPerFrame, unsigned int ruleIndex);

	void simulate();

	void draw();

	~Simulation();
};
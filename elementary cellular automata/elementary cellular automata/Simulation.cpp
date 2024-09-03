#include "Simulation.h"

#include <glew.h>
#include <glfw3.h>
#include <gtc\matrix_transform.hpp>

#include <stdlib.h>
#include <time.h>
#include <iostream>

#include "rules.h"

Simulation::Simulation(unsigned int gridXSize, unsigned int gridYSize, unsigned int stepsPerFrame, unsigned int ruleIndex) 
    : gridXSize(gridXSize), gridYSize(gridYSize), stepsPerFrame(stepsPerFrame), ruleIndex(ruleIndex)
{
	grid = new unsigned int[(long long)gridXSize];
	oldGrid = new unsigned int[(long long)gridXSize];

	srand(time(NULL));

    for (unsigned int i = 0; i < gridXSize; i++)
        grid[i] = 0;

    grid[gridXSize / 2l] = 1;

    spots = std::vector<float>();

    cellWidth =  1.f / gridXSize * 16.f/9.f;
    cellHeight =  1.f / gridYSize;
}

void Simulation::simulate()
{
    for (unsigned int steps = this->stepsPerFrame; steps > 0; steps--)
    {
        if (generation >= gridYSize) return;

        for (unsigned int i = 0; i < gridXSize; i++)
            oldGrid[i] = grid[i];

        for (unsigned int i = 1; i < gridXSize - 1; i++)
        {
            unsigned int index = (oldGrid[i - 1] << 2) + (oldGrid[i] << 1) + (oldGrid[i + 1] << 0);
            grid[i] = (ruleSet[ruleIndex] & (1 << index)) >> index;

            if (grid[i] == 1)
            {
                spots.push_back(16.f/9.f * 2.f * i / gridXSize - 16.f/9.f * 1.f);
                spots.push_back(-2.f * generation / gridYSize + 1.f);
                spots.push_back(1.f);
            }
        }

        generation++;
    }
}

Simulation::~Simulation()
{
	delete[] grid;
	delete[] oldGrid;
}

void Simulation::draw()
{
    const unsigned int bufferSize = 8 * spots.size();

    float* bufferData = new float[bufferSize];

    const unsigned int indicesCount = 6 * spots.size() / 3;
    unsigned int* indices = new unsigned int[indicesCount];

    int index = 0;
    for (int i = 0; i < spots.size()/3; i++)
    {
        int bOffset = 20 * i;
        bufferData[bOffset + 0] = -cellWidth;
        bufferData[bOffset + 1] =  cellHeight;
        bufferData[bOffset + 2] =  spots[index];
        bufferData[bOffset + 3] =  spots[index + 1];
        bufferData[bOffset + 4] =  spots[index + 2];
        
        bufferData[bOffset + 5] = -cellWidth;
        bufferData[bOffset + 6] = -cellHeight;
        bufferData[bOffset + 7] = spots[index];
        bufferData[bOffset + 8] = spots[index + 1];
        bufferData[bOffset + 9] = spots[index + 2];

        bufferData[bOffset + 10] = cellWidth;
        bufferData[bOffset + 11] = -cellHeight;
        bufferData[bOffset + 12] = spots[index];
        bufferData[bOffset + 13] = spots[index + 1];
        bufferData[bOffset + 14] = spots[index + 2];

        bufferData[bOffset + 15] = cellWidth;
        bufferData[bOffset + 16] = cellHeight;
        bufferData[bOffset + 17] = spots[index];
        bufferData[bOffset + 18] = spots[index + 1];
        bufferData[bOffset + 19] = spots[index + 2];

        index += 3;

        int iiOffset = 6 * i;
        int iOffset = 4 * i;
        indices[iiOffset + 0] = 0 + iOffset;
        indices[iiOffset + 1] = 1 + iOffset;
        indices[iiOffset + 2] = 2 + iOffset;
        indices[iiOffset + 3] = 2 + iOffset;
        indices[iiOffset + 4] = 3 + iOffset;
        indices[iiOffset + 5] = 0 + iOffset;
    }
    
    unsigned int VAID, BID, IID;
    glCreateVertexArrays(1, &VAID);
    glBindVertexArray(VAID);

    glGenBuffers(1, &BID);
    glBindBuffer(GL_ARRAY_BUFFER, BID);
    glBufferData(GL_ARRAY_BUFFER, bufferSize * sizeof(float), bufferData, GL_STATIC_DRAW);

    glGenBuffers(1, &IID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(float), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void*)(sizeof(float) * 2));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void*)(sizeof(float) * 4));

    const char* vertexSource =
        "#version 330 core\n"
        "layout(location = 0) in vec2 vertexCoord;"
        "layout(location = 1) in vec2 translation;"
        "layout(location = 2) in float neighboursW;"
        "out vec2 fragPos;"
        "out float neighboursWeight;"
        "uniform mat4 projection;"
        "void main()"
        "{"
        "   gl_Position = projection * vec4(vertexCoord.x + translation.x, vertexCoord.y + translation.y, 1.f, 1.f);"
        "   fragPos = vertexCoord;"
        "   neighboursWeight = neighboursW;"
        "}";

    const char* fragmentSource =
        "#version 330 core\n"
        "layout(location = 0) out vec4 color;"
        "in vec2 fragPos;"
        "in float neighboursWeight;"
        "void main()"
        "{"
        "   color = vec4(neighboursWeight, neighboursWeight * .9f, neighboursWeight * .8f, 1.f);"
        "}";

    unsigned int VID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VID, 1, &vertexSource, nullptr);
    glCompileShader(VID);
    unsigned int FID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FID, 1, &fragmentSource, nullptr);
    glCompileShader(FID);

    unsigned int SID = glCreateProgram();
    glAttachShader(SID, VID);
    glAttachShader(SID, FID);
    glLinkProgram(SID);
    glValidateProgram(SID);

    glDeleteShader(VID);
    glDeleteShader(FID);

    glUseProgram(SID);

    glm::mat4 projection = glm::ortho(-1.f * (16.f/9.f), 1.f * (16.f/9.f), -1.f, 1.f, -1.f, 1.f);
    glUniformMatrix4fv(glGetUniformLocation(SID, "projection"), 1, 0, &projection[0][0]);

    /*******************************/
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, nullptr);
    /*******************************/

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);

    glDeleteVertexArrays(1, &VAID);
    glDeleteBuffers(1, &BID);
    glDeleteBuffers(1, &IID);
    glDeleteProgram(SID);

    delete[] bufferData;
    delete[] indices;
}
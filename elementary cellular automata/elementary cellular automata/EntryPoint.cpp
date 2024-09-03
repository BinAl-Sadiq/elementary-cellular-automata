#define GLEW_STATIC

#include <iostream>

#include <glew.h>
#include <glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

#include "Simulation.h"

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1920, 1080, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    //GLEW need to be defined after context current get defined
    if (glewInit() != GLEW_OK)
        std::cout << "Error GLEW not ok" << std::endl;

    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window, true);
    ImGui::StyleColorsDark();

    int width = 480, ruleIndex = 90, stepsPerFrame = 3;

    Simulation* simulation = new Simulation(width, width * 9.f / 16, stepsPerFrame, ruleIndex);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplGlfwGL3_NewFrame();

        ImGui::SliderInt("Rule index: ", &ruleIndex, 0, 255);
        ImGui::SliderInt("Steps per frame: ", &stepsPerFrame, 1, 24);
        ImGui::SliderInt("Simulation width: ", &width, 48, 3840);
        if (ImGui::Button("Regenerate"))
        {
            delete simulation;
            simulation = new Simulation(width, width * 9.f / 16, stepsPerFrame, ruleIndex);
        }
        if (ImGui::Button("rule 30")) ruleIndex = 30;
        if (ImGui::Button("rule 54")) ruleIndex = 54;
        if (ImGui::Button("rule 60")) ruleIndex = 60;
        if (ImGui::Button("rule 62")) ruleIndex = 62;
        if (ImGui::Button("rule 73")) ruleIndex = 73;
        if (ImGui::Button("rule 75")) ruleIndex = 75;
        if (ImGui::Button("rule 90")) ruleIndex = 90;
        if (ImGui::Button("rule 94")) ruleIndex = 94;
        if (ImGui::Button("rule 102")) ruleIndex = 102;
        if (ImGui::Button("rule 105")) ruleIndex = 105;
        if (ImGui::Button("rule 110")) ruleIndex = 110;
        if (ImGui::Button("rule 122")) ruleIndex = 122;
        if (ImGui::Button("rule 126")) ruleIndex = 126;
        if (ImGui::Button("rule 150")) ruleIndex = 150;
        if (ImGui::Button("rule 158")) ruleIndex = 158;
        if (ImGui::Button("rule 182")) ruleIndex = 182;
        if (ImGui::Button("rule 188")) ruleIndex = 188;
        if (ImGui::Button("rule 190")) ruleIndex = 190;
        if (ImGui::Button("rule 220")) ruleIndex = 220;
        if (ImGui::Button("rule 222")) ruleIndex = 222;
        if (ImGui::Button("rule 250")) ruleIndex = 250;

        simulation->simulate();
        simulation->draw();

        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    delete simulation;

    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
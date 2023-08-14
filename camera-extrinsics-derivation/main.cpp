#include <windows.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stdio.h>

#include "assignment_solution.h"

GLuint vertexBufferID;
GLuint vertexArrayID;
GLuint colorBufferID;

int cameraFlag = 0;

void* WIN32LoadFile(const char *filePath)
{
    LARGE_INTEGER fileSize;
    HANDLE file = CreateFileA(filePath, GENERIC_READ, 0, 0,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, 0);
    void *buffer = 0;
    if (file)
    {
        if (GetFileSizeEx(file, &fileSize))
        {
            buffer = VirtualAlloc(0, fileSize.QuadPart,
                                  MEM_COMMIT|MEM_RESERVE,
                                  PAGE_READWRITE);
            if (buffer)
            {
                DWORD bytesRead;
                DWORD bufferSize = (DWORD)fileSize.QuadPart;
                ReadFile(file, buffer, bufferSize, &bytesRead, 0);
            }
        }
        return buffer;
    }
    else
    {
        return 0;
    }
}

void framebufSizeCallback(GLFWwindow *window, GLint width, GLint height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, 1);
    }
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PWSTR pCmdLine, int nCmdShow)
{
    const char *glsl_version = "#version 450";
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1280, 720, "Cube", NULL, NULL);

    if (window == NULL) {
        printf("failed to create window\n");
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufSizeCallback);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("failed to initialize GLAD\n");
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);

    int success;
    char infoLog[512];

    char *vShaderCode = (char *)WIN32LoadFile("shader.vert");
    GLuint vShaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShaderID, 1, &vShaderCode, NULL);
    glCompileShader(vShaderID);
    glGetShaderiv(vShaderID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vShaderID, 512, NULL, infoLog);
        printf("%s\n", infoLog);
    }

    char *fShaderCode = (char *)WIN32LoadFile("shader.frag");
    GLuint fShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShaderID, 1, &fShaderCode, NULL);
    glCompileShader(fShaderID);
    glGetShaderiv(fShaderID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fShaderID, 512, NULL, infoLog);
        printf("%s\n", infoLog);
    }

    GLuint shaderProgID = glCreateProgram();
    glAttachShader(shaderProgID, vShaderID);
    glAttachShader(shaderProgID, fShaderID);
    glLinkProgram(shaderProgID);
    glGetProgramiv(shaderProgID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgID, 512, NULL, infoLog);
        printf("%s\n", infoLog);
    }
    glDeleteShader(vShaderID);
    glDeleteShader(fShaderID);

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
    };

    static const GLfloat colors[] = {
        0.583f,  0.771f,  0.014f,
        0.609f,  0.115f,  0.436f,
        0.327f,  0.483f,  0.844f,
        0.822f,  0.569f,  0.201f,
        0.435f,  0.602f,  0.223f,
        0.310f,  0.747f,  0.185f,
        0.597f,  0.770f,  0.761f,
        0.559f,  0.436f,  0.730f,
        0.359f,  0.583f,  0.152f,
        0.483f,  0.596f,  0.789f,
        0.559f,  0.861f,  0.639f,
        0.195f,  0.548f,  0.859f,
        0.014f,  0.184f,  0.576f,
        0.771f,  0.328f,  0.970f,
        0.406f,  0.615f,  0.116f,
        0.676f,  0.977f,  0.133f,
        0.971f,  0.572f,  0.833f,
        0.140f,  0.616f,  0.489f,
        0.997f,  0.513f,  0.064f,
        0.945f,  0.719f,  0.592f,
        0.543f,  0.021f,  0.978f,
        0.279f,  0.317f,  0.505f,
        0.167f,  0.620f,  0.077f,
        0.347f,  0.857f,  0.137f,
        0.055f,  0.953f,  0.042f,
        0.714f,  0.505f,  0.345f,
        0.783f,  0.290f,  0.734f,
        0.722f,  0.645f,  0.174f,
        0.302f,  0.455f,  0.848f,
        0.225f,  0.587f,  0.040f,
        0.517f,  0.713f,  0.338f,
        0.053f,  0.959f,  0.120f,
        0.393f,  0.621f,  0.362f,
        0.673f,  0.211f,  0.457f,
        0.820f,  0.883f,  0.371f,
        0.982f,  0.099f,  0.879f
    };

    glGenVertexArrays(1, &vertexArrayID);
    glGenBuffers(1, &vertexBufferID);
    glGenBuffers(1, &colorBufferID);

    glBindVertexArray(vertexArrayID);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
                 vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors),
                 colors, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    Camera camera_1;
    initializeCamera(vec3{0.0f, 1.0f,  4.0f}, vec3{0.0f, 0.0f, -1.0f},
                     vec3{0.0f, 1.0f,  0.0f}, &camera_1);

    Camera camera_2;
    initializeCamera(vec3{0.0f, 0.0f,  3.0f}, vec3{0.0f, 0.0f, -1.0f},
                     vec3{0.0f, 1.0f,  0.0f}, &camera_2);

    while(!glfwWindowShouldClose(window)) {
        processInput(window);

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        {

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            /* IMGUI_DEMO_MARKER("Widgets/Basic/RadioButton"); */
            /* static int e = 0; */
            ImGui::SeparatorText("Camera");
            ImGui::RadioButton("camera 1", &cameraFlag, 0);
            ImGui::RadioButton("camera 2", &cameraFlag, 1);

            ImGui::SeparatorText("Camera Position");
            if (cameraFlag == 0)
            {
                ImGui::InputFloat("input X Coordinate", &camera_1.position[0], 0.5f, 1.0f, "%.3f");
                ImGui::InputFloat("input Y Coordinate", &camera_1.position[1], 0.5f, 1.0f, "%.3f");
                ImGui::InputFloat("input Z Coordinate", &camera_1.position[2], 0.5f, 1.0f, "%.3f");
            }
            else
            {
                ImGui::InputFloat("input X Coordinate", &camera_2.position[0], 0.5f, 1.0f, "%.3f");
                ImGui::InputFloat("input Y Coordinate", &camera_2.position[1], 0.5f, 1.0f, "%.3f");
                ImGui::InputFloat("input Z Coordinate", &camera_2.position[2], 0.5f, 1.0f, "%.3f");
            }

            ImGui::SeparatorText("Camera Rotation");
            if (cameraFlag == 0)
            {
                ImGui::SliderFloat("input horizontal angle", &camera_1.hAngle, 0, 360, "%.1f");
                ImGui::SliderFloat("input vertial angle", &camera_1.vAngle, -90, 90, "%.1f");
            }
            else
            {
                ImGui::SliderFloat("input horizontal angle", &camera_2.hAngle, 0, 360, "%.1f");
                ImGui::SliderFloat("input vertial angle", &camera_2.vAngle, -90, 90, "%.1f");
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glClearColor(0.2f, 0.3f, 0.6f, 1.0f);

            glUseProgram(shaderProgID);

            if (cameraFlag == 0)
            {
                mat4 view;

                cameraUpdate(&camera_1);
                generateViewMatrix(&camera_1, view);

                GLint viewLoc = glGetUniformLocation(shaderProgID, "view");
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float *)view);
            }

            if (cameraFlag == 1)
            {
                mat4 view;

                cameraUpdate(&camera_2);
                generateViewMatrix(&camera_2, view);

                GLint viewLoc = glGetUniformLocation(shaderProgID, "view");
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float *)view);
            }

            relativeOrientation(&camera_1, &camera_2);

            ImGui::SeparatorText("Camera Relative to Each other");
            ImGui::Text("position of camera 2 wrt 1 x: %f y: %f z: %f",
                        (camera_2.position[0] - camera_1.position[0]),
                        (camera_2.position[1] - camera_1.position[1]),
                        (camera_2.position[2] - camera_1.position[2]));
            ImGui::Text("position of camera 1 wrt 2 x: %f y: %f z: %f",
                        (camera_1.position[0] - camera_2.position[0]),
                        (camera_1.position[1] - camera_2.position[1]),
                        (camera_1.position[2] - camera_2.position[2]));
            ImGui::Text("orientation between camers U: %f V: %f V: %f",
                        glm_deg(relHAngle), glm_deg(relVAngle), glm_deg(relLookAtAngle));

            ImGui::Render();


            mat4 projection;
            glm_mat4_identity(projection);
            glm_perspective(glm_rad(55.0f), 16.0f / 9.0f,
                            0.1f, 100.0f, projection);
            GLint projectionLoc = glGetUniformLocation(shaderProgID,
                                                       "projection");
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE,
                    (float *)projection);

            mat4 model;
            glm_mat4_identity(model);
            GLint modelLoc = glGetUniformLocation(shaderProgID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float *)model);

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glBindVertexArray(vertexArrayID);
            glDrawArrays(GL_TRIANGLES, 0, 36);

        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}

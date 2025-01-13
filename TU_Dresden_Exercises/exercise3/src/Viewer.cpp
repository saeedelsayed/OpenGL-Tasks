// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved

#include "Viewer.h"

#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <nanogui/checkbox.h>

#include <gui/SliderHelper.h>

#include <iostream>

#include <OpenMesh/Core/IO/MeshIO.hh>

#include "glsl.h"

void insertFaceIntoCombinedData(std::vector<GLfloat>& combined_data,
    const std::vector<GLfloat>& v1,
    const std::vector<GLfloat>& v2,
    const std::vector<GLfloat>& v3) {
    combined_data.insert(combined_data.end(), v1.begin(), v1.end());
    combined_data.insert(combined_data.end(), v2.begin(), v2.end());
    combined_data.insert(combined_data.end(), v3.begin(), v3.end());
}

Viewer::Viewer()
    : AbstractViewer("CG1 Exercise 3"), vertex_shader_id(0), fragment_shader_id(0), program_id(0),
    framebuffer_id(0), color_texture_id(0), bloomShaderProgram(0), bloomEnabled(false)
{
    SetupGUI();

    CreateShaders();
    CreateVertexBuffers();

    // Initialize framebuffer and texture for post-processing
    glGenFramebuffers(1, &framebuffer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);

    glGenTextures(1, &color_texture_id);
    glBindTexture(GL_TEXTURE_2D, color_texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture_id, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    modelViewMatrix.setIdentity();
    projectionMatrix.setIdentity();

    camera().FocusOnBBox(nse::math::BoundingBox<float, 3>(Eigen::Vector3f(-1, -1, -1), Eigen::Vector3f(1, 1, 1)));
}

void Viewer::SetupGUI()
{
    auto mainWindow = SetupMainWindow();

    //Create GUI elements for the various options
    chkHasDepthTesting = new nanogui::CheckBox(mainWindow, "Perform Depth Testing");
    chkHasDepthTesting->setChecked(true);

    chkHasFaceCulling = new nanogui::CheckBox(mainWindow, "Perform backface Culling");
    chkHasFaceCulling->setChecked(true);

    sldJuliaCX = nse::gui::AddLabeledSliderWithDefaultDisplay(mainWindow, "JuliaC.X", std::make_pair(-1.0f, 1.0f), 0.45f, 2);
    sldJuliaCY = nse::gui::AddLabeledSliderWithDefaultDisplay(mainWindow, "JuliaC.Y", std::make_pair(-1.0f, 1.0f), -0.3f, 2);
    sldJuliaZoom = nse::gui::AddLabeledSliderWithDefaultDisplay(mainWindow, "Julia Zoom", std::make_pair(0.01f, 10.0f), 1.0f, 2);

    performLayout();
}

void Viewer::CreateVertexBuffers()
{
    // Define tetrahedron positions and colors
    std::vector<GLfloat> v1 = { -1, -1, 1, 1 };
    std::vector<GLfloat> v2 = { 1, 1, 1, 1 };
    std::vector<GLfloat> v3 = { -1, 1, -1, 1 };
    std::vector<GLfloat> v4 = { 1, -1, -1, 1 };

    std::vector<GLfloat> red = { 1, 0, 0, 1 };
    std::vector<GLfloat> green = { 0, 1, 0, 1 };
    std::vector<GLfloat> blue = { 0, 0, 1, 1 };

    std::vector<GLfloat> tetrahedron_pos_vec;
    std::vector<GLfloat> tetrahedron_col_vec;

    // Define faces
    insertFaceIntoCombinedData(tetrahedron_pos_vec, v1, v2, v3);
    insertFaceIntoCombinedData(tetrahedron_col_vec, red, red, red);
    insertFaceIntoCombinedData(tetrahedron_pos_vec, v1, v4, v2);
    insertFaceIntoCombinedData(tetrahedron_col_vec, green, green, green);
    insertFaceIntoCombinedData(tetrahedron_pos_vec, v3, v2, v4);
    insertFaceIntoCombinedData(tetrahedron_col_vec, blue, blue, blue);
    insertFaceIntoCombinedData(tetrahedron_pos_vec, v1, v3, v4);
    insertFaceIntoCombinedData(tetrahedron_col_vec, red, green, blue);

    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    // Position buffer
    glGenBuffers(1, &position_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, tetrahedron_pos_vec.size() * sizeof(GLfloat), &tetrahedron_pos_vec[0], GL_STATIC_DRAW);
    GLuint vid = glGetAttribLocation(program_id, "in_position");
    glEnableVertexAttribArray(vid);
    glVertexAttribPointer(vid, 4, GL_FLOAT, GL_FALSE, 0, 0);

    // Color buffer
    glGenBuffers(1, &color_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, tetrahedron_col_vec.size() * sizeof(GLfloat), &tetrahedron_col_vec[0], GL_STATIC_DRAW);
    GLuint cid = glGetAttribLocation(program_id, "in_color");
    glEnableVertexAttribArray(cid);
    glVertexAttribPointer(cid, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(0);
}

void Viewer::CreateShaders()
{
    std::string vs((char*)shader_vert, shader_vert_size);
    const char* vertex_content = vs.c_str();

    std::string fs((char*)shader_frag, shader_frag_size);
    const char* fragment_content = fs.c_str();

    // Vertex shader
    vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_id, 1, &vertex_content, 0);
    glCompileShader(vertex_shader_id);
    //CheckShaderCompileStatus(vertex_shader_id, "Vertex shader");

    // Fragment shader
    fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader_id, 1, &fragment_content, 0);
    glCompileShader(fragment_shader_id);
    //CheckShaderCompileStatus(fragment_shader_id, "Fragment shader");

    // Program
    program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);
}

void Viewer::drawContents()
{
    Eigen::Vector2f juliaC(sldJuliaCX->value(), sldJuliaCY->value());
    float juliaZoom = sldJuliaZoom->value();

    camera().ComputeCameraMatrices(modelViewMatrix, projectionMatrix);

    if (chkHasFaceCulling->checked())
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);

    if (chkHasDepthTesting->checked())
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    // Render to framebuffer if bloom is enabled
    if (bloomEnabled) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    glUseProgram(program_id);

    GLint modelViewMatrixId = glGetUniformLocation(program_id, "modelViewMatrix");
    GLint projectionMatrixId = glGetUniformLocation(program_id, "projectionMatrix");

    glUniformMatrix4fv(modelViewMatrixId, 1, GL_FALSE, modelViewMatrix.data());
    glUniformMatrix4fv(projectionMatrixId, 1, GL_FALSE, projectionMatrix.data());

    glBindVertexArray(vertex_array_id);
    glDrawArrays(GL_TRIANGLES, 0, 12);
    glBindVertexArray(0);
    glUseProgram(0);

    if (bloomEnabled) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Render the bloom effect
        glUseProgram(bloomShaderProgram);

        GLint bloomEnabledId = glGetUniformLocation(bloomShaderProgram, "bloomEnabled");
        glUniform1i(bloomEnabledId, bloomEnabled);

        glBindTexture(GL_TEXTURE_2D, color_texture_id);
        glDrawArrays(GL_TRIANGLES, 0, 6); // Render fullscreen quad

        glUseProgram(0);
    }
}

void Viewer::keyCallback(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_B && action == GLFW_PRESS) {
        bloomEnabled = true;
    }
    if (key == GLFW_KEY_N && action == GLFW_PRESS) {
        bloomEnabled = false;
    }
}

#pragma once

#include <X11/Xlib.h>
#include <GL/glx.h>

#include <bits/unique_ptr.h>
#include <vector>

#include "math.h"
#include "global.h"

using std::unique_ptr;
using std::make_unique;


class renderer
{
    global_database* global = nullptr;

    Display* rx_display = nullptr;
    Window vx_window = {};
    int vx_connection_number = -1;
    char vx_connection_string[20] = {};
    XVisualInfo* vx_buffer_config = nullptr;
    XSetWindowAttributes vx_window_attributes = {};
    std::vector<Atom> vx_window_protocols = {};
    Atom vx_wm_delete_window = 0;
    Atom wm_state = 0;
    Atom wm_fullscreen = 0;
    // GL X extensions
    GLXContext vglx_context = nullptr;
    GLXFBConfig* vglx_fbconfigurations = nullptr;
    GLXFBConfig vglx_fbselection = nullptr;
    const char* vglx_extensions_string = nullptr;
    int vglx_fb_count = -1;
    int vglx_major = -1;
    int bglx_minor = -1;

    unsigned int vbo_actives[10] = {};
    unsigned int vbo = 0;
    unsigned int vao[10] = {};

    const char* shader_fragment_source = "#version 330 core \n"
        "out vec4 frag_color; \n"
        "void main() \n"
        "{ \n"
        "    frag_color = vec4(1.f, 0.5f, 0.2f, 1.f); \n"
        "}\0";
    const char* shader_vertex_source = "#version 330 core \n"
        "layout (location = 0) in vec3 pix; \n"
        "void main() \n"
        "{ \n"
        "    gl_Position = vec4(pix.x, pix.y, pix.z, 1.f); \n"
        "}\0";
    unsigned int shader_vertex = 0;
    unsigned int shader_fragment = 0;
    unsigned int shader_program = 0;
    char shader_info_log[512] = {};
    char shader_fragment_log[512] = {};
    int shader_compiled = false;
    int shader_link_sucess = 0;

    unique_ptr<float4[]> mbuffer = make_unique<float4[]>( 1920*1080 );
    float mtest_triangle[9] =
    {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };

    int progress_x = 0;
    int progress_y = 0;

public:
    float4 mtriangle_color = { .2f, .9f, .2f, 1.f };
    float4 mrectangle_color = {.5f, .1f, .5f, 1.f};
    float4 mcircle_color = { .2f, .9f, .2f, 1.f };
    float4 msignfield_color = {1.f/255*98.f, 1.f/255*42.f, 1.f/255*125.f, 1.f};
    float4 gradient_approximation[10000] = {};
    int buffer_damage_size = 1920*1080;

    CONSTRUCTOR renderer();
    GLXContext get_gl_context() const;
    bool draw_test_triangle(float4 p_color);
    bool draw_test_circle(float4 p_color);
    bool draw_test_rectangle(float4 p_color);
    bool draw_test_signfield(float4 p_color);
    bool refresh();
    DESTRUCTOR virtual ~renderer();
};

#include <GL/glew.h>

#include "OpenCLUtil.h"
#define __CL_ENABLE_EXCEPTIONS
#include "cl.hpp"

#ifdef OS_WIN
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif

#ifdef OS_LNX
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "OpenGLUtil.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;
using namespace cl;

typedef unsigned int uint;

static int wind_width = 720;
static int wind_height= 720;
static int gJuliaSetIndex = 0;

static const float matrix[16] =
{
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

static const float vertices[12] =
{
    -1.0f,-1.0f, 0.0,
     1.0f,-1.0f, 0.0,
     1.0f, 1.0f, 0.0,
    -1.0f, 1.0f, 0.0
};

static const float texcords[8] =
{
    0.0, 1.0,
    1.0, 1.0,
    1.0, 0.0,
    0.0, 0.0
};

static const uint indices[6] = {0,1,2,0,2,3};

typedef struct {
    Device d;
    CommandQueue q;
    Program p;
    Kernel k;
    ImageGL tex;
    cl::size_t<3> dims;
} process_params;

typedef struct {
    GLuint prg;
    GLuint vao;
    GLuint tex;
} render_params;

process_params params;
render_params rparams;

static void glfw_error_callback(int error, const char* desc)
{
    fputs(desc,stderr);
}

static void glfw_key_callback(GLFWwindow* wind, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(wind, GL_TRUE);
        else if (key == GLFW_KEY_1)
            gJuliaSetIndex = 0;
        else if (key == GLFW_KEY_2)
            gJuliaSetIndex = 1;
        else if (key == GLFW_KEY_3)
            gJuliaSetIndex = 2;
        else if (key == GLFW_KEY_4)
            gJuliaSetIndex = 3;
        else if (key == GLFW_KEY_5)
            gJuliaSetIndex = 4;
        else if (key == GLFW_KEY_6)
            gJuliaSetIndex = 5;
        else if (key == GLFW_KEY_7)
            gJuliaSetIndex = 6;
        else if (key == GLFW_KEY_8)
            gJuliaSetIndex = 7;
        else if (key == GLFW_KEY_9)
            gJuliaSetIndex = 8;
        else if (key == GLFW_KEY_Q)
            gJuliaSetIndex = 9;
    }
}

static void glfw_framebuffer_size_callback(GLFWwindow* wind, int width, int height)
{
    glViewport(0,0,width,height);
}

void processTimeStep(void);
void renderFrame(void);

int main()
{
    if (!glfwInit())
        return 255;

          GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode    = glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_RED_BITS    , mode->redBits    );
    glfwWindowHint(GLFW_GREEN_BITS  , mode->greenBits  );
    glfwWindowHint(GLFW_BLUE_BITS   , mode->blueBits   );
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    wind_width  = mode->width;
    wind_height = mode->height;

    GLFWwindow* window;

    glfwSetErrorCallback(glfw_error_callback);

    window = glfwCreateWindow(wind_width,wind_height,"Julia Sets",monitor,NULL);
    if (!window) {
        glfwTerminate();
        return 254;
    }

    glfwMakeContextCurrent(window);
    GLenum res = glewInit();
    if (res!=GLEW_OK) {
        std::cout<<"Error Initializing GLEW | Exiting"<<std::endl;
        return 253;
    }

    cl_int errCode;
    try {
        Platform lPlatform = getPlatform();
        // Select the default platform and create a context using this platform and the GPU
#ifdef OS_LNX
        cl_context_properties cps[] = {
            CL_GL_CONTEXT_KHR, (cl_context_properties)glfwGetGLXContext(window),
            CL_GLX_DISPLAY_KHR, (cl_context_properties)glfwGetX11Display(),
            CL_CONTEXT_PLATFORM, (cl_context_properties)lPlatform(),
            0
        };
#endif
#ifdef OS_WIN
        cl_context_properties cps[] = {
            CL_GL_CONTEXT_KHR, (cl_context_properties)glfwGetWGLContext(window),
            CL_WGL_HDC_KHR, (cl_context_properties)GetDC(glfwGetWin32Window(window)),
            CL_CONTEXT_PLATFORM, (cl_context_properties)lPlatform(),
            0
        };
#endif
        std::vector<Device> devices;
        lPlatform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
        // Get a list of devices on this platform
        for (unsigned d=0; d<devices.size(); ++d) {
            if (checkExtnAvailability(devices[d],CL_GL_SHARING_EXT)) {
                params.d = devices[d];
                break;
            }
        }
        Context context(params.d, cps);
        // Create a command queue and use the first device
        params.q = CommandQueue(context, params.d);
        params.p = getProgram(context, ASSETS_DIR "/julia.cl",errCode);

        std::ostringstream options;
        options << "-I " << std::string(ASSETS_DIR);

        params.p.build(std::vector<Device>(1, params.d), options.str().c_str());
        params.k = Kernel(params.p, "julia");
        // create opengl stuff
        rparams.prg = initShaders(ASSETS_DIR "/julia.vert", ASSETS_DIR "/julia.frag");
        rparams.tex = createTexture2D(wind_width,wind_height);
        GLuint vbo  = createBuffer(12,vertices,GL_STATIC_DRAW);
        GLuint tbo  = createBuffer(8,texcords,GL_STATIC_DRAW);
        GLuint ibo;
        glGenBuffers(1,&ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(uint)*6,indices,GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
        // bind vao
        glGenVertexArrays(1,&rparams.vao);
        glBindVertexArray(rparams.vao);
        // attach vbo
        glBindBuffer(GL_ARRAY_BUFFER,vbo);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
        glEnableVertexAttribArray(0);
        // attach tbo
        glBindBuffer(GL_ARRAY_BUFFER,tbo);
        glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,NULL);
        glEnableVertexAttribArray(1);
        // attach ibo
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibo);
        glBindVertexArray(0);
        // create opengl texture reference using opengl texture
        params.tex = ImageGL(context,CL_MEM_READ_WRITE,GL_TEXTURE_2D,0,rparams.tex,&errCode);
        if (errCode!=CL_SUCCESS) {
            std::cout<<"Failed to create OpenGL texture refrence: "<<errCode<<std::endl;
            return 250;
        }
        params.dims[0] = wind_width;
        params.dims[1] = wind_height;
        params.dims[2] = 1;
    } catch(Error error) {
        std::cout << error.what() << "(" << error.err() << ")" << std::endl;
        std::string val = params.p.getBuildInfo<CL_PROGRAM_BUILD_LOG>(params.d);
        std::cout<<"Log:\n"<<val<<std::endl;
        return 249;
    }

    glfwSetKeyCallback(window,glfw_key_callback);
    glfwSetFramebufferSizeCallback(window,glfw_framebuffer_size_callback);

    while (!glfwWindowShouldClose(window)) {
        // process call
        processTimeStep();
        // render call
        renderFrame();
        // swap front and back buffers
        glfwSwapBuffers(window);
        // poll for events
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}

inline unsigned divup(unsigned a, unsigned b)
{
    return (a+b-1)/b;
}

void processTimeStep()
{
    cl::Event ev;
    try {
        glFinish();

        std::vector<Memory> objs;
        objs.clear();
        objs.push_back(params.tex);
        // flush opengl commands and wait for object acquisition
        cl_int res = params.q.enqueueAcquireGLObjects(&objs,NULL,&ev);
        ev.wait();
        if (res!=CL_SUCCESS) {
            std::cout<<"Failed acquiring GL object: "<<res<<std::endl;
            exit(248);
        }
        NDRange local(16, 16);
        NDRange global( local[0] * divup(params.dims[0], local[0]),
                        local[1] * divup(params.dims[1], local[1]));
        // set kernel arguments
        params.k.setArg(0,params.tex);
        params.k.setArg(1,(int)params.dims[0]);
        params.k.setArg(2,(int)params.dims[1]);
        params.k.setArg(3,gJuliaSetIndex);
        params.k.setArg(4,0.9f);
        params.k.setArg(5,0);
        params.k.setArg(6,0);
        params.q.enqueueNDRangeKernel(params.k,cl::NullRange, global, local);
        // release opengl object
        res = params.q.enqueueReleaseGLObjects(&objs);
        ev.wait();
        if (res!=CL_SUCCESS) {
            std::cout<<"Failed releasing GL object: "<<res<<std::endl;
            exit(247);
        }
        params.q.finish();
    } catch(Error err) {
        std::cout << err.what() << "(" << err.err() << ")" << std::endl;
    }
}

void renderFrame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2,0.2,0.2,0.0);
    glEnable(GL_DEPTH_TEST);
    // bind shader
    glUseProgram(rparams.prg);
    // get uniform locations
    int mat_loc = glGetUniformLocation(rparams.prg,"matrix");
    int tex_loc = glGetUniformLocation(rparams.prg,"tex");
    // bind texture
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(tex_loc,0);
    glBindTexture(GL_TEXTURE_2D,rparams.tex);
    glGenerateMipmap(GL_TEXTURE_2D);
    // set project matrix
    glUniformMatrix4fv(mat_loc,1,GL_FALSE,matrix);
    // now render stuff
    glBindVertexArray(rparams.vao);
    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
    glBindVertexArray(0);
}

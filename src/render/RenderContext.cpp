//
// Copyright (c) 2021 Julian Hinxlage. All rights reserved.
//

#include "RenderContext.h"
#include "common/Log.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace baseline;

static void glfwErrorCallback(int error, const char* description) {
    Log::error("GLFW code %d: %s", error, description);
}

namespace baseline {

    thread_local void *context = nullptr;

    void *RenderContext::create() {
        //init glfw
        glfwSetErrorCallback(glfwErrorCallback);
        static bool glfwInited = false;
        if(!glfwInited){
            if(glfwInit() != GLFW_TRUE) {
                Log::error("failed to initialize GLFW");
                return nullptr;
            }
            glfwInited = true;
        }

        //create context
        GLFWwindow *c = glfwCreateWindow(1, 1, "", nullptr, (GLFWwindow*)context);
        if(!c){
            Log::error("failed to create GLFW context");
            return nullptr;
        }
        set(c);

        Log::info("OpenGL version: %s", glGetString(GL_VERSION));
        Log::info("GLSL version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
        Log::info("GPU vendor: %s", glGetString(GL_VENDOR));
        Log::info("GPU: %s", glGetString(GL_RENDERER));
        return c;
    }

    void *RenderContext::get() {
        return context;
    }

    void RenderContext::set(void *c) {
        static bool glewInited = false;
        if (!glewInited) {
            if (glewInit() != GLEW_OK) {
                Log::error("failed to initialize GLEW");
            }
            glewInited = true;
        }

        if(c != context){
            glfwMakeContextCurrent((GLFWwindow*)c);
            context = c;
        }
    }

    void RenderContext::destroy() {
        if(context != nullptr){
            glfwDestroyWindow((GLFWwindow*)context);
            set(nullptr);
        }
    }

    void RenderContext::setDepth(bool enabled) {
        if(enabled){
            glEnable(GL_DEPTH_TEST);
        }else{
            glDisable(GL_DEPTH_TEST);
        }
    }

    void RenderContext::setBlend(bool enabled) {
        if(enabled){
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }else{
            glDisable(GL_BLEND);
        }
    }

    void RenderContext::setCull(bool enabled, bool front) {
        if(enabled){
            glEnable(GL_CULL_FACE);
            glCullFace(front ? GL_FRONT : GL_BACK);
        }else{
            glDisable(GL_CULL_FACE);
        }
    }

    void RenderContext::flush(bool synchronous) {
        if(synchronous){
            glFinish();
        }else{
            glFlush();
        }
    }

}

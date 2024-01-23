//
// Copyright (c) 2021 Julian Hinxlage. All rights reserved.
//

#pragma once

namespace baseline {

    class RenderContext {
    public:
        static void* create();
        static void *get();
        static void set(void *context);
        static void destroy();

        static void setDepth(bool enabled);
        static void setBlend(bool enabled);
        static void setCull(bool enabled, bool front = false);
        static void flush(bool synchronous = false);
    };

}


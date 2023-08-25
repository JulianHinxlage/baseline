//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#pragma once

#include <stdint.h>

namespace baseline {

    class Clock {
    public:
        Clock();
        void reset();
        double elapsed();
        double round();
        static double now();
        static void sleep(double seconds);
    private:
        uint64_t startTime;
    };

}


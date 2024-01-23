//
// Copyright (c) 2021 Julian Hinxlage. All rights reserved.
//

#pragma once

#include "VertexArray.h"
#include <string>
#include <glm/glm.hpp>

namespace baseline {

    class Mesh {
    public:
        Mesh();

        bool load(const std::string &file);
        bool save(const std::string& file);
        void create(float *vertices, int vertexCount, int *indices, int indexCount, std::vector<Attribute> layout = {{Type::FLOAT, 3}, {Type::FLOAT, 3}, {Type::FLOAT, 2}}, bool keepData = false);

        const std::vector<float>& getVertexData() { return vertexData; }
        const std::vector<int>& getIndexData() { return indexData; }

        VertexArray vertexArray;
        glm::vec3 boundingMin;
        glm::vec3 boundingMax;
        int changeCounter;
    private:
        std::vector<float> vertexData;
        std::vector<int> indexData;
    };

}


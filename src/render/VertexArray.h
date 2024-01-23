//
// Copyright (c) 2021 Julian Hinxlage. All rights reserved.
//

#pragma once

#include "Buffer.h"
#include "enum.h"
#include <memory>
#include <vector>

namespace baseline {

    class Attribute {
    public:
        Type type;
        int count;
        bool normalized;
        int size;
        int offset;

        Attribute(Type type = Type::FLOAT, int count = 1, bool normalized = false);
    };

    //a GPU vertex buffer (represents a mesh)
    class VertexArray {
    public:
        VertexArray();
        explicit VertexArray(const VertexArray &vertexArray);
        ~VertexArray();

        void bind();
        static void unbind();
        uint32_t getId();

        void addIndexBuffer(const std::shared_ptr<Buffer> &indexBuffer, Type type);
        void addVertexBuffer(const std::shared_ptr<Buffer> &vertexBuffer, std::vector<Attribute> layout, int divisor = 0);
        void submit(int vertexCount = -1, int instanceCount = -1);
        void setPrimitive(Primitive primitive);
        void clear();
        int getVertexCount();

    private:
        uint32_t id;
        uint32_t nextAttribute;
        Primitive primitive;

        class IBuffer{
        public:
            std::shared_ptr<Buffer> indexBuffer;
            Type type;
        };
        std::vector<IBuffer> indexBuffer;

        class VBuffer{
        public:
            std::shared_ptr<Buffer> vertexBuffer;
            std::vector<Attribute> layout;
            int divisor;
        };
        std::vector<VBuffer> vertexBuffer;
    };

}


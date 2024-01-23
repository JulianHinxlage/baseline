//
// Copyright (c) 2021 Julian Hinxlage. All rights reserved.
//

#pragma once

#include "Texture.h"
#include <unordered_map>
#include <memory>

namespace baseline {

    class FrameBufferAttachmentSpec {
    public:
        TextureAttachment type = TextureAttachment::COLOR;
        Color clearColor = color::white;
        std::string name = "";
        glm::vec2 resizeFactor = {1, 1};
        bool clear = true;
        bool resize = true;
        bool mipMapping = true;
        TextureFormat textureFormat = TextureFormat::TEXTURE_FORMAT_NONE;
        bool magNearest = true;
        bool minNearest = false;
        bool sRepeat = true;
        bool tRepeat = true;
        bool useBorderColor = false;
        Color borderColor = color::white;
    };

    class FrameBuffer {
    public:
        FrameBuffer();
        FrameBuffer(const FrameBuffer &frameBuffer);
        FrameBuffer(uint32_t width, uint32_t height, std::vector<FrameBufferAttachmentSpec> specs);
        ~FrameBuffer();

        void bind() const;
        static void unbind();
        uint32_t getId() const;
        glm::vec2 getSize();

        void init(uint32_t width, uint32_t height, std::vector<FrameBufferAttachmentSpec> specs);

        std::shared_ptr<Texture> getAttachment(TextureAttachment attachment);
        std::shared_ptr<Texture> getAttachment(const std::string& name);
        const FrameBufferAttachmentSpec* getAttachmentSpec(TextureAttachment attachment);
        const FrameBufferAttachmentSpec *getAttachmentSpec(const std::string &name);
        std::shared_ptr<Texture> setAttachment(FrameBufferAttachmentSpec spec, const std::shared_ptr<Texture> &texture);
        std::shared_ptr<Texture> setAttachment(FrameBufferAttachmentSpec spec);

        void resize(uint32_t width, uint32_t height);
        void clear();
        void clear(TextureAttachment attachment);
    private:
        uint32_t id;
        uint32_t width;
        uint32_t height;

        class Attachment{
        public:
            std::shared_ptr<Texture> texture;
            FrameBufferAttachmentSpec spec;
        };

        std::unordered_map<uint32_t, Attachment> attachments;
    };

}


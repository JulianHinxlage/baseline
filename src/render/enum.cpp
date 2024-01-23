//
// Copyright (c) 2021 Julian Hinxlage. All rights reserved.
//

#include "enum.h"
#include <GL/glew.h>
#include <cstdio>

namespace baseline {

    uint32_t internalEnum(Type type) {
        switch (type) {
            case Type::INT8:
                return GL_BYTE;
            case Type::INT16:
                return GL_SHORT;
            case Type::INT32:
                return GL_INT;
            case Type::UINT8:
                return GL_UNSIGNED_BYTE;
            case Type::UINT16:
                return GL_UNSIGNED_SHORT;
            case Type::UINT32:
                return GL_UNSIGNED_INT;
            case Type::FLOAT:
                return GL_FLOAT;
            default:
                return GL_NONE;
        }
    }

    uint32_t internalEnum(Primitive primitive) {
        switch (primitive) {
            case Primitive::POINTS:
                return GL_POINTS;
            case Primitive::LINES:
                return GL_LINES;
            case Primitive::TRIANGLES:
                return GL_TRIANGLES;
            case Primitive::QUADS:
                return GL_QUADS;
            default:
                return GL_NONE;
        }
    }

    uint32_t internalEnum(TextureAttachment attachment) {
        switch(attachment){
            case TextureAttachment::DEPTH:
                return GL_DEPTH_ATTACHMENT;
            case TextureAttachment::STENCIL:
                return GL_STENCIL_ATTACHMENT;
            case TextureAttachment::COLOR:
                return GL_COLOR_ATTACHMENT0;
            default:
                if(attachment > TextureAttachment::COLOR && (int)attachment < (int)TextureAttachment::COLOR + 16){
                    return GL_COLOR_ATTACHMENT0 + ((int)attachment - (int)TextureAttachment::COLOR);
                }else{
                    return GL_NONE;
                }
        }
    }

    uint32_t internalEnum(TextureFormat format) {
        switch (format) {
            case TextureFormat::TEXTURE_FORMAT_NONE:
                return GL_NONE;
            case TextureFormat::RGBA8:
                return GL_RGBA8;
            case TextureFormat::RGB8:
                return GL_RGB8;
            case TextureFormat::RG8:
                return GL_RG8;
            case TextureFormat::RED8:
                return GL_R8;


            case TextureFormat::RGB16F:
                return GL_RGB16F;
            case TextureFormat::RGB32F:
                return GL_RGB32F;
            case TextureFormat::RGBA16F:
                return GL_RGBA16F;
            case TextureFormat::RGBA32F:
                return GL_RGBA32F;

            case TextureFormat::RED16:
                return GL_R16F;
            case TextureFormat::RED32:
                return GL_R32F;
            case TextureFormat::RED32INT:
                return GL_R32I;
            case TextureFormat::ALPHA8:
                return GL_ALPHA8;
            case TextureFormat::ALPHA16:
                return GL_ALPHA16;
            case TextureFormat::DEPTH16:
                return GL_DEPTH_COMPONENT16;
            case TextureFormat::DEPTH24:
                return GL_DEPTH_COMPONENT24;
            case TextureFormat::DEPTH32:
                return GL_DEPTH_COMPONENT32;
            case TextureFormat::DEPTH24STENCIL8:
                return GL_DEPTH24_STENCIL8;
            default:
                return GL_NONE;
        }
    }

    uint32_t internalEnum(TextureType type) {
        switch (type) {
            case TextureType::TEXTURE_2D:
                return GL_TEXTURE_2D;
            case TextureType::TEXTURE_3D:
                return GL_TEXTURE_3D;
            case TextureType::TEXTURE_CUBE_MAP:
                return GL_TEXTURE_CUBE_MAP;
            default:
                return GL_NONE;
        }
    }

    uint32_t internalEnum(BufferType type) {
        switch (type) {
            case BufferType::VERTEX_BUFFER:
                return GL_ARRAY_BUFFER;
            case BufferType::INDEX_BUFFER:
                return GL_ELEMENT_ARRAY_BUFFER;
            case BufferType::UNIFORM_BUFFER:
                return GL_UNIFORM_BUFFER;
            default:
                return GL_NONE;
        }
    }

    uint32_t internalEnumSize(Type type) {
        switch (type) {
            case Type::INT8:
                return 1;
            case Type::INT16:
                return 2;
            case Type::INT32:
                return 4;
            case Type::UINT8:
                return 1;
            case Type::UINT16:
                return 2;
            case Type::UINT32:
                return 4;
            case Type::FLOAT:
                return 4;
            default:
                return 0;
        }
    }

    uint32_t internalEnumSize(Primitive primitive) {
        switch (primitive) {
            case Primitive::POINTS:
                return 1;
            case Primitive::LINES:
                return 2;
            case Primitive::TRIANGLES:
                return 3;
            case Primitive::QUADS:
                return 4;
            default:
                return 0;
        }
    }

    uint32_t internalEnumSize(TextureFormat format) {
        switch (format) {
            case TextureFormat::RGBA8:
                return 32;
            case TextureFormat::RGB8:
                return 24;
            case TextureFormat::RG8:
                return 16;
            case TextureFormat::RED8:
                return 8;
            case TextureFormat::RED16:
                return 16;
            case TextureFormat::RED32:
                return 32;
            case TextureFormat::RED32INT:
                return 32;
            case TextureFormat::ALPHA8:
                return 8;
            case TextureFormat::ALPHA16:
                return 16;
            case TextureFormat::DEPTH16:
                return 16;
            case TextureFormat::DEPTH24:
                return 24;
            case TextureFormat::DEPTH32:
                return 32;
            default:
                return 0;
        }
    }

}

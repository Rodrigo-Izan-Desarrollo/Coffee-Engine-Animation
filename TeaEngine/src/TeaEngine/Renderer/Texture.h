#pragma once

#include "TeaEngine/Core/Base.h"
#include "TeaEngine/Renderer/Image.h"

#include <cstdint>
#include <string>

namespace Tea {

    enum class ImageFormat
    {
        R8,
        RG8,
        RGB8,
        RGBA8
    };

    class Texture
    {
    public:
        Texture(const std::string& path);
        ~Texture();

        void Bind(uint32_t slot);

         std::pair<int, int> GetSize() { return std::make_pair(m_Width, m_Height); };
         int GetWidth() { return m_Width; };
         int GetHeight() { return m_Width; };
        const std::string& GetPath() { return m_FilePath; };
        ImageFormat GetImageFormat() { return m_Format; };

        static Ref<Texture> Load(const std::string& path);
    private:
        std::string m_FilePath;
        uint32_t m_textureID;
        int m_Width, m_Height;
        ImageFormat m_Format;
    };

}

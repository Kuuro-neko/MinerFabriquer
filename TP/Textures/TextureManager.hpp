#pragma once

#include <TP/Textures/Texture.hpp>
#include <unordered_map>

class TextureManager {
private:
    TextureManager();
    std::unordered_map<std::string, PBRTexture*> pbrTextures;
public:
    static TextureManager& getInstance() {
        static TextureManager instance;
        return instance;
    };

    static void initialize() {
        getInstance();
    }

    PBRTexture* getPBRTexture(const std::string& name);

    inline void addPBRTexture(const std::string& name, PBRTexture* texture) {
        pbrTextures[name] = texture;
    }
};
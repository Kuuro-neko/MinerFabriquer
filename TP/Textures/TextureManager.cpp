#include <TP/Textures/TextureManager.hpp>

TextureManager::TextureManager() {
    addPBRTexture("zombie", new PBRTexture("../textures/zombie/zombie.png",
        "../textures/zombie/zombie_normal.png",
        "../textures/zombie/zombie_roughness.png",
        "../textures/zombie/zombie_metallic.png"));
    
    addPBRTexture("ice_zombie", new PBRTexture("../textures/zombie/ice_zombie.png",
        "../textures/zombie/zombie_normal.png",
        "../textures/zombie/ice_zombie_roughness.png",
        "../textures/zombie/zombie_metallic.png"));
    
    addPBRTexture("husk", new PBRTexture("../textures/zombie/husk.png",
        "../textures/zombie/zombie_normal.png",
        "../textures/zombie/husk_roughness.png",
        "../textures/zombie/zombie_metallic.png"));
    
    addPBRTexture("steve", new PBRTexture("../textures/steve/steve.png",
        "../textures/steve/steve_normal.png",
        "../textures/steve/steve_roughness.png",
        "../textures/steve/steve_metallic.png"));

}

PBRTexture* TextureManager::getPBRTexture(const std::string& name) {
    auto it = pbrTextures.find(name);
    if (it != pbrTextures.end()) {
        return it->second;
    }
    return nullptr;
}
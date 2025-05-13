#include <TP/Database/ItemTypes.hpp>
#include <vector>

std::pair<float, float> ItemData::getTexCoords(unsigned char side) {
    return std::make_pair(xTex, yTex);
}

ItemData* ItemDatabase::getItem(int id) {
    auto it = m_items.find(id);
    if (it != m_items.end()) {
        return &it->second;
    }
    return &m_items[ERROR_BLOC];
}

std::pair<float, float> ItemDatabase::getTexCoords(int id, unsigned char side) {
    auto it = m_items.find(id);
    if (it != m_items.end()) {
        return it->second.getTexCoords(side);
    }
    return std::make_pair(m_items[ERROR_BLOC].xTex, m_items[ERROR_BLOC].yTex);
}
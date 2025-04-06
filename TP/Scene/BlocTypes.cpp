#include <TP/Scene/BlocTypes.hpp>
#include <vector>

std::pair<float, float> BlockData::getTexCoords(unsigned char side) {
    if (side == BLOC_TOP) {
        return std::make_pair(xTexTop, yTexTop);
    }
    if (side == BLOC_BOTTOM) {
        return std::make_pair(xTexBottom, yTexBottom);
    }
    return std::make_pair(xTexSide, yTexSide);
}

BlockData* BlocDatabase::getBloc(int id) {
    auto it = m_blocs.find(id);
    if (it != m_blocs.end()) {
        return &it->second;
    }
    return &m_blocs[255];
}

std::pair<float, float> BlocDatabase::getTexCoords(int id, unsigned char side) {
    auto it = m_blocs.find(id);
    if (it != m_blocs.end()) {
        return it->second.getTexCoords(side);
    }
    return std::make_pair(m_blocs[255].xTexSide, m_blocs[255].yTexSide);
}
#pragma once

#include <exception>
#include <string>

class ChunkOOBException : public std::exception
{
public:
    ChunkOOBException(int x, int y, int z, int chunkX, int chunkY, int chunkZ)
        : m_x(x), m_y(y), m_z(z), m_chunkX(chunkX), m_chunkY(chunkY), m_chunkZ(chunkZ) {
        m_message = "ChunkOOBException: Trying to access local coords ("
            + std::to_string(m_x) + ", " + std::to_string(m_y) + ", " + std::to_string(m_z) +
            ") in chunk ("
            + std::to_string(m_chunkX) + ", " + std::to_string(m_chunkY) + ", " + std::to_string(m_chunkZ) + ")";
        }
    const char* what() const noexcept override
    {
        return m_message.c_str();
    }
    inline int getX() const { return m_x; }
    inline int getY() const { return m_y; }
    inline int getZ() const { return m_z; }
    inline int getChunkX() const { return m_chunkX; }
    inline int getChunkY() const { return m_chunkY; }
    inline int getChunkZ() const { return m_chunkZ; }
private:
    int m_x;
    int m_y;
    int m_z;
    int m_chunkX;
    int m_chunkY;
    int m_chunkZ;
    std::string m_message;
};
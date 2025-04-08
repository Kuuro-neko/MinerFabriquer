#include <TP/Scene/World.hpp>

void World::generation() {

    int groundLevel = 4;
    chunks.emplace(std::make_tuple(0, 0, 0), VoxelChunk(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE));
    VoxelChunk& chunk = chunks.at(std::make_tuple(0, 0, 0));
    chunk.setBloc(2, groundLevel+1, 2, LOG_OAK);
    chunk.setBloc(2, groundLevel+2, 2, LOG_OAK);
    chunk.setBloc(2, groundLevel+3, 2, LOG_OAK);
    chunk.setBloc(2, groundLevel+4, 2, LEAVES_OAK);
    chunk.setBloc(2, groundLevel+3, 3, LEAVES_OAK);
    chunk.setBloc(3, groundLevel+3, 2, LEAVES_OAK);
    chunk.setBloc(2, groundLevel+3, 1, LEAVES_OAK);
    chunk.setBloc(1, groundLevel+3, 2, LEAVES_OAK);
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_SIZE; j++) {
            chunk.setBloc(i, groundLevel-4, j, BEDROCK);
            chunk.setBloc(i, groundLevel-3, j, STONE);
            chunk.setBloc(i, groundLevel-2, j, DIRT);
            chunk.setBloc(i, groundLevel-1, j, DIRT);
            chunk.setBloc(i, groundLevel, j, GRASS);
        }
    }
    chunk.setBloc(8, groundLevel+4, 8, GLOWSTONE);
    chunk.setBloc(8, groundLevel+4, 9, GLOWSTONE);
    chunk.setBloc(9, groundLevel+4, 8, GLOWSTONE);
    chunk.setBloc(9, groundLevel+4, 9, GLOWSTONE);
    chunk.setBloc(8, groundLevel+5, 8, GLOWSTONE);
    chunk.setBloc(8, groundLevel+5, 9, GLOWSTONE);
    chunk.setBloc(9, groundLevel+5, 8, GLOWSTONE);
    chunk.setBloc(9, groundLevel+5, 9, GLOWSTONE);
    chunk.generateMesh();

    chunks.emplace(std::make_tuple(1, 0, 0), VoxelChunk(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE));
    VoxelChunk& chunk2 = chunks.at(std::make_tuple(1, 0, 0));
    chunk2.setBloc(2, groundLevel+1, 2, LOG_OAK);
    chunk2.setBloc(2, groundLevel+2, 2, LOG_OAK);
    chunk2.setBloc(2, groundLevel+3, 2, LOG_OAK);
    chunk2.setBloc(2, groundLevel+4, 2, LEAVES_OAK);
    chunk2.setBloc(2, groundLevel+3, 3, LEAVES_OAK);
    chunk2.setBloc(3, groundLevel+3, 2, LEAVES_OAK);
    chunk2.setBloc(2, groundLevel+3, 1, LEAVES_OAK);
    chunk2.setBloc(1, groundLevel+3, 2, LEAVES_OAK);
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_SIZE; j++) {
            chunk2.setBloc(i, groundLevel-4, j, BEDROCK);
            chunk2.setBloc(i, groundLevel-3, j, STONE);
            chunk2.setBloc(i, groundLevel-2, j, DIRT);
            chunk2.setBloc(i, groundLevel-1, j, DIRT);
            chunk2.setBloc(i, groundLevel, j, GRASS);
        }
    }
    chunk2.setBloc(8, groundLevel+4, 8, GLOWSTONE);
    chunk2.setBloc(8, groundLevel+4, 9, GLOWSTONE);
    chunk2.setBloc(9, groundLevel+4, 8, GLOWSTONE);
    chunk2.setBloc(9, groundLevel+4, 9, GLOWSTONE);
    chunk2.setBloc(8, groundLevel+5, 8, GLOWSTONE);
    chunk2.setBloc(8, groundLevel+5, 9, GLOWSTONE);
    chunk2.setBloc(9, groundLevel+5, 8, GLOWSTONE);
    chunk2.setBloc(9, groundLevel+5, 9, GLOWSTONE);
    chunk2.generateMesh();
}
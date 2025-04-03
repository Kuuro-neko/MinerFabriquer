#!/bin/bash

# Créer le répertoire de sortie si nécessaire
mkdir -p ./build/TP1

# Générer les fichiers de construction avec le mode Debug activé
cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Debug

# Construire le projet
cmake --build ./build -j16
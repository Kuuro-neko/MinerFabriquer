#! /bin/sh

convert metallic_atlas_x32.png -resize 320x320 metallic_atlas_x16.png
convert roughness_atlas_x32.png -resize 320x320 roughness_atlas_x16.png
convert normal_atlas_x32.png -resize 320x320 normal_atlas_x16.png
convert texture_atlas_x32.png -resize 320x320 texture_atlas_x16.png
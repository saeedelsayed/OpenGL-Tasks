# Procedural Terrain Rendering

This project implements a procedural terrain renderer as part of the Advanced Rendering exercise for the Computer Graphics I course at TU Dresden. The terrain includes detailed textures, normal mapping and specular highlights.
## Features
### 1. Procedural Terrain Geometry
- Generates flat tessellated terrain patches.
- Adds procedural height information dynamically in the vertex shader.
### 2. Grass and Rock Texturing
- Blends grass and rock textures based on the terrain's normal vector.
- Smooth transitions between textures for natural appearance.
### 3. Road with Alpha Map
- Road texture placement controlled by an alpha map.
- Smooth blending of road and surrounding terrain.
### 4. Specular Highlights
- Uses a specular map to add localized highlights to the road texture.
### 5. Normal Mapping
- Applies tangent-space normal mapping for realistic surface details on the road.
- Dynamically transforms normals to world space for accurate lighting.
## Implementation Details
### Vertex Shader (terrain.vert)
- Dynamically adjusts terrain height using getTerrainHeight function.
- Calculates normals using finite differences.
- Computes tangent and bitangent vectors for normal mapping.
### Fragment Shader (terrain.frag)
- Grass and rock texture blending.
- Road texture blending using an alpha map.
- Specular highlights using a specular map.
- Normal mapping for detailed road surface.
## Screenshots
### First step
Rendering a flat terrain
![Image](https://github.com/user-attachments/assets/8583ba30-7569-41be-b223-3f9afe267d6a)
### Second step
Adding height map
![Image](https://github.com/user-attachments/assets/3fdd1b24-383e-4433-863b-faab55132df5)
### Third step
Adding grass texture
![Image](https://github.com/user-attachments/assets/fff85a6f-fe8e-4f04-93de-1e1887500a55)
### fourth step
Adding rock texture
![Image](https://github.com/user-attachments/assets/6c5b27dd-9e2d-42c4-9473-e545c1ca68e6)
### Fifth step
Adding road, specular highlights and normal mapping 
![Image](https://github.com/user-attachments/assets/eb36eaad-d91d-4e54-97be-57b2e8f3bdb1)

## License
This project is the property of the Chair of Computer Graphics and Visualization, TU Dresden. Redistribution is prohibited.

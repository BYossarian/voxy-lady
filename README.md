# voxy-lady
Voxel engine

*** short term:

- better world gen
    - better structure for storing chunks in world
    - biomes
    - trees/plants/cactus
    - caves
    - clouds
    - more performant
    - multi-threaded
    - object pool for chunks
    - only build meshes once all neighbours have loaded block data: chunk status: FREE, BLOCKS_LOADED, MESH_BUILT
        (can do this in a background thread)

- improve shaders
    - shadows
    - semi-transparent water
    - PBR

*** long term:

- need to update persceptive if window aspect ratio changes

- collision detection/physics
    - ray-casting

- optimise
    - store one mesh with all possible vertices in a VBO, and use 
        indexed draws; would need a per-chunk buffer for texture atlas indices
    - low fidelity (even lower!) shader for chunks in the distance

- proper game states

- interaction with blocks/inventory
    - maintain symmetry between vertices array and VBO for chunks
    - persistence

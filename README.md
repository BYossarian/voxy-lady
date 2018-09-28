# voxy-lady
Voxel engine (more like boilerplate right now...)

### short term todo:

- performance
    - more performant mesh building
    - better structure for storing chunks in world? (see infinite-2d-array-view.h)
    - object pool for chunks (see infinite-2d-array-view.h - use as chunk pool)

- better world gen
    - biomes
    - trees/plants/cactus
    - caves
    - clouds

- improve shaders
    - shadows (cascaded shadow maps)
    - semi-transparent water
    - PBR

### long term todo:

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

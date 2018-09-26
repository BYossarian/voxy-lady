
#pragma once

struct Block {

    struct Properties {
        bool visible;
        bool solid;
        int frontTexture;
        int backTexture;
        int leftTexture;
        int rightTexture;
        int topTexture;
        int bottomTexture;
    };

    static constexpr int AIR = 0;
    static constexpr int GRASS = 1;
    static constexpr int DIRT = 2;
    static constexpr int ROCK = 3;
    static constexpr int SAND = 4;
    static constexpr int WATER = 5;

    static constexpr Properties properties[] = {
        { false, false, -1, -1, -1, -1, -1, -1 },
        { true, true, 3, 3, 3, 3, 2, 43 },
        { true, true, 43, 43, 43, 43, 43, 43 },
        { true, true, 0, 0, 0, 0, 0, 0 },
        { true, true, 40, 40, 40, 40, 40, 40 },
        { true, true, 356, 356, 356, 356, 356, 356 }
    };

    int type;

};

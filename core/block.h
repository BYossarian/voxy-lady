
#pragma once

#include <map>

struct Block {
    
    enum class Type { AIR = 0, GRASS = 1, DIRT = 2, ROCK = 3, SAND = 4, WATER = 5 };

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

    Type type;

    const Properties& getProperties() const { return properties[static_cast<int>(type)]; }

private:

    static constexpr Properties properties[] = {
        { false, false, -1, -1, -1, -1, -1, -1 },
        { true, true, 3, 3, 3, 3, 2, 43 },
        { true, true, 43, 43, 43, 43, 43, 43 },
        { true, true, 0, 0, 0, 0, 0, 0 },
        { true, true, 40, 40, 40, 40, 40, 40 },
        { true, true, 356, 356, 356, 356, 356, 356 }
    };

};

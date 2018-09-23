
#pragma once

// axis-aligned bounding box:
struct AABB {
    
    float xMin, xMax, yMin, yMax, zMin, zMax;

    bool intersects(const AABB &other) const {

        if (xMin > other.xMax) {
            return false;
        }

        if (xMax < other.xMin) {
            return false;
        }

        if (yMin > other.yMax) {
            return false;
        }

        if (yMax < other.yMin) {
            return false;
        }

        if (zMin > other.zMax) {
            return false;
        }

        if (zMax < other.zMin) {
            return false;
        }

        return true;

    }

};

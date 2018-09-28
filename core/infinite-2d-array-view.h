
#pragma once

#include <vector>
#include <cmath>

// ok, the situation we have is: imagine an infintely large 2D array.
// (this represents the 'infinite' 2D array of Chunks that make up our World)
// we (obivously) can only consider a finite 2D subarray so we take a finite
// 2D view of that array (i.e. the chunks in the vicinity of the player). This 
// view will go from baseX -> (baseX + side) and baseY -> (baseY + side). However, 
// a further complication is that we don't want to have to shuffle/move the elements 
// in our array as our view moves (i.e. if some element in our array represents 
// some point (x, y) then moving the view shouldn't change which point that element 
// represents). Therefore, we need to represent our  2D view within something like 
// a 2D cyclic buffer.
// NB: we're assuming the size of the view (side) doesn't change
template <typename T, int side>
struct Infinite2DArrayView {
    
    Infinite2DArrayView(int x, int y): view(side * side), baseIndex(0), baseX(x), baseY(y) {}

    std::vector<T> view;
    // baseIndex is the index within view that represents the 
    // upper-left of the view
    int baseIndex;
    // baseX and baseY are the coords (within the infinite 2D array) 
    // of the upper-left of the view
    int baseX;
    int baseY;

    void moveView(int x, int y) {

        if (x == baseX && y == baseY) { return; }

        // TODO: how to handle allocation/dellocation of objects moving out of view?
        // TODO: cache some of the values used in getViewIndex etc.?

    }

    // maps some x and y in the infinite array to the index within 
    // view. throws if x and y aren't currently within view.
    int getViewIndex(int x, int y) {

        if (x < baseX || y < baseY) {
            throw;
        }

        if (x >= baseX + side || y >= baseY + side) {
            throw;
        }

        int row = (y - baseY + std::floor(baseIndex / side)) % side;
        int column = (x - baseX + (baseIndex % side)) % side;

        return row * side + column;

    }

    int getXCoord(int i) {

        if (i < 0 || i >= view.size()) {
            throw;
        }

        return (i % side + side - baseI % side) % side + baseX;

    }

    int getYCoord(int i) {

        if (i < 0 || i >= view.size()) {
            throw;
        }

        return (std::floor(i / side) + side - std::floor(baseI / side)) % side + baseY;

    }

    T get(int x, int y) {
        return view[getViewIndex(x, y)];
    }

};

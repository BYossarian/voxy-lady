
// perlin noise
// adapted from https://github.com/sol-prog/Perlin_Noise
// which is itself a C++ adaption of Perlin's reference
// implementation, given here: https://mrl.nyu.edu/~perlin/noise/

#pragma once

#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>
#include <vector>

class PerlinNoise {
    // The permutation vector
    std::vector<int> p;
public:
    // Generate a new permutation vector based on the value of seed
    PerlinNoise(unsigned int seed);
    // Get a noise value, for 2D images z can have any value
    double noise(double x, double y, double z) const;

    PerlinNoise(const PerlinNoise&) = delete;
    PerlinNoise& operator=(const PerlinNoise&) = delete;

private:

    double fade(double t) const;
    double lerp(double t, double a, double b) const;
    double grad(int hash, double x, double y, double z) const;

};

// Generate a new permutation vector based on the value of seed
PerlinNoise::PerlinNoise(uint seed) {
    p.resize(256);

    // Fill p with values from 0 to 255
    std::iota(p.begin(), p.end(), 0);

    // Initialize a random engine with seed
    std::default_random_engine engine(seed);

    // Suffle  using the above random engine
    std::shuffle(p.begin(), p.end(), engine);

    // Duplicate the permutation vector
    p.insert(p.end(), p.begin(), p.end());
}

double PerlinNoise::noise(double x, double y, double z) const {
    // Find the unit cube that contains the point
    int X = static_cast<int>(floor(x)) & 255;
    int Y = static_cast<int>(floor(y)) & 255;
    int Z = static_cast<int>(floor(z)) & 255;

    // Find relative x, y,z of point in cube
    x -= floor(x);
    y -= floor(y);
    z -= floor(z);

    // Compute fade curves for each of x, y, z
    double u = fade(x);
    double v = fade(y);
    double w = fade(z);

    // Hash coordinates of the 8 cube corners
    int A = p[X] + Y;
    int AA = p[A] + Z;
    int AB = p[A + 1] + Z;
    int B = p[X + 1] + Y;
    int BA = p[B] + Z;
    int BB = p[B + 1] + Z;

    // Add blended results from 8 corners of cube
    double res = lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z), grad(p[BA], x-1, y, z)), lerp(u, grad(p[AB], x, y-1, z), grad(p[BB], x-1, y-1, z))), lerp(v, lerp(u, grad(p[AA+1], x, y, z-1), grad(p[BA+1], x-1, y, z-1)), lerp(u, grad(p[AB+1], x, y-1, z-1),  grad(p[BB+1], x-1, y-1, z-1))));
    return (res + 1.0)/2.0;
}

double PerlinNoise::fade(double t) const { 
    return t * t * t * (t * (t * 6 - 15) + 10);
}

double PerlinNoise::lerp(double t, double a, double b) const { 
    return a + t * (b - a); 
}

double PerlinNoise::grad(int hash, double x, double y, double z) const {
    int h = hash & 15;
    // Convert lower 4 bits of hash into 12 gradient directions
    double u = h < 8 ? x : y,
           v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

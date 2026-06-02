#pragma once

// Coherent value/gradient noise for procedural surface weathering. Self-contained
// (no external dependencies) and deterministic: the same (coords, seed) always
// returns the same value, so Maya re-evaluation is stable.

// Classic 3D Perlin noise. Returns a smooth pseudo-random value in roughly
// [-1, 1]. `seed` shuffles the internal permutation table so different seeds
// give independent fields.
double perlin(double x, double y, double z, int seed);

// Fractal Brownian motion: sums `octaves` layers of perlin() at doubling
// frequency and halving amplitude. octaves <= 0 returns 0 (no weathering).
// The result is normalised back toward [-1, 1]. This is the function the mesh
// builders call to drive displacement.
double fbm(double x, double y, double z, int octaves, int seed);

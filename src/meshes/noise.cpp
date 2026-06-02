#include "noise.h"
#include <cmath>

namespace {

// Ken Perlin's 6t^5 - 15t^4 + 10t^3 smoothstep: zero 1st and 2nd derivatives at
// the endpoints, so adjacent cells blend without visible creases.
inline double fade(double t) {
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

inline double lerp(double a, double b, double t) {
    return a + t * (b - a);
}

// Gradient: pick one of 12 edge-vector directions from the low 4 bits of the
// hash and dot it with the distance vector (x, y, z). This is Perlin's improved
// 2002 gradient set.
inline double grad(int hash, double x, double y, double z) {
    const int h = hash & 15;
    const double u = h < 8 ? x : y;
    const double v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

// Hash a lattice corner directly from its integer coordinates plus the seed.
// Avoids storing a 512-entry permutation table while still giving each seed an
// independent, well-mixed field. (Integer hash in the spirit of Wang/xxHash.)
inline int hash3(int x, int y, int z, int seed) {
    unsigned int h = static_cast<unsigned int>(seed) * 374761393u;
    h += static_cast<unsigned int>(x) * 668265263u;
    h += static_cast<unsigned int>(y) * 2246822519u;
    h += static_cast<unsigned int>(z) * 3266489917u;
    h = (h ^ (h >> 13)) * 1274126177u;
    h ^= (h >> 16);
    return static_cast<int>(h);
}

} // namespace

double perlin(double x, double y, double z, int seed) {
    // Unit cube that contains the point, and local coordinates within it.
    const int X = static_cast<int>(std::floor(x));
    const int Y = static_cast<int>(std::floor(y));
    const int Z = static_cast<int>(std::floor(z));
    const double xf = x - std::floor(x);
    const double yf = y - std::floor(y);
    const double zf = z - std::floor(z);

    const double u = fade(xf);
    const double v = fade(yf);
    const double w = fade(zf);

    // Hash the 8 cube corners.
    const int aaa = hash3(X,     Y,     Z,     seed);
    const int baa = hash3(X + 1, Y,     Z,     seed);
    const int aba = hash3(X,     Y + 1, Z,     seed);
    const int bba = hash3(X + 1, Y + 1, Z,     seed);
    const int aab = hash3(X,     Y,     Z + 1, seed);
    const int bab = hash3(X + 1, Y,     Z + 1, seed);
    const int abb = hash3(X,     Y + 1, Z + 1, seed);
    const int bbb = hash3(X + 1, Y + 1, Z + 1, seed);

    // Trilinear blend of the 8 corner gradient dot-products.
    const double x1 = lerp(grad(aaa, xf,       yf,       zf),
                           grad(baa, xf - 1.0, yf,       zf), u);
    const double x2 = lerp(grad(aba, xf,       yf - 1.0, zf),
                           grad(bba, xf - 1.0, yf - 1.0, zf), u);
    const double y1 = lerp(x1, x2, v);

    const double x3 = lerp(grad(aab, xf,       yf,       zf - 1.0),
                           grad(bab, xf - 1.0, yf,       zf - 1.0), u);
    const double x4 = lerp(grad(abb, xf,       yf - 1.0, zf - 1.0),
                           grad(bbb, xf - 1.0, yf - 1.0, zf - 1.0), u);
    const double y2 = lerp(x3, x4, v);

    return lerp(y1, y2, w);
}

double fbm(double x, double y, double z, int octaves, int seed) {
    if (octaves <= 0) return 0.0;

    double sum       = 0.0;
    double amplitude = 1.0;
    double frequency = 1.0;
    double norm      = 0.0;   // running sum of amplitudes, to renormalise to ~[-1,1]

    for (int o = 0; o < octaves; ++o) {
        // Offset each octave's seed so layers don't share lattice alignment.
        sum  += amplitude * perlin(x * frequency, y * frequency, z * frequency, seed + o * 1013);
        norm += amplitude;
        amplitude *= 0.5;
        frequency *= 2.0;
    }
    return sum / norm;
}

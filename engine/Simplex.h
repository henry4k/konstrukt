/* Simplex.h
 *
 * Copyright 2007 Eliot Eshelman
 * battlestartux@6by9.net
 *
 *
 *  This file is part of Battlestar Tux.
 *
 *  Battlestar Tux is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  Battlestar Tux is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Battlestar Tux; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef __SIMPLEX__
#define __SIMPLEX__

/* 2D, 3D and 4D Simplex functions - values (-1, 1).
 * This algorithm was originally designed by Ken Perlin, but my code has been adapted
 * from the implementation written by Stefan Gustavson (stegu@itn.liu.se)
 *
 * In many cases, you may think you only need a 1D noise function, but in practice 2D
 * is almost always better.  For instance, if you're using the current frame number
 * as the parameter for the noise, all objects will end up with the same noise value
 * at each frame.  By adding a second parameter on the second dimension, you can ensure
 * that each gets a unique noise value and they don't all look identical.
 */


// Multi-octave Simplex noise - multiple noise values are combined.
float SimplexNoise( const int octaves, const float persistence, const float scale, const float x, const float y );
float SimplexNoise( const int octaves, const float persistence, const float scale, const float x, const float y, const float z );

// Raw Simplex noise - a single noise value.
float SimplexRawNoise( const float x, const float y );
float SimplexRawNoise( const float x, const float y, const float z );

float RigidSimplexNoise( float x, float y, float H, float lacunarity, int octaves, float offset, float threshold );
float RigidSimplexNoise( float x, float y, float z, float H, float lacunarity, int octaves, float offset, float threshold );

#endif

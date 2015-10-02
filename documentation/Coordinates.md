Coordinates
===========

A short excursion on how to work with coordinates in graphics programming and
what needs to be remembered when working with these in Apoapsis:


Handedness
----------

Apoapsis uses a left handed coordinate system.

That means:
Given your actor stands at the worlds origin, with no rotation or whatsoever.
To move forward you would *increase its Z coordinate*.
To move right you would *increase its X coordinate*.
To move up you would *increase its Y coordinate*.


W Coordinate
------------

To put it short:  It is used to determine whether a vector defines a direction
or a position:

If W is 1, then the vector is a position in space.  
If W is 0, then the vector is a direction.


Matrices
--------

*Model space:*
Vectors here are relative to the models origin.

*World space:*
Vectors here are relative to the world origin.

*Camera space:*
Vectors here are relative to the camera.

*Homogeneous space:*
Map vectors to normalized device coordinates.  So that everything, which is
visible resides in a cube: (-1,-1,-1) to (+1,+1,+1).


What matrices transform vectors from one space into another?

```
Model Space
|
| Model Matrix
v
World Space
|
| View Matrix
v
Camera Space
|
| Projection Matrix
v
Homogeneous Space
```


Tangent space
-------------

Vectors are relative to a surface.


References
----------

- http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/
- https://www.gamedev.net/topic/211203-what-is-homogeneous-space/

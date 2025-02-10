# chunkmanip
*POLITEHNICA Bucharest*
*Faculty of Computer Science and Automatic Control*
*Burlacu Vasile - 311CD - 2024*
## Description
- Chunkmanip is a tool for making various operations in a 3D grid, called a chunk
- In a chunk, there are `width*height*depth` blocks
- Each block can be of three types: 0 - air, 1 - grass, 2 - wood, 3 - stone
- There are 10 operations that can be done to a given chunk, each represented by a function in `libchunk/`
- There is a checker for the tool, if you think you want to satisfy #valgrind :)
- The checker also comes with a chunk viewer made with matplotlib, to easen your burden if you want to do some work
## Functionality
### Place a block
 - Elementary, but will help in the future operations
 - Places a block at the given coordinates
 - If the coordinates don't belong to the chunk it will not be placed
### Fill a cuboid
 - Fills a cuboid between two given points
### Fill a sphere
 - Fills a sphere with the center at a given point and a given radius
### Build a shell
 - Makes a shell of a given *block type 1* around every given *block type 2*
 - Again, if any of the shell blocks don't belong to the chunk, that block will not be placed
### Fill in xOz
 - A *flood fill* type algorithm
 - If there is a block of type B at position (x,y,z), the function places `block` at all positions in the plane parallel to xOz that can be reached by moving one block at a time, parallel to the Ox and Oz axes, starting from position (x,y,z) and only passing through blocks of type B.
### Fill in space
- The same concept, but this time it fills in 3D
### Rotate around the Oy axis
- Returns the chunk rotated 90 degrees around the y-axis (or in the xOz plane). Viewed from above, the rotation is counterclockwise. The dimensions of the chunk returned by the function are: `new_width = depth`, `new_height = height`, and `new_depth = width`.
### Gravity
- The worst of them all :)
- Applies gravity to the whole chunk, with a little observation
- Gravity is applied to objects, not blocks
- Adjacent blocks of the same type form an object
- Objects can't supress eachother, only touch :)
- Gravity also treats the case when two objects are interdependent, treated them as a single one
### Compression
- For bigger chunks there is a need for data optimisation so the chunks can pe compressed
- Compression is done according to the `Run Length Encoding (RLE)`
- The result is a char array of runs, where each run can be of one(if the run has <32 blocks) byte or two(if the run has 32 <= blocks < 4096).
- Each run is a pair `(Block_type, length)`
### Decompression
- We would want to make operations to the encoded chunk, so we would need to decode it

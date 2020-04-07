# CSCI 441: Computer Graphics - Project 1

*Maintained by Dana Parker, Troy Oster, Ben Bushnell, and Michael S.*

## How to Build the Project

1. Within `csci441-graphics-proj01/`, go into the `src` directory via `cd src`
2. Within `src/` run `mkdir build`
3. Enter the `build` directory via `cd build`
4. Run `cmake ..`
5. Build the project by entering `make`
6. Run the executable binary by entering `./proj01`

## Controls

- W, A, S, and D keys
  - Control pitch and yaw of camera
- Arrow keys
  - Movement keys (up-arrow is forward, left-arrow is left, etc.)
- Spacebar
  - Switch between first- and third-person view

## Implemented Source/Header Files

### obj_loader.h
We created this file to implement the `loadObj` function which loads in models that are in the .obj format.
It's a simple .obj parser that was adapted from a tutorial [found online](http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/).

We understood that the purpose of the `loadObj` function would be to populate a vector with the buffer
data for our objects. To do that, we had to understand how a .obj stores vertex information. We soon found out that
not all .obj's are created equal, and decided it would be easiest to create a .obj parser that works with the .obj's
that are exported from blender. Both the Character and Maze model were made in blender.

The first thing that `loadObj` does is read in all of the vertices, the uv texture coordinates,
the normals, and the triangle information and stores them for later. Then, in a for loop, it uses all of the stored information
to populate the buffer data vector with each vertex in the format `[x,y,z|u,v|n1,n2,n3]`. We chose not to use an EBO,
as we didn't have enough experience to get it working. Therefore, there are likely vertices which are rendered multiple
times.

### tex_loader.h
This file houses the `loadTexture` function, which was taken from the texturing lab. We pass in the path of the texture,
and let `stb` do its thing.

## How we Implemented Texturing
It wasn't too difficult to modify the `renderer.h` file, which we pulled from a previous lab, so that it passed in the
texture to the fragment shader. However, it was interesting adding lighting and texture to the models simultaneously, which we hadn't done before. Once we realized that we simply had to multiply our lighting and texture vectors to obtain
the final output color, that became easy too.

## How We Implemented Camera Switching

To implement switching between first- and third-person camera view,
we first had to be able to keep track of whether
we were in first-person view or third-person view.
To do this, we made a global integer variable called `camera_mode`.
We then made two macros,
`FIRST_PERSON` (defined to 0) and `THIRD_PERSON` (defined to 1),
that we use to set the value of `camera_mode`.

To change between first- and third-person with the spacebar, we had `processInput` continually check if the spacebar was pressed and stored it's "status" (whether or not the spacebar is pressed) into an integer via the following statement:

```cpp
int spacebar_status = glfwGetKey(window, GLFW_KEY_SPACE);
```

We can then check to see if the spacebar is pressed via:

```cpp
 if (spacebar_status == GLFW_PRESS)
    {
        spacebar_is_pressed = true;
    }
```

We used the boolean `spacebar_is_pressed` as a safeguard for the program: We don't want the user to switch modes more than once per press of the spacebar. We can achieve this with the following:

```cpp
if (spacebar_status == GLFW_RELEASE && spacebar_is_pressed == true)
    {
        spacebar_is_pressed = false;
        camera_mode = !camera_mode;
```

The `spacebar_is_pressed = false;` statement is important so that we make the modifications needed to switch modes only once per press of the spacebar.

Now that we have switched the camera mode, we can look to the value of `camera_mode` to see whether we are in first-person view or third-person view and react accordingly.

### First-person View

The code that switches to first-person mode is as follows:

```cpp
// Entering first person
if (camera_mode == FIRST_PERSON)
{
    model.scale(0.0001, 0.0001, 0.0001);
    projection.perspective(45, 1, .01, 10);
    camera.projection = projection;
    camera.eye = Vector4(model.values[12] + 1, 1, model.values[14]);
    camera.origin = Vector4(0, 1, -1);
    camera.up = Vector4(0, 1, 0);
    camera.front = Vector4(0,0,0);
}
```

### Third-person View

The code that switches to third-person mode is as follows:

```cpp
// Entering third person
if (camera_mode == THIRD_PERSON)
{
    model.scale(0.9999, 0.9999, 0.9999);
    projection.ortho(-12, 12, -12, 12, 1, 11);
    camera.projection = projection;
    camera.eye = Vector4(0, 5, 0);
    camera.origin = Vector4(0, 0, 0);
    camera.up = Vector4(1, 0, 0);
    camera.front = Vector4(0, -5, 0);
}
```
Note how the third person view uses the orthogonal projection matrix, rather than the perspective projection matrix. The orthogonal matrix is use for the bird's eye view as it causes the maze to appear 2 dimensional. 


## References
1. [Learn Open Gl: Camera](https://learnopengl.com/Getting-started/Camera)

2. [OpenGL-Tutorial: Model Loading](http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/)
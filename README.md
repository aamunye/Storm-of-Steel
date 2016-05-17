# Storm of Steel - CS488 Winter 2016 Final Project

----

## Dependencies
* OpenGL 3.2+
* GLFW
    * http://www.glfw.org/
* Lua
    * http://www.lua.org/
* Premake4
    * https://github.com/premake/premake-4.x/wiki
    * http://premake.github.io/download.html
* GLM
    * http://glm.g-truc.net/0.9.7/index.html
* AntTweakBar
    * http://anttweakbar.sourceforge.net/doc/
* irrKlang
    * http://www.ambiera.com/irrklang/


---

## Building the project
We use **premake4** as our cross-platform build system. First you will need to build all
the static libraries that the projects depend on. To build the libraries, open up a
terminal, and **cd** to the top level of the Storm-of-Steel project directory and then run the
following:

    $ premake4 gmake
    $ make

This will build the following static libraries, and place them in the top level **lib**
folder of your cs488 project directory.
* libcs488-framework.a
* libglfw3.a
* libimgui.a

Next we can build the game itself.  To do this, **cd** into A5, and run the following terminal
commands in order to compile the A0 executable using all .cpp files in the A0 directory:

    $ cd A5/
    $ ./run.sh

---

## Manual
The controls are fairly simple.  This is a game with one solder(puppet) and one missile.
Your objective is to target and hit the advancing soldier.

1) Clicking on the W,A,S,D buttons makes the camera fly over the terrain.
2) Scrolling the mouse up and down zooms in and out.
3) Press space and a solder will start moving forwards(you can't control it, it just moves forward)
4) Click on a point on the screen to launch the rocket to that point.
    - Note that the rocket doesn't aim directly at the target,
  because there is a wind component that perturbs the eventual destination of the payload
5) When the rocket hits the ground or the solder, it disappears until you click on a point again
6) When the soldier is hit, they disappear until you press space again.

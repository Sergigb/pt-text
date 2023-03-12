One of the probably trillion implementations of 2D OpenGL text rendering. I originally implemented this for 
[another project](https://github.com/Sergigb/pec-engine) because I didn't want to have a dependency _just_ to render text (even though
we depend on FreeType lol!). Then I discovered that rendering text is kinda hard.

### How does it work?

The characters are rendered as quads with two triangles using indexed geometry. There are two main classes under ```src```, Atlas and Text2D. The
class Atlas creates a texture atlas and holds the information of each one of the characters (width, heigth, x displacement, etc). Text2D uses the
atlas information to create the vertex, index and texture buffers. Under the folder ```example``` there's and example of to use these classes.

This code will probably not integrate very well with your code, I'd recomend writing your own implementation and use this code as a guide. Or you could
just use a library but where's the fun in that?

### Dependencies

FreeType is necessary to create the atlas, but theoretically you could drop it from your OpenGL code if you create a separate application to save
the atlas as an image and the character data in some sort of file, and then load them in your OpenGL application.

The example uses GLFW and GLEW (Text2D also uses GLEW but it can be easily replaced with GLAD or whatever). The code is GLP'd because I like that license
but I won't legally prosecute you if you don't follow the terms.

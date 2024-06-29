One of the probably trillion implementations of 2D OpenGL text rendering. I originally implemented this for 
[another project](https://github.com/Sergigb/pec-engine) because I didn't want to have a dependency _just_ to render text (even though
we depend on FreeType lol!). Then I discovered that rendering text is kinda hard.

![example](data/example.jpg?raw=true "example")

### How does it work?

The characters are rendered as quads with two triangles using indexed geometry. There are two main classes under ```src```, Atlas and Text2D. The
class Atlas creates a texture atlas and holds the information of each one of the characters (width, height, x/y displacement, etc). Text2D uses the
atlas information to create the vertex, index and texture buffers. Under the folder ```example``` there's and example of how to use these classes.

You can render as many strings as you want with a single drawing call (numerically, we're limited by the number of indices in the index buffer because
it uses GL_UNSIGNED_SHORT, but switching to GL_INT or  GL_UNSIGNED_INT should increase this limit). Each Text2D instance can hold different strings
of different sizes and positions. We can also have differently colored strings within the same object. We can't have differently colored words in the
same string, maybe some day I will implement that.

This code will probably not integrate very well with your project, I'd recommend writing your own implementation and use this code as a guide. Or you could
just use a separate library but where's the fun in that?

### Dependencies

FreeType is necessary to create the atlas, but theoretically you could drop it from your OpenGL code if you create a separate application to save
the atlas as an image and the character data in some sort of file, and then load them in your OpenGL application.

The example uses GLFW and GLEW (Text2D also uses GLEW but it can be easily replaced with GLAD or whatever). The code is GLP'd because I like that license
but I won't legally prosecute you if you don't follow the terms.

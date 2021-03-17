#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class Texture {
    public: 
        GLuint id;
        char const* path;
        int width, height;
        int channels;

        Texture(
            char const* path
        ); 
};

Texture::Texture(char const* path) {

    stbi_set_flip_vertically_on_load(1);

    unsigned char* data = stbi_load(path, &(this->width), &(this->height),
                                 &(this->channels), 0);

    glGenTextures(1, &(this->id));
    glBindTexture(GL_TEXTURE_2D, this->id);

    if(channels == 3) //RGB
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    else if(channels == 4) //RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
};


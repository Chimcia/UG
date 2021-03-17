class Program {
    public:
        GLuint id;

        Program(
            const char* vertex_shader, 
            const char* fragment_shader
        );
};

Program::Program(const char* vertex_shader, const char* fragment_shader) {
    id = glCreateProgram();

	glAttachShader(id, LoadShader(GL_VERTEX_SHADER, vertex_shader));
	glAttachShader(id, LoadShader(GL_FRAGMENT_SHADER, fragment_shader));

	LinkAndValidateProgram(id);
};
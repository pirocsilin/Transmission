#ifndef GLSLPROGRAM_H
#define GLSLPROGRAM_H
#include <string>
#include <QOpenGLFunctions>

class GLSLProgram: protected QOpenGLFunctions {
public:
    GLSLProgram();
    ~GLSLProgram();

    void compileShaders();

    void linkShaders();

    void addAttribute(const std::string& attributeName);

    void use();
    void unuse();

    GLint getUniformLocation(const std::string uniformName);

private:
    int numAttributes;
    void compileShader(const std::string& code, GLuint id);
    GLuint programID;
    GLuint vertexShaderID;
    GLuint fragmentShaderID;
};

#endif // GLSLPROGRAM_H

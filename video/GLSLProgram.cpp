#include "GLSLProgram.h"
#include "error.h"
#include <vector>
#include <fstream>
#include <QDebug>


GLSLProgram::GLSLProgram() : numAttributes(0), programID(0), vertexShaderID(0), fragmentShaderID(0)
{
}

GLSLProgram::~GLSLProgram()
{

}

GLint GLSLProgram::getUniformLocation(const std::string uniformName)
{
    GLint location = glGetUniformLocation(programID, uniformName.c_str());
    if (location == 0xFFFFFFFF) {
        qDebug() << "Uniform " << " not found in shader!";
    }
    return location;
}

void GLSLProgram::use()
{
    glUseProgram(programID);
    for (int i = 0; i < numAttributes; i++) {
        glEnableVertexAttribArray(i);
    }
}

void GLSLProgram::unuse()
{
    for (int i = 0; i < numAttributes; i++) {
        glDisableVertexAttribArray(i);
    }
    glUseProgram(0);
}

void GLSLProgram::addAttribute(const std::string& attributeName)
{
    glBindAttribLocation(programID, numAttributes++, attributeName.c_str());
}

void GLSLProgram::compileShaders()
{
    initializeOpenGLFunctions();
    // Vertex and fragment shaders are successfully compiled.
    // Now time to link them together into a program.
    // Get a program object.
    programID = glCreateProgram();

    vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    if (vertexShaderID == 0) {
        fatalError("Vertex shader failed to be created");
    }

    fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    if (fragmentShaderID == 0) {
        fatalError("Fragment shader failed to be created");
    }

    const std::string fragmentShaderFilePath = R""(
        #version 100
        precision mediump float;
        uniform sampler2D mySampler;
        varying vec4 fragmentColor;
        varying vec2 fragmentUV;
        void main() {
            vec4 textureColor = texture2D(mySampler, fragmentUV);
            gl_FragColor = textureColor; //* fragmentColor;
        }
    )"";

    const std::string vertexShaderFilePath = R""(
        #version 100
        attribute vec2 vertexPosition;
        attribute vec4 vertexColor;
        attribute vec2 vertexUV;
        varying vec2 fragmentPosition;
        varying vec4 fragmentColor;
        varying vec2 fragmentUV;
        uniform mat4 rotationMatrix;
        void main() {
            gl_Position = rotationMatrix * vec4(vertexPosition, 0.0, 1.0);
            fragmentPosition = vertexPosition;
            fragmentColor = vertexColor;
            fragmentUV = vec2(vertexUV.x, 1.0 - vertexUV.y);
        }
    )"";


    compileShader(vertexShaderFilePath, vertexShaderID);
    compileShader(fragmentShaderFilePath, fragmentShaderID);
}

void GLSLProgram::linkShaders()
{
    // Attach our shaders to our program
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);

    // Link our program
    glLinkProgram(programID);

    // Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    glGetProgramiv(programID, GL_LINK_STATUS, (int *)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<char> infoLog(maxLength);
        glGetProgramInfoLog(programID, maxLength, &maxLength, &infoLog[0]);

        // We don't need the program anymore.
        glDeleteProgram(programID);
        // Don't leak shaders either.
        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);

        // Use the infoLog as you see fit.
        std::printf("%s\n", &infoLog[0]);
        fatalError("Shaders failed to link!");
    }

    // Always detach shaders after a successful link.
    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
}

void GLSLProgram::compileShader(const std::string& code, GLuint id)
{

    const char* contentsPtr =  code.c_str();

    glShaderSource(id, 1, &contentsPtr, nullptr);

    glCompileShader(id);

    GLint sucess = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &sucess);

    if(sucess == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(id, maxLength, &maxLength, &errorLog[0]);

        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        glDeleteShader(id); // Don't leak the shader.
        std::printf("%s\n", &errorLog[0]);
        fatalError("Shader failed to compile");
    }
}

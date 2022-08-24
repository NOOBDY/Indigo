#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include <string>

#include <GL/glew.h>

class Program {
public:
    Program(const std::string &vertexShaderFilepath,
            const std::string &fragmentShaderFilepath);
    ~Program();

    void Bind() const;
    void Unbind() const;

    GLuint GetProgramID() const { return m_ProgramID; }

private:
    std::string LoadFile(const std::string &filepath);
    void CompileShader(const GLuint shaderID, const std::string &shaderSrc);
    void LinkProgram();

private:
    GLuint m_ProgramID;

    GLuint m_VertexShaderID;
    GLuint m_FragmentShaderID;
};

#endif
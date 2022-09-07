#include "program.hpp"

#include <fstream>
#include <sstream>

Program::Program(const std::string &vertexShaderFilepath,
                 const std::string &fragmentShaderFilepath) {
    LOG_TRACE("Creating Program");
    m_ProgramID = glCreateProgram();

    m_VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    m_FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    CompileShader(m_VertexShaderID, vertexShaderFilepath);
    CompileShader(m_FragmentShaderID, fragmentShaderFilepath);

    LinkProgram();
}

Program::~Program() {
    LOG_TRACE("Deleting Program");
    glDeleteProgram(m_ProgramID);
}

void Program::Bind() const {
    glUseProgram(m_ProgramID);
}

void Program::Unbind() const {
    glUseProgram(0);
}

void Program::SetInt(const std::string &name, int value) {
    GLuint location = glGetUniformLocation(m_ProgramID, name.c_str());
    glUniform1i(location, value);
}

std::string Program::LoadShaderFile(const std::string &filepath) {
    std::string source;
    std::ifstream stream(filepath, std::ios::in);

    LOG_TRACE("Loading File: '{}'", filepath);

    if (stream.is_open()) {
        std::stringstream sstr;
        sstr << stream.rdbuf();
        source = sstr.str();
        stream.close();
    } else {
        LOG_ERROR("Failed Loading File: '{}'", filepath);
        throw;
    }

    return source;
}

void Program::CompileShader(const GLuint shaderID,
                            const std::string &shaderFilepath) {
    std::string shaderSrc = LoadShaderFile(shaderFilepath);
    const char *srcPtr = shaderSrc.c_str();

    LOG_TRACE("Compiling Shader: '{}'", shaderFilepath);
    glShaderSource(shaderID, 1, &srcPtr, NULL);
    glCompileShader(shaderID);

    GLint status = GL_FALSE;

    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        int infoLogLength;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::vector<char> errMessage(infoLogLength + 1);
        glGetShaderInfoLog(shaderID, infoLogLength, NULL, &errMessage[0]);

        LOG_ERROR("Failed to Compile Shader: '{}'\n{}", shaderFilepath,
                  &errMessage[0]);
        throw;
    } else {
        LOG_TRACE("Compiling Successful");
    }
}

void Program::LinkProgram() {
    LOG_TRACE("Linking Program");

    glAttachShader(m_ProgramID, m_VertexShaderID);
    glAttachShader(m_ProgramID, m_FragmentShaderID);
    glLinkProgram(m_ProgramID);

    /*
     * The error handling here seems redundant but
     * the linking process doesn't seem to happen until
     * the program enters the rendering loop. So it's
     * better to handle it here for better tracing.
     * Also, glGetProgramInfoLog produces better error
     * messages than the general OpenGLDebugMessageCallback
     */
    GLint status = GL_FALSE;

    glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        int infoLogLength;
        glGetProgramiv(m_ProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::vector<char> errMessage(infoLogLength + 1);
        glGetProgramInfoLog(m_ProgramID, infoLogLength, NULL, &errMessage[0]);

        LOG_ERROR("Failed to Link Program:\n{}", &errMessage[0]);
        throw;
    } else {
        LOG_TRACE("Linking Successful");
    }

    glDetachShader(m_ProgramID, m_VertexShaderID);
    glDetachShader(m_ProgramID, m_FragmentShaderID);

    glDeleteShader(m_VertexShaderID);
    glDeleteShader(m_FragmentShaderID);
}
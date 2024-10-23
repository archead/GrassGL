#pragma once
#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include <glad/glad.h>

class VertexArray {
public:
    VertexArray();  // Constructor
    ~VertexArray(); // Destructor

    void bind() const;  // Bind the VAO
    void unbind() const; // Unbind the VAO
    void addBuffer(GLuint bufferID, GLuint index, GLint size, GLenum type, GLboolean normalized = GL_FALSE, GLsizei stride = 0, const void* pointer = nullptr);

private:
    GLuint vaoID; // VAO identifier
};

#endif // VERTEX_ARRAY_H

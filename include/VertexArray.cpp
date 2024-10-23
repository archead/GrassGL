#include "VertexArray.h"

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &vaoID);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &vaoID);
}

void VertexArray::bind() const
{
	glBindVertexArray(vaoID);
}

void VertexArray::unbind() const
{
	glBindVertexArray(0);
}

void VertexArray::addBuffer(GLuint bufferID, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer)
{
	bind();
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);
	glVertexAttribPointer(index, size, type, normalized, stride, pointer);
	glEnableVertexAttribArray(index);
}

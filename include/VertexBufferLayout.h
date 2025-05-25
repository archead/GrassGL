#pragma once
#include <vector>
#include <glad/glad.h>

struct VertexBufferElement
{
	unsigned int type;
	unsigned int count;
	bool normalized;
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;
public:
	VertexBufferLayout()
		: m_Stride(0) {}


	template<typename T>
	void Push(int count)
	{
		static_assert(false);
	}

	template<>
	void Push<float>(int count)
	{
		m_Elements.push_back({ static_cast<unsigned int>(GL_FLOAT), static_cast<unsigned int>(count), false });
		m_Stride += sizeof(GLfloat) * count;
	}

	template<>
	void Push<unsigned int>(int count)
	{
		m_Elements.push_back({ static_cast<unsigned int>(GL_UNSIGNED_INT), static_cast<unsigned int>(count), false });
		m_Stride += sizeof(GLuint) * count;
	}

	template<>
	void Push<unsigned char>(int count)
	{
		m_Elements.push_back({ static_cast<unsigned int>(GL_UNSIGNED_BYTE), static_cast<unsigned int>(count), true });
		m_Stride += sizeof(GLubyte) * count;
	}

	inline const std::vector<VertexBufferElement> GetElements() const { return m_Elements; }
	inline unsigned int GetStride() const { return m_Stride; }
};


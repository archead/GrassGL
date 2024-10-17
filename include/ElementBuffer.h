#pragma once
class ElementBuffer
{
private:
	unsigned int m_RendererID;
	unsigned int m_Count;

public:
	ElementBuffer(const unsigned int* data, unsigned int size);
	~ElementBuffer();

	void bind() const;
	void unbind() const;
};
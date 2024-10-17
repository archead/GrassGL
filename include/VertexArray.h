#pragma once

#include "VertexBuffer.h"

class VertexArray
{
private:

public:
	VertexArray();
	~VertexArray();

	void addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
};


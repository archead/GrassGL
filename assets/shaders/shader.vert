#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aOffset;
layout (location = 3) in vec3 aNormal;


out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;


uniform mat4 model;
uniform mat4 shearFast;
uniform mat4 shearSlow;
uniform mat4 viewProj;
uniform float time;
uniform mat4 rotateOffsetTemplate;

float hash(float n) { return fract(sin(n) * 1e4); } // taken from https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83

// Constant for PI
const float PI = 3.14159265359;

 void main()
 {
    vec3 offset = aOffset;
	float hashedInstance = hash(gl_InstanceID);

	float swayAmount = 0.1f * (sin(2.0f * time + hashedInstance) + 0.25f * sin(3.0f * time + hashedInstance));
	swayAmount *= smoothstep(0.3f, 1.0f, 1 - offset.y);

	vec3 swayedPos = aPos + offset;
	swayedPos.x += swayAmount;

	mat4 rotateOffset = rotateOffsetTemplate;

	rotateOffset[0][0] = cos(hashedInstance * (PI/180));
	rotateOffset[2][0] = sin(hashedInstance * (PI/180));
	rotateOffset[0][2] = -sin(hashedInstance * (PI/180));
	rotateOffset[2][2] = cos(hashedInstance * (PI/180));

	if(gl_InstanceID == 0)
	{
		gl_Position = viewProj * model * vec4(aPos, 1.0f);
	}

	else
	{
		if (aPos.y > 0)
		{
			gl_Position = viewProj * rotateOffset * model * vec4(swayedPos, 1.0f);
		}

		else 
		{
			gl_Position = viewProj * model * vec4(aPos.x + offset.x, aPos.y, aPos.z + offset.z, 1.0f);
		}
	}

	FragPos = vec3(model * vec4(aPos, 1.0f));
	Normal = mat3(transpose(inverse(model))) * aNormal;
	TexCoord = aTexCoord;
 }

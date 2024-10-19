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

//mat4x4(
//(1.000000, 0.000000, 0.000000, 0.000000),
//(0.016770, 1.000000, 0.000000, 0.000000),
//(0.000000, 0.000000, 1.000000, 0.000000),
//(0.008385, 0.000000, 0.000000, 1.000000))

float hash(float n) { return fract(sin(n) * 1e4); } // taken from https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83

 void main()
 {
 
    vec3 offset = aOffset;
	if(gl_InstanceID == 0)
		gl_Position = viewProj * model * vec4(aPos, 1.0f);
	else
	{
		if (aPos.y > 0)
		{
			mat4 scale;
			scale[0] = vec4(1.00f, 0.00f, 0.00f, 0.00f);  
			scale[1] = vec4(0.00f, 1.00f, 0.00f, 0.00f);
			scale[2] = vec4(0.00f, 0.00f, 1.00f, 0.00f); 
			scale[3] = vec4(0.00f, 0.00f, 0.00f, 1.00f); 
			mat4 sFast = scale;
			mat4 sSlow = scale;

			//sFast[1][0] = shearFast[1][0] / (2 + offset.y);
			//sSlow[1][0] = shearSlow[1][0] / (2 + offset.y);


			float hashedInstance = hash(gl_InstanceID);
			if(hashedInstance <= 0.3) {
				gl_Position = viewProj * model * shearFast * vec4(aPos + offset, 1.0f);
			}
			else {
				gl_Position = viewProj * model * shearSlow * vec4(aPos + offset, 1.0f);

			}
		}

		else
			gl_Position = viewProj * model * vec4(aPos.x + offset.x, aPos.y, aPos.z + offset.z, 1.0f);
	}

	FragPos = vec3(model * vec4(aPos, 1.0f));
	Normal = mat3(transpose(inverse(model))) * aNormal;
	TexCoord = aTexCoord;
 }

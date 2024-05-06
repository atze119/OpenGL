#version 460
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

uniform int visionEffect;

const float offset = 1.0 / 300.0;

void main()
{
	vec2 offsets[9] = vec2[](
		vec2(-offset, offset), // top-left
		vec2(0.0f, offset), // top-center
		vec2(offset, offset), // top-right
		vec2(-offset, 0.0f), // center-left
		vec2(0.0f, 0.0f), // center-center
		vec2(offset, 0.0f), // center-right
		vec2(-offset, -offset), // bottom-center
		vec2(0.0f, -offset), // bottom-center
		vec2(offset, -offset) // bottom-right
	);
	float kernel[9];
	if (visionEffect == 1)
	{
		kernel = float[](
			1.0 / 16, 2.0 / 16, 1.0 / 16,
			2.0 / 16, 4.0 / 16, 2.0 / 16,
			1.0 / 16, 2.0 / 16, 1.0 / 16
		);
	}
	else if (visionEffect == 2)
	{
		kernel = float[](
			-1, -1, -1,
			-1,  9, -1,
			-1, -1, -1
		);
	}
	else if (visionEffect == 3)
	{
		kernel = float[](
			1, 1, 1,
			1, -8, 1,
			1, 1, 1
		);
	}
	else
	{
		kernel = float[](
			0, 0, 0,
			0, 1, 0,
			0, 0, 0
		);
	}
	
	vec3 sampleTex[9];
	for (int i = 0; i < 9; i++)
	{
		sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
	}
	vec3 col = vec3(0.0);
	for (int i = 0; i < 9; i++)
		col += sampleTex[i] * kernel[i];
	FragColor = vec4(col, 1.0f);

}



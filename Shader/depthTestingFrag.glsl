#version 460
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{
	vec4 texColor = texture(texture1, TexCoords);
	// dropping fragments with alpha value smaller than 0.1 because they should be transparent!
	if (texColor.a < 0.1)
		discard;
	FragColor = texColor;
}
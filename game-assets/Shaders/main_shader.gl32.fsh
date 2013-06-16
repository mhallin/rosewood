#version 150 core

in vec3 colorVarying;
in vec2 texcoordVarying;

out vec4 fragColor;

uniform sampler2D rw_texture;

void main()
{
	vec4 tc = texture(rw_texture, texcoordVarying).rgba;
	float ta = tc.a;
    fragColor = vec4(colorVarying, 0.5)*(1.0-ta) + tc;
}

#version 150 core

in vec4 position;
in vec3 normal;
in vec2 texcoord;

out vec3 colorVarying;
out vec2 texcoordVarying;

uniform mat4 it_projection_matrix;
uniform mat4 it_mv_matrix;
uniform mat3 it_n_matrix;

void main()
{
    vec3 eyeNormal = normalize(it_n_matrix * normal);
    vec3 lightPosition = vec3(0.0, 0.0, 3.0);
    vec3 diffuseColor = vec3(0.2, 0.6, 0.2);

    float nDotVP = max(0.0, dot(eyeNormal, normalize(lightPosition)));

    colorVarying = diffuseColor * nDotVP;

    gl_Position = it_projection_matrix * it_mv_matrix * position;

	texcoordVarying = texcoord;
}

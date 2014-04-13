#version 150 core

in vec4 rw_vertex;
in vec3 rw_normal;
in vec2 rw_texcoord;

out vec3 colorVarying;
out vec2 texcoordVarying;

uniform mat4 rw_projection_matrix;
uniform mat4 rw_modelview_matrix;
uniform mat3 rw_normal_matrix;

uniform vec4 rw_light_position;
uniform vec4 rw_light_color;

void main()
{
    vec3 eyeNormal = normalize(rw_normal_matrix * rw_normal);

    float nDotVP = max(0.0, dot(eyeNormal, normalize(rw_light_position.xyz)));

    colorVarying = rw_light_color.rgb * nDotVP;

    gl_Position = rw_projection_matrix * rw_modelview_matrix * rw_vertex;

	texcoordVarying = rw_texcoord;
}

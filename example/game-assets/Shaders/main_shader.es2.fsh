varying lowp vec3 colorVarying;
varying lowp vec2 texcoordVarying;

uniform sampler2D rw_texture;

void main()
{
	lowp vec4 tc = texture2D(rw_texture, texcoordVarying).rgba;
	lowp float ta = tc.a;
    gl_FragColor = vec4(colorVarying, 0.5)*(1.0-ta) + tc;
}

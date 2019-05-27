attribute vec4 vPosition;
attribute vec2 a_texCoord;
uniform mat4 glMatrix;
varying vec2 tc;
void main()
{
gl_Position = glMatrix * vPosition;
tc = a_texCoord;
    gl_PointSize = 10.0f;\n"+
}
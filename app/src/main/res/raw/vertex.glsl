uniform mat4 uMVPMatrix;//总变换矩阵
attribute vec4 vPosition;
attribute vec2 a_texCoord;
varying vec2 v_texCoord;
void main() {
    //根据总变换矩阵计算此次绘制此顶点位置
    //gl_Position = vPosition;
    gl_Position = uMVPMatrix * vPosition;
    //将顶点的位置传给片元着色器
    v_texCoord = a_texCoord;
}
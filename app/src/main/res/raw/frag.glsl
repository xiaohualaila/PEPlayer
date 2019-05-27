precision mediump float;

varying lowp vec2 v_texCoord;
uniform sampler2D SamplerY;
uniform sampler2D SamplerU;
uniform sampler2D SamplerV;
//uniform vec4 vColor;
void main() {
    //gl_FragColor = vColor;
    mediump vec3 yuv;
    lowp vec3 rgb;
    yuv.x = texture2D(SamplerY, v_texCoord).r;
    yuv.y = texture2D(SamplerU, v_texCoord).r - 0.5;
    yuv.z = texture2D(SamplerV, v_texCoord).r - 0.5;
    rgb = mat3( 1,        1,       1,
                0,    -0.39465,  2.03211,
             1.13983, -0.58060,     0 ) * yuv;
    gl_FragColor = vec4(rgb,0);
}
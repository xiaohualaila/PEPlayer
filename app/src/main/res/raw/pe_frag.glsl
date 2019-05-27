varying lowp vec2 tc;
uniform int SampleType;    //0 = rgb, 1 = yuv球面， 2 = yuv平面
uniform sampler2D SamplerY;
uniform sampler2D SamplerU;
uniform sampler2D SamplerV;
uniform sampler2D SamplerA;
void main(void)
{
   if (SampleType == 0) {
       gl_FragColor = vec4(texture2D(SamplerY, tc).r,
                           texture2D(SamplerY, tc).g,
                           texture2D(SamplerY, tc).b, 1);
   } else {
       mediump vec3 yuv;
       lowp vec3 rgb;
       yuv.x = texture2D(SamplerY, tc).r;
       yuv.y = texture2D(SamplerU, tc).r - 0.5;
       yuv.z = texture2D(SamplerV, tc).r - 0.5;
       rgb = mat3( 1,        1,       1,
                    0,    -0.39465,  2.03211,
                 1.13983, -0.58060,     0 ) * yuv;
       if (SampleType == 1) {
           gl_FragColor = vec4(rgb, texture2D(SamplerA, tc).r);
       } else {
           gl_FragColor = vec4(rgb, 1);
       }
   }
}
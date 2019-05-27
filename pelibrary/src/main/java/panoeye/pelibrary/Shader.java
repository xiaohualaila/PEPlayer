package panoeye.pelibrary;

import android.content.Context;
import android.content.res.Resources;
import android.opengl.GLES20;
import android.util.Log;

import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.io.InputStreamReader;

/**
 * Created by tir on 2017/4/17.
 * 着色器类
 * OpenGL超级宝典笔记——GLSL语言基础：https://my.oschina.net/sweetdark/blog/208024#OSC_h3_12
 */
//attribute属性，使用顶点数组封装每个顶点的数据，一般用于每个顶点都不相同的变量，如顶点位置，颜色。
    //uniform 顶点着色器使用的常量数据，不能被着色器修改，一般用于对所有顶点都相同的变量
    //sampler 这个是可选的，特殊的uniform 表示顶点着色器使用的纹理
    //mat4 4*4 浮点数矩阵
 public class Shader {
    private static final String TAG = "Shader";
    private static final String vertexShaderCode =
                    "attribute vec4 vPosition;                          \n"+
                    "attribute vec2 a_texCoord;                         \n"+
                    "uniform mat4 glMatrix;                                         \n"+
                    "varying vec2 tc;                                   \n"+
                    "void main()                                                    \n"+
                    "{                                              \n"+
                    "gl_Position = glMatrix * vPosition;                \n"+
                    "tc = a_texCoord;                                   \n"+
                    "}                                              \n";
    private static final String fragmentShaderCode =
            "varying lowp vec2 tc;                              \n"+
            "uniform int SampleType;                            \n"+//0 = rgb, 1 = yuv球面， 2 = yuv平面
            "uniform sampler2D SamplerY;                        \n"+
            "uniform sampler2D SamplerU;                        \n"+
            "uniform sampler2D SamplerV;                        \n"+
            "uniform sampler2D SamplerA;                        \n"+
            "uniform sampler2D TurnY;                           \n"+
            "uniform sampler2D TurnU;                           \n"+
            "uniform sampler2D TurnV;                           \n"+
            "void main(void)                                    \n"+
            "{                                                  \n"+
            "   if (SampleType == 0) {                          \n"+
            "       gl_FragColor = vec4(texture2D(SamplerY, tc).r,\n"+
            "                           texture2D(SamplerY, tc).g,\n"+
            "                           texture2D(SamplerY, tc).b, 1);\n"+
            "   } else {                                        \n"+
            "       mediump vec3 yuv;                           \n"+
            "       lowp vec3 rgb;                              \n"+
            "       yuv.x = texture2D(SamplerY, tc).r;          \n"+
            "       yuv.y = texture2D(SamplerU, tc).r - 0.5;    \n"+
            "       yuv.z = texture2D(SamplerV, tc).r - 0.5;    \n"+
            "       rgb = mat3( 1,        1,       1,           \n"+
            "                    0,    -0.39465,  2.03211,      \n"+
            "                 1.13983, -0.58060,     0 ) * yuv; \n"+
//                    "       rgb = mat3( 1,        0,       1.13983,\n"+
//                    "                    1,    -0.39465,  -0.58060,\n"+
//                    "                 1, 2.03211,     0 ) * yuv; \n"+
            "       if (SampleType == 1) {                      \n"+
            "           gl_FragColor = vec4(rgb, texture2D(SamplerA, tc).r);\n"+
//                    "gl_FragColor = vec4(0.0,1.0,0.0,1.0);\n"+
            "       } else {                                    \n"+
            "           gl_FragColor = vec4(rgb, 1);            \n"+
//                    "gl_FragColor = vec4(0.0,1.0,0.0,1);\n"+
            "       }                                           \n"+
            "   }                                               \n"+
            "}                                                  \n";


    private static final String YUV420toRGB444JustGainFSPCode =
            "varying lowp vec2 tc;                              \n"+
                    "uniform int SampleType;                            \n"+//0 = rgb, 1 = yuv球面， 2 = yuv平面
//                    "uniform sampler2D SamplerY;                        \n"+
//                    "uniform sampler2D SamplerU;                        \n"+
//                    "uniform sampler2D SamplerV;                        \n"+
                    "uniform sampler2D SamplerA;                        \n"+
                    "uniform sampler2D TurnY;                           \n"+
                    "uniform sampler2D TurnU;                           \n"+
                    "uniform sampler2D TurnV;                           \n"+
                    "void main(void)                                    \n"+
                    "{                                                  \n"+
//                    "   if (SampleType == 0) {                          \n"+
//                    "       gl_FragColor = vec4(texture2D(SamplerY, tc).r,\n"+
//                    "                           texture2D(SamplerY, tc).g,\n"+
//                    "                           texture2D(SamplerY, tc).b, 1);\n"+
//                    "   } else {                                        \n"+
                    "       mediump vec3 yuv;                           \n"+
                    "       lowp vec3 rgb;                              \n"+
                    "    vec4 gain_Y = texture2D(TurnY,tc);  \n"+
                    "    vec4 gain_U = texture2D(TurnU,tc);  \n"+
                    "    vec4 gain_V = texture2D(TurnV,tc);  \n"+
                    "       yuv.x = (gain_Y.r);          \n"+
                    "       yuv.y = (gain_U.r-0.5);    \n"+
                    "       yuv.z = (gain_V.r-0.5);    \n"+
//            "       rgb = mat3( 1,        1,       1,           \n"+
//            "                    0,    -0.39465,  2.03211,      \n"+
//            "                 1.13983, -0.58060,     0 ) * yuv; \n"+

                    "	 rgb.r=yuv.x         +1.1403*yuv.z;						       \n"+
                    "	 rgb.g=yuv.x-0.3939*yuv.y-0.5808*yuv.z;						       \n"+
                    "	 rgb.b=yuv.x+2.0284*yuv.y;								       \n"+
                    "	 rgb.r = (rgb.r<0.0)?0.0:((rgb.r>1.0)?1.0:rgb.r);	       \n"+
                    "	 rgb.g = (rgb.g<0.0)?0.0:((rgb.g>1.0)?1.0:rgb.g);		   \n"+
                    "	 rgb.b = (rgb.b<0.0)?0.0:((rgb.b>1.0)?1.0:rgb.b);		   \n"+

//                    "       rgb = yuv * mat3( 1,        0,       1.13983,\n"+
//                    "                    1,    -0.39465,  -0.58060,\n"+
//                    "                 1, 2.03211,     0 ); \n"+
                    "       if (SampleType == 1) {                      \n"+
                    "           gl_FragColor = vec4(rgb, texture2D(SamplerA, tc).r);\n"+
//                    "           gl_FragColor = vec4(rgb, 1);            \n"+
//                    "gl_FragColor = vec4(1.0,0.0,0.0,texture2D(SamplerA, tc).r);\n"+
                    "       } else {                                    \n"+
                    "           gl_FragColor = vec4(rgb, 1);            \n"+
//                    "gl_FragColor = vec4(0.0,0.0,1.0,1);\n"+
                    "       }                                           \n"+
//                    "   }                                               \n"+
                    "}                                                  \n";

    private static final String YUV420toRGB444PlusGainFSPCode =
        "varying lowp vec2 tc;                              \n"+
        "uniform int SampleType;                            \n"+//0 = rgb, 1 = yuv球面， 2 = yuv平面
        "uniform sampler2D SamplerY;                        \n"+
        "uniform sampler2D SamplerU;                        \n"+
        "uniform sampler2D SamplerV;                        \n"+
        "uniform sampler2D SamplerA;                        \n"+
        "uniform sampler2D TurnY;                           \n"+
        "uniform sampler2D TurnU;                           \n"+
        "uniform sampler2D TurnV;                           \n"+
        "void main(void)                                    \n"+
        "{                                                  \n"+
        "   if (SampleType == 0) {                          \n"+
        "       gl_FragColor = vec4(texture2D(SamplerY, tc).r,\n"+
        "                           texture2D(SamplerY, tc).g,\n"+
        "                           texture2D(SamplerY, tc).b, 1);\n"+
        "   } else {                                        \n"+
        "       mediump vec3 yuv;                           \n"+
        "       lowp vec3 rgb;                              \n"+
        "       vec4 gain_Y = texture2D(TurnY,tc);  \n"+
        "       vec4 gain_U = texture2D(TurnU,tc);  \n"+
        "       vec4 gain_V = texture2D(TurnV,tc);  \n"+
        "       yuv.x = texture2D(SamplerY, tc).r + (gain_Y.r-0.5)*0.75;          \n"+
        "       yuv.y = texture2D(SamplerU, tc).r - 0.5 + (gain_U.r-0.5)*0.75;    \n"+
        "       yuv.z = texture2D(SamplerV, tc).r - 0.5 + (gain_V.r-0.5)*0.75;    \n"+
//            "       rgb = mat3( 1,        1,       1,           \n"+
//            "                    0,    -0.39465,  2.03211,      \n"+
//            "                 1.13983, -0.58060,     0 ) * yuv; \n"+

        "	 rgb.r=yuv.x         +1.1403*yuv.z;						       \n"+
        "	 rgb.g=yuv.x-0.3939*yuv.y-0.5808*yuv.z;						       \n"+
        "	 rgb.b=yuv.x+2.0284*yuv.y;								       \n"+
        "	 rgb.r = (rgb.r<0.0)?0.0:((rgb.r>1.0)?1.0:rgb.r);	       \n"+
        "	 rgb.g = (rgb.g<0.0)?0.0:((rgb.g>1.0)?1.0:rgb.g);		   \n"+
        "	 rgb.b = (rgb.b<0.0)?0.0:((rgb.b>1.0)?1.0:rgb.b);		   \n"+

//                    "       rgb = yuv * mat3( 1,        0,       1.13983,\n"+
//                    "                    1,    -0.39465,  -0.58060,\n"+
//                    "                 1, 2.03211,     0 ); \n"+
            "       if (SampleType == 1) {                      \n"+
            "           gl_FragColor = vec4(rgb, texture2D(SamplerA, tc).r);\n"+
//                    "gl_FragColor = vec4(0.0,1.0,0.0,1.0);\n"+
            "       } else {                                    \n"+
            "           gl_FragColor = vec4(rgb, 1);            \n"+
//                    "gl_FragColor = vec4(0.0,1.0,0.0,1);\n"+
            "       }                                           \n"+
            "   }                                               \n"+
            "}                                                  \n";

    private static final String YUV420toRGB444PlusGain_VR_FSPCode =
        "varying lowp vec2 tc;                                      \n"+
        "uniform int SampleType;                                    \n"+//0 = rgb, 1 = yuv球面， 2 = yuv平面
        "uniform sampler2D SamplerY,SamplerU,SamplerV,SamplerA,TurnY,TurnU,TurnV; \n"+
        "void main()                                                \n"+
        "{                                                          \n"+
        "    float y,u,v,r,g,b;                                     \n"+
        "    vec4 colorO;                                           \n"+
        "    if (SampleType == 0) {                                 \n"+
        "        gl_FragColor = vec4(texture2D(SamplerY, tc).r,     \n"+
        "                            texture2D(SamplerY, tc).g,     \n"+
        "                            texture2D(SamplerY, tc).b, 1); \n"+
        "    } else {                                               \n"+
        "        vec4 colorY = texture2D(SamplerY,tc);              \n"+
        "        vec4 colorU = texture2D(SamplerU,tc);              \n"+
        "        vec4 colorV = texture2D(SamplerV,tc);              \n"+
        "        vec4 colorA = texture2D(SamplerA,tc);              \n"+
        "        vec4 gain_Y = texture2D(TurnY,tc);                 \n"+
        "        vec4 gain_U = texture2D(TurnU,tc);                 \n"+
        "        vec4 gain_V = texture2D(TurnV,tc);                 \n"+
        "	     y=colorY.r+(gain_Y.r-0.5)*0.75;     	            \n"+
        "	     u=colorU.r-0.5+(gain_U.r-0.5)*0.75;	            \n"+
        "	     v=colorV.r-0.5+(gain_V.r-0.5)*0.75;	            \n"+
//        "        y = texture2D(SamplerY,tc).r + texture2D(TurnY,tc).r-0.5;    \n"+
//        "        u = texture2D(SamplerU,tc).r-0.5 + texture2D(TurnU,tc).r-0.5;\n"+
//        "        v = texture2D(SamplerV,tc).r-0.5 + texture2D(TurnV,tc).r-0.5;\n"+
        "	     r=y         +1.1403*v;						        \n"+
        "	     g=y-0.3939*u-0.5808*v;						        \n"+
        "	     b=y+2.0284*u;								        \n"+
        "	     colorO.r = (r<0.0)?0.0:((r>1.0)?1.0:r);	        \n"+
        "	     colorO.g = (g<0.0)?0.0:((g>1.0)?1.0:g);		    \n"+
        "	     colorO.b = (b<0.0)?0.0:((b>1.0)?1.0:b);		    \n"+
        "	     colorO.a = texture2D(SamplerA,tc).r;		        \n"+
        "	     gl_FragColor = colorO;                             \n"+
        "    }                                                      \n"+
        "}                                                          \n";

    //创建shader程序的方法:调用时不需传参
    public static int buildProgram(){
        //加载顶点着色器
        int vertexShader = loadShader(GLES20.GL_VERTEX_SHADER,vertexShaderCode);
        //加载片源着色器fragment
        int fragmentShader;
        if (Define.isCmbExist){
//        fragmentShader = loadShader(GLES20.GL_FRAGMENT_SHADER,YUV420toRGB444JustGainFSPCode);
            fragmentShader = loadShader(GLES20.GL_FRAGMENT_SHADER, YUV420toRGB444PlusGain_VR_FSPCode);//支持调色
        }else {
            fragmentShader = loadShader(GLES20.GL_FRAGMENT_SHADER,fragmentShaderCode);//不支持调色
        }
        //创建着色器程序
        int programHandle = GLES20.glCreateProgram();
        //若成功
        if(programHandle !=0){
            //向程序中加入顶点着色器
            GLES20.glAttachShader(programHandle,vertexShader);
            //向程序中加入片元着色器
            GLES20.glAttachShader(programHandle,fragmentShader);
            //链接程序
            GLES20.glLinkProgram(programHandle);
            int[] linkStatus = new int[1];
            //获取program的链接情况
            GLES20.glGetProgramiv(programHandle,GLES20.GL_LINK_STATUS,linkStatus,0);
            //若链接失败则报错并删除程序
            if (linkStatus[0] != GLES20.GL_TRUE) {  //GLES20.GL_TRUE == 1
                Log.e("TGA","could not link program:");
                Log.e("TGA",GLES20.glGetProgramInfoLog(programHandle));
                GLES20.glDeleteProgram(programHandle);
                programHandle = 0;
            }
        }
        //释放资源
        GLES20.glDeleteShader(vertexShader);
        GLES20.glDeleteShader(fragmentShader);

        return programHandle;
    }
    //加载自定义shader的方法
    public static int loadShader(int shaderType,String shaderCode){
        //shader类型,自定义管线有vertexShader和fragmentShader两种
        // GLES20.GL_VERTEX_SHADER   GLES20.GL_FRAGMENT_SHADER;
        //shader的脚本字符串,管线的处理程序，GLSL语言

        //创建一个新shader
        int shader = GLES20.glCreateShader(shaderType);
        //若创建成功则加载shader
        if (shader!=0){
            //加载shader的源代码
            GLES20.glShaderSource(shader,shaderCode);
            //编译shader
            GLES20.glCompileShader(shader);
            //存放编译成功的shader数量的数组
            int[] compiled = new int[1];
            //获取shader的编译情况
            GLES20.glGetShaderiv(shader,GLES20.GL_COMPILE_STATUS,compiled,0);
            if (compiled[0]==0){
                //若编译失败则显示日志并删除shader
                Log.e("ES20_ERROR", "Could not compile shader: " + shaderType);
                Log.e("ES20_ERROR",shaderCode);
                Log.e("ES20_ERROR",GLES20.glGetShaderInfoLog(shader));
                GLES20.glDeleteShader(shader);
                shader = 0;
            }
        }
        return shader;
    }
    //创建shader程序的方法:调用需传入参数
    public static int buildProgram(String vertexSource,String fragmentSource){
        //加载顶点着色器
        int vertexShader = loadShader(GLES20.GL_VERTEX_SHADER,vertexSource);
        if (vertexShader == 0){
            return 0;
        }
        //加载片元着色器
        int pixelShader = loadShader(GLES20.GL_FRAGMENT_SHADER,fragmentSource);
        if (pixelShader == 0){
            return 0;
        }
        //创建程序
        int program = GLES20.glCreateProgram();
        //若创建成功，则向程序中加入顶点着色器和片元着色器
        if (program!=0){
            //向程序中加入顶点着色器
            GLES20.glAttachShader(program,vertexShader);
            //checkGlError("glAttachShader");
            //向程序中加入片元着色器
            GLES20.glAttachShader(program,pixelShader);
            //checkGlError("glAttachShader");
            //链接程序
            GLES20.glLinkProgram(program);
            //存放链接成功program数量的数组
            int[] linkStatus = new int[1];
            //获取链接情况
            GLES20.glGetProgramiv(program,GLES20.GL_LINK_STATUS,linkStatus,0);
            if (linkStatus[0]!=GLES20.GL_TRUE){
                Log.e("ES20_ERROR", "Could not link program: ");
                Log.e("ES20_ERROR", GLES20.glGetProgramInfoLog(program));
                GLES20.glDeleteProgram(program);
                program = 0;
            }
        }
        GLES20.glDeleteShader(vertexShader);
        GLES20.glDeleteShader(pixelShader);
        return program;
    }
    public static void checkGlError(String op){
        int error;
        while ((error = GLES20.glGetError())!=GLES20.GL_NO_ERROR){
            Log.e("ES20_ERROR", op + ": glError " + error);
            throw new RuntimeException(op + ": glError " + error);
        }
    }
    //从sh脚本中加载shader内容的方法
    public static String loadFromAssetsFile(String fname, Resources r){
        String result = null;
        try{
            InputStream in = r.getAssets().open(fname);
            int ch =0 ;
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            while ((ch = in.read())!=-1){
                baos.write(ch);
            }
            byte[] buff = baos.toByteArray();
            baos.close();
            in.close();
            result = new String(buff,"UTF-8");
            result = result.replaceAll("\\r\\n","\n");
        }catch (Exception e){
            e.printStackTrace();
        }
        return result;
    }
    //从安卓Resource的raw文件夹根据资源Id读取文本文件
    public static String readTextFileFromResource(Context context , int resourceId){
        StringBuilder body = new StringBuilder();
        InputStream inputStream = null;
        InputStreamReader reader = null;
        BufferedReader bufferedReader = null;
        try {
            inputStream = context.getResources().openRawResource(resourceId);
            reader = new InputStreamReader(inputStream);
            bufferedReader = new BufferedReader(reader);
            String line = null;
            while ((line = bufferedReader.readLine()) != null){
                body.append(line + "\n");
            }
        }catch (Exception e){
            Log.d(TAG, "readTextFileFromResource: read ResourceFile has error");
        }finally {
            try {
                if (bufferedReader != null){
                    bufferedReader.close();
                }
                if (reader != null){
                    reader.close();
                }
                if (inputStream != null){
                    inputStream.close();
                }
            }catch (Exception e){}
        }
        return body.toString();
    }

}

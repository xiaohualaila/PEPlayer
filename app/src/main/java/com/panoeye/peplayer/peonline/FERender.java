package com.panoeye.peplayer.peonline;

import android.content.Context;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import panoeye.pelibrary.Define;
import panoeye.pelibrary.PEDecodeBuffer;
import panoeye.pelibrary.PEError;
import panoeye.pelibrary.PEMatrix;
import panoeye.pelibrary.PESquare;
import panoeye.pelibrary.PEVideoTexture;
import panoeye.pelibrary.Shader;

/**
 * Created by tir on 2017/4/17.
 */

public class FERender implements GLSurfaceView.Renderer {
    float[] mProjMatrix = new float[16];
    float[] mModeViewMatrix = new float[16];
    float[] mMatrix = new float[16];
    final Float PI = 3.141592f;
    private int width = 1;
    private int height = 1;
    private float fovy;//fovy 是 y 轴的 field of view 值，也就是视角大小。
    float[] glRotateXAixs = new float[3];
    float[] glRotateYAixs = new float[3];
    float[] glRotateZAixs = new float[3];
    float[] glTranslate = new float[3];
    int mProgram;// 自定义渲染管线着色器程序id
    int muMVPMatrixHandle;// 总变换矩阵引用
    int maPositionHandle; // 顶点位置属性引用
    int shaderTexture;

    private int tex_y;
    private int tex_u;
    private int tex_v;

    String mVertexShader;// 顶点着色器
    String mFragmentShader;// 片元着色器
    Integer radius =450 ;
    int[][] textures;
    Ball []ball;
    PESquare[] squares;
    PEDecodeBuffer decodeBuffer;
    public FERender(PEDecodeBuffer decodeBuffer,Context context){
        mVertexShader = Shader.loadFromAssetsFile("vertex.sh",context.getResources());
        mFragmentShader = Shader.loadFromAssetsFile("frag.sh",context.getResources());

        ball = new Ball[2];
        squares = new PESquare[2];
        textures = new int[Define.cameraCount][3];
        this.decodeBuffer = decodeBuffer;

        if (Define.isHalfBall == true){
            ball[0] = new Ball(-90,0);
            squares[0]=new PESquare();
        }else {
            ball[0] = new Ball(-90,0);
            ball[1] = new Ball(0,90);
            squares[0]=new PESquare();
            squares[1]=new PESquare();
        }

    }
    public void useProgram(){
        mProgram = Shader.buildProgram(mVertexShader, mFragmentShader);
        GLES20.glUseProgram(mProgram);
        //获取程序中顶点位置属性引用
        maPositionHandle = GLES20.glGetAttribLocation(mProgram, "vPosition");
        //获取程序中总变换矩阵引用
        muMVPMatrixHandle = GLES20.glGetUniformLocation(mProgram, "uMVPMatrix");

        tex_y          = GLES20.glGetUniformLocation(mProgram,"SamplerY");
        tex_u          = GLES20.glGetUniformLocation(mProgram,"SamplerU");
        tex_v          = GLES20.glGetUniformLocation(mProgram,"SamplerV");

        shaderTexture  = GLES20.glGetAttribLocation(mProgram,"a_texCoord");


    }

    private static final String TAG = "FERender";
    @Override
    public void onDrawFrame(GL10 gl10) {
        glResize();
        Define.openglFrameRate ++;
//        Log.d(TAG, "onDrawFrame:openglFrameRate-->"+Define.openglFrameRate);
        try {
            glDraw(Define.x,Define.y,Define.z);//opengl绘制
        }catch (PEError e){
            Log.d("onDrawFrame", e.name+e.msg);
        }

    }
    public void glDraw(float x,float y,float z)throws PEError{
        //清除深度缓冲与颜色缓冲
        GLES20.glClear(GLES20.GL_DEPTH_BUFFER_BIT | GLES20.GL_COLOR_BUFFER_BIT);
        GLES20.glClearColor(0.8f,0.8f,0.8f,1.0f);

        if (Define.isStretch == false){
            float yd =(float)(y/180*PI);
            float xd =(float)(x/180*PI);
            float zd =(float)(z/180*PI);
            float zp = 0;
            if (Define.isInBall == true){
                zp = 0;
            }else {
                zp = -450;
            }
            PEMatrix.makeVec3(glTranslate, 0, 0,zp);
            PEMatrix.makeUnit(mModeViewMatrix);
            PEMatrix.translate(mModeViewMatrix, glTranslate);

            PEMatrix.rotate(mModeViewMatrix,yd,glRotateXAixs);
            PEMatrix.rotate(mModeViewMatrix,-xd,glRotateZAixs);
            float[] rollAxis = {(float)Math.sin(-xd),(float)Math.cos(-xd),0};
            PEMatrix.rotate(mModeViewMatrix,-zd,rollAxis);
        }else {
            PEMatrix.makeVec3(glTranslate, x, -y,-400);
            PEMatrix.makeUnit(mModeViewMatrix);
            PEMatrix.translate(mModeViewMatrix, glTranslate);
        }

        PEMatrix.calMulti(mMatrix, mProjMatrix, mModeViewMatrix);

        // 将最终变换矩阵传入shader程序
        GLES20.glUniformMatrix4fv(muMVPMatrixHandle, 1, false, mMatrix, 0);

        int count = 1;
        if (Define.isHalfBall == false){
            count = 2;
        }
        for (int i = 0 ;i<count;i++){

            do {
                if (textures[i] == null){
                    Log.e(TAG, "glDraw:textures[i] == null ");
                }
                decodeBuffer.undateIfNeed(i,textures[i]);
                PEVideoTexture videoTexture = (PEVideoTexture) decodeBuffer.textureDict.get(i);

                if (videoTexture == null){

                    break;
                }

                GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
                videoTexture.bindTextureY();
                GLES20.glUniform1i(tex_y, 0);

                GLES20.glActiveTexture(GLES20.GL_TEXTURE1);
                videoTexture.bindTextureU();
                GLES20.glUniform1i(tex_u, 1);

                GLES20.glActiveTexture(GLES20.GL_TEXTURE2);
                videoTexture.bindTextureV();
                GLES20.glUniform1i(tex_v, 2);
                /**
                 * glVertexAttribPointer()方法(同名c方法，作用类似)：告诉OpenGL该如何解析顶点数据（应用到逐个顶点属性上）：
                 * 六个参数依次为：1.指定我们要配置的属性，2.属性的大小，3.数据的类型，4.是否希望数据被标准化，5.步长(Stride)，6.顶点/纹理缓冲数据
                 */
                GLES20.glVertexAttribPointer(shaderTexture,2,GLES20.GL_FLOAT,false,0,ball[i].mTextureBuffer);
//                GLES20.glVertexAttribPointer(shaderTexture,2,GLES20.GL_FLOAT,false,0,squares[i].mTextureBuffer);
                GLES20.glEnableVertexAttribArray(shaderTexture);

                // 指定顶点位置数据
                if (Define.isStretch ==false){
                    GLES20.glVertexAttribPointer(maPositionHandle, 3, GLES20.GL_FLOAT, false, 0, ball[i].mVertexBuffer);
//                    GLES20.glVertexAttribPointer(maPositionHandle, 3, GLES20.GL_FLOAT, false, 0, squares[i].mVertexBuffer);
                }else {
                    if (Define.isFlip == false){
                        GLES20.glVertexAttribPointer(maPositionHandle, 2, GLES20.GL_FLOAT, false, 0, ball[i].vertexStretch);
//                        GLES20.glVertexAttribPointer(maPositionHandle, 3, GLES20.GL_FLOAT, false, 0, squares[i].mVertexBuffer);
                    }else {
                        GLES20.glVertexAttribPointer(maPositionHandle, 2, GLES20.GL_FLOAT, false, 0, ball[i].vertexStretchFlip);
//                        GLES20.glVertexAttribPointer(maPositionHandle, 3, GLES20.GL_FLOAT, false, 0, squares[i].mVertexBuffer);
                    }
                }

                // 允许顶点位置数据数组
                GLES20.glEnableVertexAttribArray(maPositionHandle);
                // 绘制球
                GLES20.glDrawArrays(GLES20.GL_TRIANGLES, 0, ball[i].vCount);
//                squares[0].draw(mMatrix);

                GLES20.glDisableVertexAttribArray(maPositionHandle);
                GLES20.glDisableVertexAttribArray(shaderTexture);

            }while (false);
        }


    }
    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        initTexture();
        GLES20.glEnable(GLES20.GL_DEPTH_TEST);
        GLES20.glDepthFunc(GLES20.GL_LEQUAL);
        //GLES20.glEnable(GLES20.GL_BLEND);
        GLES20.glEnable(GLES20.GL_TEXTURE_2D);
        //GLES20.glEnable(GLES20.GL_CULL_FACE);
    }

    @Override
    public void onSurfaceChanged(GL10 gl10,int width,int height) {
        PEMatrix.makeUnit(mProjMatrix);
        PEMatrix.makeUnit(mModeViewMatrix);
        PEMatrix.makeUnit(mMatrix);
        this.width = width;
        this.height = height;
        PEMatrix.makeVec3(glRotateXAixs, 1, 0, 0);
        PEMatrix.makeVec3(glRotateZAixs, 0, 0, 1);
        useProgram();
        glResize();
    }
    public void glResize(){
        GLES20.glViewport(0,0,width,height);
        this.fovy = Define.fovy;

        Float aspect ;
        aspect = (new Float(width))/(new Float(height));

        Float zNear = 0.1f;
        Float zFar = 2000.0f;

        Float top = zNear*(new Float(Math.tan(fovy*PI/360.0)));
        Float bottom = -top;
        Float left = bottom * aspect;
        Float right = top * aspect;

        PEMatrix.frustum(mProjMatrix,left,right,bottom,top,zNear,zFar);
    }
    public void initTexture(){
        for (int j = 0;j<Define.cameraCount;j++){
            GLES20.glGenTextures(3,textures[j],0);
            for (int i = 0;i<3;i++){
                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D,textures[j][i]);
                GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
                GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);

                GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
                GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);

                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D,0);
            }
        }
    }
}

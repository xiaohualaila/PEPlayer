package com.panoeye.peplayer.peonline;

import android.content.Context;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.Log;

import com.panoeye.peplayer.R;

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
 * Created by Administrator on 2017/11/13.
 * 渲染器类-->完成实际的绘图动作
 * 1-->建立一个OpenGL ES环境
 * 2-->定义形状
 * 3-->绘图形状
 * 4-->应用投影和相机视图
 * 5-->添加运动
 * 6-->触摸事件响应
 */

public class RawRenderer implements GLSurfaceView.Renderer {
    float[] mModelMatrix = new float[16];     //模型矩阵
    float[] mViewMatrix = new float[16];      //视图矩阵
    float[] mProjectionMatrix = new float[16];//投影矩阵
    private final float[] mMVPMatrix = new float[16];//模型视图投影矩阵
    private float[] mRotationMatrix = new float[16];//旋转矩阵
    final Float PI = 3.141592f;
    private int width = 1;
    private int height = 1;
    private float fovy;//fovy 是 y 轴的 field of view 值，也就是视角大小。
    float[] glRotateXAixs = new float[3];
    float[] glRotateYAixs = new float[3];
    float[] glRotateZAixs = new float[3];
    float[] glTranslate = new float[3];
    private static final String TAG = "RawRenderer";
    String mVertexShader;  // 顶点着色器
    String mFragmentShader;// 片段着色器
    int mProgram;// 自定义渲染管线着色器程序id
    PESquare[] squares;
    int[][] textures;
    PEDecodeBuffer decodeBuffer;

    // 既然renderer的代码运行于主界面之外的单独线程中，你必须声明这个公开变量为volatile(挥发物;不稳定的),
    // 事实上不声明为volatile也可以。
    public volatile float mAngle;

    static final int COORDS_PER_VERTEX = 3;
    // 设置颜色（海蓝色），分别为red, green, blue 和alpha(透明度)的值
    float color[] = { 0.33371875f, 0.76953125f, 0.66665625f, 1.0f };
    private int mColorHandle;   //指向fragment shader的成员vColor的handle
    private int mPositionHandle; // 顶点位置属性引用
    private int mTexCoordHandle;
    private int mMVPMatrixHandle;// 总变换矩阵引用
    private int tex_y;
    private int tex_u;
    private int tex_v;
    private final int vertexCount = 6; // 顶点数量为4(=12/3)
    private final int vertexStride = COORDS_PER_VERTEX * 4; // 顶点之间的间距（步幅），其中每个顶点占12(=3*4)bytes
    public RawRenderer(PEDecodeBuffer decodeBuffer, Context context){
        mVertexShader = Shader.readTextFileFromResource(context, R.raw.vertex);
        mFragmentShader = Shader.readTextFileFromResource(context,R.raw.frag);
        this.decodeBuffer = decodeBuffer;
        squares = new PESquare[Define.cameraCount];
        textures = new int[Define.cameraCount][3];
        for (int i = 0;i<Define.cameraCount;i++){
            squares[i]=new PESquare();
        }
    }

    /**
     * onSurfaceCreated()方法
     * Called once to set up the view's OpenGL ES environment.
     * 仅调用一次，用于设置view的OpenGLES环境。
     */
    public void onSurfaceCreated(GL10 unused, EGLConfig config) {
        initTexture();
        useProgram();
//        GLES20.glEnable(GLES20.GL_DEPTH_TEST);
//        GLES20.glDepthFunc(GLES20.GL_LEQUAL);
//        //GLES20.glEnable(GLES20.GL_BLEND);
//        GLES20.glEnable(GLES20.GL_TEXTURE_2D);
//        //GLES20.glEnable(GLES20.GL_CULL_FACE);
    }

    /**
     * onDrawFrame()方法
     * Called for each redraw of the view.
     * 每次View被重绘时被调用。
     */
    public void onDrawFrame(GL10 unused) {
        Define.openglFrameRate +=1;
        glResize();
        glDraw(Define.x,Define.y,Define.z);
    }

 /**
  * onSurfaceChanged()方法
  * Called if the geometry of the view changes, for example when the device's screen orientation changes.
  * 如果view的几何形状发生变化了就调用，例如当竖屏变为横屏时。
 */
    public void onSurfaceChanged(GL10 unused, int width, int height) {
        //设置视口大小
        GLES20.glViewport(0, 0, width, height);
        //竖屏启动：竖屏：1080*1752;横屏：1920*912；
        //横屏启动：竖屏：1080*1776;横屏：1920*936。
        Log.d(TAG, "onSurfaceChanged: width:"+width+",height:"+height);
        this.width = width;
        this.height = height;
//        float ratio = (float) width / height;//缩小、放大比例
//        // 此投影矩阵在onDrawFrame()中将应用到对象的坐标
//        // frustumM()函数，对矩阵进行ratio倍投影变换；frustum（截头锥体,视锥）。
//        PEMatrix.frustumM(mProjectionMatrix, 0, ratio, -ratio, -1, 1, 3, 7);
//        //注: 只对你的对象应用一个投影变换一般会导制什么也看不到。通常，你必须也对其应用一个视口变换才能看到东西。
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
    public void useProgram() {
        mProgram = Shader.buildProgram(mVertexShader, mFragmentShader);
        GLES20.glUseProgram(mProgram);
        //获取程序中顶点位置、纹理位置的属性引用
        mPositionHandle = GLES20.glGetAttribLocation(mProgram, "vPosition");
        mTexCoordHandle  = GLES20.glGetAttribLocation(mProgram,"a_texCoord");
        //获取程序中总变换矩阵引用
        mMVPMatrixHandle = GLES20.glGetUniformLocation(mProgram, "uMVPMatrix");

        tex_y          = GLES20.glGetUniformLocation(mProgram,"SamplerY");
        tex_u          = GLES20.glGetUniformLocation(mProgram,"SamplerU");
        tex_v          = GLES20.glGetUniformLocation(mProgram,"SamplerV");
    }

    public void glResize(){
        this.fovy = Define.fovy;

        Float aspect = (new Float(width))/(new Float(height));

        Float zNear = 0.1f;
        Float zFar = 2000.0f;
        Float top = zNear*(new Float(Math.tan(fovy*PI/180.0)));
        Float bottom = -top;
        Float left = bottom * aspect;
        Float right = top * aspect;
//        Log.d(TAG, "glResize: top:"+top+",bottom:"+bottom+";left:"+left+",right:"+right);
        PEMatrix.frustum(mProjectionMatrix,left,right,bottom,top,zNear,zFar);//缩放

//        // PEMatrix.setLookAtM()函数，设置相机（即观测点）的位置(视图矩阵)
//        PEMatrix.setLookAtM(mViewMatrix, 0, 0, 0, -3, 0f, 0f, 0f, 0f, 1.0f, 0.0f);

//        // 计算投影和视图变换
//        // PEMatrix.multiplyMM()函数，对矩阵进行乘法运算（mMVPMatrix = mProjectionMatrix * mViewMatrix）。
//        PEMatrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mViewMatrix, 0);

//        //PEMatrix.setRotateM()函数，设置旋转矩阵。
//        PEMatrix.setRotateM(mRotationMatrix, 0, mAngle, 0, 0, -1.0f);
//
//        // 合并旋转矩阵到投影和相机视图矩阵
//        PEMatrix.multiplyMM(mMVPMatrix, 0, mRotationMatrix, 0, mMVPMatrix, 0);
    }

    public void glDraw(float x,float y,float z) { // 传入计算出的变换矩阵
        //清除深度缓冲与颜色缓冲
        GLES20.glClear(GLES20.GL_DEPTH_BUFFER_BIT | GLES20.GL_COLOR_BUFFER_BIT);
        GLES20.glClearColor(0.8f,0.8f,0.8f,1.0f);

//        if (Define.isStretch == false){
        float dx =x/150;
        float dy =y/120;
//            float zd =(float)(z/180*PI);
//            float zp = 0;
//            if (Define.isInBall == true){
//                zp = 0;
//            }else {
//                zp = -450;
//            }
        PEMatrix.makeVec3(glTranslate, dx, -dy,0);
        PEMatrix.makeUnit(mModelMatrix);
        PEMatrix.translate(mModelMatrix, glTranslate);//平移

//            PEMatrix.rotate(mModelMatrix,yd,glRotateXAxis);
//            PEMatrix.rotate(mModelMatrix,-xd,glRotateZAxis);
//            float[] rollAxis = {(float)Math.sin(-xd),(float)Math.cos(-xd),0};
//            PEMatrix.rotate(mModelMatrix,-zd,rollAxis);
//        }else {
//            PEMatrix.makeVec3(glTranslate, x, -y,-400);
//            PEMatrix.makeUnit(mModelMatrix);
//            PEMatrix.translate(mModelMatrix, glTranslate);
//        }
        PEMatrix.calMulti(mMVPMatrix, mProjectionMatrix, mModelMatrix);

//        // 获取指向fragment shader的成员vColor的handle
//        mColorHandle = GLES20.glGetUniformLocation(mProgram, "vColor");
//        // 设置三角形的颜色
//        GLES20.glUniform4fv(mColorHandle, 1, color, 0);

        // 将投影和视口变换传递给着色器shader
        GLES20.glUniformMatrix4fv(mMVPMatrixHandle, 1, false, mMVPMatrix, 0);

//        int count = 2;
//        if (Define.isHalfBall == false){
//            count = 2;
//        }
        for (int i = 0 ;i<Define.cameraCount;i++){
            if (Define.cameraCount == 5){
                squares[i].mVertexBuffer5.position(18*i);
            }else if (Define.cameraCount == 8){
                squares[i].mVertexBuffer8.position(18*i);
            }else {
                squares[i].mVertexBuffer.position(18*i);
            }
            do {
                if (textures[i] == null){
                    Log.e(TAG, "glDraw:textures[i]: == null ");
                }
                if (decodeBuffer == null){
                    Log.e(TAG, "glDraw:decodeBuffer: == null ");
                }
                try {
                    decodeBuffer.undateIfNeed(i,textures[i]);
                } catch (PEError peError) {
                    peError.printStackTrace();
                }
//                FEVideoTexture videoTexture = decodeBuffer[i].texture;
                PEVideoTexture videoTexture = null;//得到PEVideoTexture类对象
                try {
                    videoTexture = (PEVideoTexture)decodeBuffer.textureDict.get(i);
                } catch (PEError peError) {
                    peError.printStackTrace();
                }

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


                // 允许顶点位置数据数组
                GLES20.glEnableVertexAttribArray(mPositionHandle);
                GLES20.glEnableVertexAttribArray(mTexCoordHandle);
                // 指定顶点位置数据
//                if (Define.isStretch ==false){
////                    GLES20.glVertexAttribPointer(mPositionHandle, 3, GLES20.GL_FLOAT, false, 0, ball[i].mVertexBuffer);
//                }else {
//                    if (Define.isFlip == false){
////                        GLES20.glVertexAttribPointer(maPositionHandle, 2, GLES20.GL_FLOAT, false, 0, ball[i].vertexStretch);
//                    }else {
////                        GLES20.glVertexAttribPointer(maPositionHandle, 2, GLES20.GL_FLOAT, false, 0, ball[i].vertexStretchFlip);
//                    }
//                }
                //GLES20.glVertexAttribPointer(mTexCoordHandle,2,GLES20.GL_FLOAT,false,0,ball[i].mTextureBuffer);

                if (Define.cameraCount == 5){
                    GLES20.glVertexAttribPointer(mPositionHandle, 3, GLES20.GL_FLOAT, false, vertexStride, squares[i].mVertexBuffer5);
                    GLES20.glVertexAttribPointer(mTexCoordHandle,2,GLES20.GL_FLOAT,false,0,squares[i].mTextureFlipBuffer5);
                }else if (Define.cameraCount == 8){
                    GLES20.glVertexAttribPointer(mPositionHandle, 3, GLES20.GL_FLOAT, false, vertexStride, squares[i].mVertexBuffer8);
                    GLES20.glVertexAttribPointer(mTexCoordHandle,2,GLES20.GL_FLOAT,false,0,squares[i].mTextureFlipBuffer8);
                }else {
                    GLES20.glVertexAttribPointer(mPositionHandle, 3, GLES20.GL_FLOAT, false, vertexStride, squares[i].mVertexBuffer);
                    GLES20.glVertexAttribPointer(mTexCoordHandle,2,GLES20.GL_FLOAT,false,0,squares[i].mTextureBuffer);
//                    GLES20.glVertexAttribPointer(mTexCoordHandle,2,GLES20.GL_FLOAT,false,0,squares[i].mTextureFlipBuffer);
                }

                // 绘制球
                //GLES20.glDrawArrays(GLES20.GL_TRIANGLES, 0, ball[i].vCount);
                // 绘制三角形// 用 glDrawElements 来绘制矩形，mVertexIndexBuffer 指定了顶点绘制顺序
                GLES20.glDrawArrays(GLES20.GL_TRIANGLES, 0, vertexCount);
//                GLES20.glDrawElements(GLES20.GL_TRIANGLES, squares[i].drawOrder.length,GLES20.GL_UNSIGNED_SHORT, squares[i].drawListBuffer);

                // 禁用这个指向三角形的顶点数组(注释无异)
//                GLES20.glDisableVertexAttribArray(mPositionHandle);
//                GLES20.glDisableVertexAttribArray(mTexCoordHandle);

            }while (false);
        }
    }

    public float getAngle() {
        return mAngle;
    }

    public void setAngle(float angle) {
        mAngle = angle;
    }
}

package panoeye.pelibrary;

import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.Log;

import java.nio.FloatBuffer;
import java.nio.IntBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by tir on 2016/12/16.
 * 渲染器类
 */

 public class PERenderer implements GLSurfaceView.Renderer{

    final Float PI = 3.141592f;
    FloatBuffer verCoordbuffer ;//顶点坐标
    FloatBuffer texCoordbuffer;//纹理坐标

    FloatBuffer VR_VERTEX_COORD;
    FloatBuffer TEXTURE_COORD;

    private int vboVer[][] ;//vbo 顶点缓冲区对象
    private int vboTex[][] ;

    float[] mProjMatrix = new float[16];
    float[] mModeViewMatrix = new float[16];
    float[] mMatrix = new float[16];
    float[] glTranslate = new float[3];
    float[] glRotateXAixs = new float[3];
    float[] glRotateYAixs = new float[3];
    float[] glRotateZAixs = new float[3];
    private int shaderVertex;
    private int shaderTexture;
    private int shaderMatrix;
    private int tex_type;
    private int tex_y;
    private int tex_u;
    private int tex_v;
    private int tex_a;
    private int program;

    int[][] textures ;

    IntBuffer renderBuffer,frameBuffer,displayTexture;

    private int width = 1;
    private int height = 1;
    private float fovy =45f;

    private boolean changeFlag = false ;

    PEBinFile binFile ;//bin文件
    PEDecodeBuffer decodeBuffer;
    Integer radius ;
    //构造函数
    public PERenderer(PEBinFile binFile, PEDecodeBuffer decodeBuffer)throws PEError{
        this.binFile = binFile;
        this.decodeBuffer = decodeBuffer;
        textures = new int[binFile.info.camCount][4];//int textures[8][4]

        vboVer = new int[binFile.info.camCount][1];
        vboTex = new int[binFile.info.camCount][1];

        displayTexture = IntBuffer.allocate(1);
        frameBuffer = IntBuffer.allocate(1);
        renderBuffer = IntBuffer.allocate(1);

        VR_VERTEX_COORD = FloatBufferInit.bufferUtilInit(PESquare.VR_VERTEX_COORD.length);
        VR_VERTEX_COORD.put(PESquare.VR_VERTEX_COORD).position(0);

        TEXTURE_COORD = FloatBufferInit.bufferUtilInit(PESquare.VR_TEXTURE_COORD.length);
        TEXTURE_COORD.put(PESquare.VR_TEXTURE_COORD).position(0);
    }

    private static final String TAG = "PERenderer";
    //仅调用一次，用于设置view的OpenGLES环境。
    public void onSurfaceCreated(GL10 gl, EGLConfig config){
        initVBO();
    }
    //初始化顶点缓冲对象
    void initVBO(){
        for (Integer cid:binFile.cidList){
            //从bin文件获取顶点坐标
            if (Define.flip == false){
                verCoordbuffer = binFile.camConfigs.get(cid).verCoordbuffer;
            }else {
                verCoordbuffer = binFile.camConfigs.get(cid).verCoordbufferFlip;
            }
            //从bin文件获取纹理坐标
            texCoordbuffer = binFile.camConfigs.get(cid).texCoordbuffer;

            GLES20.glGenBuffers(1,vboVer[cid],0);
            GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER,vboVer[cid][0]);
            GLES20.glBufferData(GLES20.GL_ARRAY_BUFFER,binFile.camConfigs.get(cid).verCoordLength*4,verCoordbuffer,GLES20.GL_STATIC_DRAW);
            GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER,0);

            GLES20.glGenBuffers(1,vboTex[cid],0);
            GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER,vboTex[cid][0]);
            GLES20.glBufferData(GLES20.GL_ARRAY_BUFFER,binFile.camConfigs.get(cid).texCoordLength*4,texCoordbuffer,GLES20.GL_STATIC_DRAW);
            GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER,0);
        }
    }

    //每次View被重绘时被调用。
    public void onDrawFrame(GL10 gl) {
//        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT|GLES20.GL_DEPTH_BUFFER_BIT);

        try {
            Define.openglFrameRate++;
            glDraw(Define.x,Define.y,Define.z);//opengl绘制
        }catch (PEError e){
            Log.d("onDrawFrame", e.name+e.msg);
        }
    }

    //如果view的几和形状发生变化了就调用，例如当竖屏变为横屏时。
    public void onSurfaceChanged(GL10 gl,int width,int height){
        this.width = width;
        this.height = height;

        if (changeFlag == false) {
            GLES20.glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
            radius = binFile.info.radius;

            PEMatrix.makeVec3(glRotateXAixs, 1, 0, 0);
            PEMatrix.makeVec3(glRotateZAixs, 0, 0, 1);

            GLES20.glDisable(GLES20.GL_DEPTH_TEST);
            GLES20.glBlendFunc(GLES20.GL_SRC_ALPHA, GLES20.GL_ONE_MINUS_SRC_ALPHA);
            GLES20.glEnable(GLES20.GL_BLEND);
            GLES20.glEnable(GLES20.GL_TEXTURE_2D);

            for (Integer cid : binFile.cidList) {
                GLES20.glGenTextures(4, textures[cid], 0);
                Size size = binFile.camConfigs.get(cid).fullSize;

                for (int i = 0; i < 4; i++) {
                    GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[cid][i]);
                    //set filtering
                    GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
                    GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_NEAREST);

                    GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
                    GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);

                    GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
                }

                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[cid][3]);

                GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, size.width, size.height, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, binFile.camConfigs.get(cid).alphaData);

                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
            }
            //创建纹理
            GLES20.glGenTextures(1, displayTexture);
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, displayTexture.get(0));
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_NEAREST);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_GENERATE_MIPMAP_HINT, GLES20.GL_TRUE);
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGB, width, height, 0, GLES20.GL_RGB, GLES20.GL_UNSIGNED_BYTE, null);
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
            //创建renderbuffer
            GLES20.glGenRenderbuffers(1, renderBuffer);
            GLES20.glBindRenderbuffer(GLES20.GL_RENDERBUFFER, renderBuffer.get(0));
            GLES20.glRenderbufferStorage(GLES20.GL_RENDERBUFFER, GLES20.GL_DEPTH_ATTACHMENT, width, height);
            GLES20.glBindRenderbuffer(GLES20.GL_RENDERBUFFER, 0);
            //创建framebuffer
            GLES20.glGenFramebuffers(1, frameBuffer);
            GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, frameBuffer.get(0));
            GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES20.GL_TEXTURE_2D, displayTexture.get(0), 0);
            GLES20.glFramebufferRenderbuffer(GLES20.GL_FRAMEBUFFER, GLES20.GL_DEPTH_ATTACHMENT, GLES20.GL_RENDERBUFFER, renderBuffer.get(0));
            GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);

            GLES20.glFinish();
            int status = GLES20.glCheckFramebufferStatus(GLES20.GL_FRAMEBUFFER);

            PEMatrix.makeUnit(mProjMatrix);
            PEMatrix.makeUnit(mModeViewMatrix);
            PEMatrix.makeUnit(mMatrix);

            useProgram();
        }
        glResize();
    }
    public void glResize(){
//        GLES20.glViewport(0,0,width,height);
        this.fovy = Define.fovy;

        Float aspect ;
        if (Define.showVR == true){
            aspect = (new Float(width/2))/(new Float(height));
        }else {
            aspect = (new Float(width))/(new Float(height));
        }

        Float zNear = 0.1f;
        Float zFar = 2000.0f;

        Float top = zNear*(new Float(Math.tan(fovy*PI/360.0)));
        Float bottom = -top;
        Float left = bottom * aspect;
        Float right = top * aspect;

        PEMatrix.frustum(mProjMatrix,left,right,bottom,top,zNear,zFar);
    }
    public void useProgram(){
        program = Shader.buildProgram();
        GLES20.glUseProgram(program);
        shaderVertex   = GLES20.glGetAttribLocation(program,"vPosition");
        shaderTexture  = GLES20.glGetAttribLocation(program,"a_texCoord");
        shaderMatrix   = GLES20.glGetUniformLocation(program,"glMatrix");
        tex_type       = GLES20.glGetUniformLocation(program,"SampleType");

        tex_y          = GLES20.glGetUniformLocation(program,"SamplerY");
        tex_u          = GLES20.glGetUniformLocation(program,"SamplerU");
        tex_v          = GLES20.glGetUniformLocation(program,"SamplerV");
        tex_a          = GLES20.glGetUniformLocation(program,"SamplerA");

    }

    public void glDraw(float x,float y,float z) throws PEError {
        glResize();
        if (Define.isNeedFilp == true){
            initVBO();
            Define.isNeedFilp = false;
        }
//        if (Define.showVR == true){
//            GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER,frameBuffer.get(0));
//        }else {
//            GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER,0);
//        }
        GLES20.glClearColor(0.8f,0.8f,0.8f,1.0f);
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);

        float yd = (y/180*PI);
        float xd = (x/180*PI);
        float zd = (z/180*PI);

        PEMatrix.makeUnit(mModeViewMatrix);
        if (Define.mode == Define.MODE_GLOBULAR) {
            PEMatrix.makeVec3(glTranslate, 0, 0, -(radius * 7 / 8));//-(radius * 7 / 8)
            PEMatrix.translate(mModeViewMatrix, glTranslate);
            GLES20.glEnable(GLES20.GL_BLEND);//启用图像融合

            PEMatrix.rotate(mModeViewMatrix,-yd,glRotateXAixs);
            PEMatrix.rotate(mModeViewMatrix,-xd,glRotateZAixs);
            float[] rollAxis = {(float)Math.sin(-xd),(float)Math.cos(-xd),0};
            PEMatrix.rotate(mModeViewMatrix,-zd,rollAxis);
        }
        if (Define.mode == Define.MODE_ORIGINAL) {
            PEMatrix.makeVec3(glTranslate, xd, -yd, 0);
            PEMatrix.translate(mModeViewMatrix, glTranslate);
            GLES20.glDisable(GLES20.GL_BLEND);//禁用图像融合
        }
//        PEMatrix.translate(mModeViewMatrix, glTranslate);
        PEMatrix.calMulti(mMatrix, mProjMatrix, mModeViewMatrix);
        if (Define.mode == Define.MODE_GLOBULAR) {
            for (Integer cid : binFile.cidList) {
                decodeBuffer.undateIfNeed(cid, textures[cid]);//更新解码好的缓存数据

                PEVideoTexture videoTexture = (PEVideoTexture) decodeBuffer.textureDict.get(cid);//得到PEVideoTexture类对象

                //更改一个uniform变量或数组的值，变量的值应该由getUniformLocation函数返回，
                GLES20.glUniform1i(tex_type, 1);
                //更改的变量   矩阵个数  是否要转置矩阵   指针
                GLES20.glUniformMatrix4fv(shaderMatrix, 1, false, mMatrix, 0);//更改矩阵或矩阵数组
                //选择由纹理函数进行修改的当前纹理单位
                GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
                videoTexture.bindTextureY();
                GLES20.glUniform1i(tex_y, 0);

                GLES20.glActiveTexture(GLES20.GL_TEXTURE1);
                videoTexture.bindTextureU();
                GLES20.glUniform1i(tex_u, 1);

                GLES20.glActiveTexture(GLES20.GL_TEXTURE2);
                videoTexture.bindTextureV();
                GLES20.glUniform1i(tex_v, 2);

                GLES20.glActiveTexture(GLES20.GL_TEXTURE3);
                videoTexture.bindTextureA();
                GLES20.glUniform1i(tex_a, 3);

                //绑定vbover
                GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, vboVer[cid][0]);

                //顶点位置坐标
                GLES20.glVertexAttribPointer(shaderVertex, 3, GLES20.GL_FLOAT, false, 0, 0);
                GLES20.glEnableVertexAttribArray(shaderVertex);

                //vboTex
                GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, vboTex[cid][0]);
                //纹理坐标
                GLES20.glVertexAttribPointer(shaderTexture, 2, GLES20.GL_FLOAT, false, 0, 0);
                GLES20.glEnableVertexAttribArray(shaderTexture);

                GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0);

                GLES20.glViewport(0,0,width/2,height);
                GLES20.glDrawArrays(GLES20.GL_TRIANGLES, 0, verCoordbuffer.capacity() / 3);
                GLES20.glViewport(width/2,0,width/2,height);
                GLES20.glDrawArrays(GLES20.GL_TRIANGLES, 0, verCoordbuffer.capacity() / 3);

                GLES20.glDisableVertexAttribArray(shaderVertex);
                GLES20.glDisableVertexAttribArray(shaderTexture);
            }
//            if (Define.showVR == true){
//                GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER,0);
//                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D,0);
//
//                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D,displayTexture.get(0));
//                GLES20.glGenerateMipmap(GLES20.GL_TEXTURE_2D);
//                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D,0);
//
//                GLES20.glUniform1i(tex_type,0);
//                GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
//                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D,displayTexture.get(0));
//                GLES20.glUniform1i(tex_y,0);
//                PEMatrix.makeUnit(mMatrix);
//
//                GLES20.glUniformMatrix4fv(shaderMatrix,1,false,mMatrix,0);
//
//                GLES20.glVertexAttribPointer(shaderVertex,2,GLES20.GL_FLOAT,false,0,VR_VERTEX_COORD);
//                GLES20.glEnableVertexAttribArray(shaderVertex);
//
//                GLES20.glVertexAttribPointer(shaderTexture,2,GLES20.GL_FLOAT,false,0,TEXTURE_COORD);
//                GLES20.glEnableVertexAttribArray(shaderTexture);
//
//                GLES20.glDrawArrays(GLES20.GL_TRIANGLES,0,12);
//                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D,0);
//            }
        }
        if (Define.mode == Define.MODE_ORIGINAL){
            int cameraCount = binFile.info.camCount;
            PESquare[] squares;
            squares = new PESquare[cameraCount];
            for (int i = 0;i<cameraCount;i++){
                squares[i]=new PESquare();
            }

            // 将投影和视口变换传递给着色器shader
            GLES20.glUniformMatrix4fv(shaderMatrix, 1, false, mMatrix, 0);

            for (int i = 0 ;i<cameraCount;i++){
                do {
                    if (textures[i] == null){
                        Log.e(TAG, "glDraw:textures[i]: == null ");
                    }
                    if (decodeBuffer == null){
                        Log.e(TAG, "glDraw:decodeBuffer: == null ");
                    }
                    decodeBuffer.undateIfNeed(i,textures[i]);
//                        PEVideoTexture videoTexture = decodeBuffer[i].texture;
                    PEVideoTexture videoTexture = (PEVideoTexture)decodeBuffer.textureDict.get(i);
                    if (videoTexture == null){
                        Log.e(TAG, "glDrawOrigin: videoTexture为空");
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

//                    GLES20.glActiveTexture(GLES20.GL_TEXTURE3);
//                    videoTexture.bindTextureA();
//                    GLES20.glUniform1i(tex_a, 3);

                    // 允许顶点位置数据数组
                    GLES20.glEnableVertexAttribArray(shaderVertex);
                    GLES20.glEnableVertexAttribArray(shaderTexture);

                    switch (cameraCount){
                        case 5:
                            squares[i].mVertexBuffer5.position(18*i);
                            GLES20.glVertexAttribPointer(shaderVertex, 3, GLES20.GL_FLOAT, false, 12, squares[i].mVertexBuffer5);
                            GLES20.glVertexAttribPointer(shaderTexture,2,GLES20.GL_FLOAT,false,0,squares[i].mTextureFlipBuffer5);
                            break;
                        case 8:
                            squares[i].mVertexBuffer8.position(18*i);
                            GLES20.glVertexAttribPointer(shaderVertex, 3, GLES20.GL_FLOAT, false, 12, squares[i].mVertexBuffer8);
                            GLES20.glVertexAttribPointer(shaderTexture,2,GLES20.GL_FLOAT,false,0,squares[i].mTextureFlipBuffer8);
                            break;
                        default:
                            squares[i].mVertexBuffer.position(18*i);
                            GLES20.glVertexAttribPointer(shaderVertex, 3, GLES20.GL_FLOAT, false, 12, squares[i].mVertexBuffer);
                            GLES20.glVertexAttribPointer(shaderTexture,2,GLES20.GL_FLOAT,false,0,squares[i].mTextureBuffer);
                            break;
                    }

                    GLES20.glDrawArrays(GLES20.GL_TRIANGLES, 0, 6);

                    // 禁用这个指向三角形的顶点数组(注释无异)
//                GLES20.glDisableVertexAttribArray(shaderVertex);
//                GLES20.glDisableVertexAttribArray(shaderTexture);

                }while (false);
            }
        }
    }
}


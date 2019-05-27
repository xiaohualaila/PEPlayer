package panoeye.pelibrary;

import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.Log;

import java.nio.FloatBuffer;
import java.nio.IntBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by Administrator on 2018/1/9.
 */

public class PERendererGain implements GLSurfaceView.Renderer {
    private static final String TAG = "PERendererOnline";
    int cameraNum;
    final Float PI = 3.141592f;
    FloatBuffer verCoordbuffer ;//顶点坐标
    FloatBuffer texCoordbuffer; //纹理坐标

    FloatBuffer VR_VERTEX_COORD;//VR模式顶点坐标
    FloatBuffer VR_TEXTURE_COORD;  //VR模式纹理坐标

    private int vboVer[][] ;//vbo 顶点缓冲区对象
    private int vboTex[][] ;

    float[] mProjMatrix = new float[16];
    float[] mModeViewMatrix = new float[16];
    float[] mMatrix = new float[16];
    float[] glTranslate = new float[3];
    float[] glRotateXAxis = new float[3];
    float[] glRotateZAxis = new float[3];
    private int shaderVertex;
    private int shaderTexture;
    private int shaderMatrix;
    private int tex_type;
    private int tex_y;
    private int tex_u;
    private int tex_v;
    private int tex_a;
    private int program;

    private int turn_y;
    private int turn_u;
    private int turn_v;

    int[][] textures ;
    int[][] texturesTurn;

    IntBuffer textureId, rboId, fboId;

    private int width = 1;
    private int height = 1;
    private float fovy =45f;

    private boolean changeFlag = false ;

    PEBinFile binFile ;//bin文件
    PEDecodeBuffer decodeBuffer;
    Integer radius ;

    //构造函数
    public PERendererGain(PEBinFile binFile, PEDecodeBuffer decodeBuffer)throws PEError {
        this.binFile = binFile;
        this.decodeBuffer = decodeBuffer;
        this.cameraNum = binFile.info.camCount;
        textures = new int[binFile.info.camCount][7];//int textures[8][4]
        texturesTurn = new int[binFile.info.camCount][3];//int textures[8][3]

        vboVer = new int[binFile.info.camCount][1];
        vboTex = new int[binFile.info.camCount][1];

        textureId = IntBuffer.allocate(1);
        fboId = IntBuffer.allocate(1);
        rboId = IntBuffer.allocate(1);
        //VR模式顶点坐标
        VR_VERTEX_COORD = FloatBufferInit.bufferUtilInit(PESquare.VR_VERTEX_COORD.length);
        VR_VERTEX_COORD.put(PESquare.VR_VERTEX_COORD).position(0);
        //VR模式纹理坐标
        VR_TEXTURE_COORD = FloatBufferInit.bufferUtilInit(PESquare.VR_TEXTURE_COORD.length);
        VR_TEXTURE_COORD.put(PESquare.VR_TEXTURE_COORD).position(0);
    }

    //仅调用一次，用于设置view的OpenGLES环境。
    public void onSurfaceCreated(GL10 gl, EGLConfig config){
        Log.d(TAG, "onSurfaceCreated: ");
//        GLES20.glClearColor(0.3f, 0.5f, 0.7f, 1.0f);
        GLES20.glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        initVBO();
//        initTexture();
//        initVR();
        useProgram();
//        float[] dst = new float[3];
//        int offset = 0;
//        for (int pointCount = 1;pointCount<10;pointCount++){
//            verCoordbuffer.get(dst,0,3);
//            offset = offset+3;
//            Log.d(TAG, "onSurfaceChanged: "+dst[0]+","+dst[1]+","+dst[2]);
//        }
    }

    //每次View被重绘时被调用。
    public void onDrawFrame(GL10 gl) {
//        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT|GLES20.GL_DEPTH_BUFFER_BIT);
        Define.openglFrameRate++;
        glResize();
        try {
            glDraw();//opengl绘制
        }catch (PEError e){
            Log.d("onDrawFrame", e.name+e.msg);
        }
    }

    //如果view的几和形状发生变化了就调用，例如当竖屏变为横屏时。
    public void onSurfaceChanged(GL10 gl,int width,int height){
        Log.d(TAG, "onSurfaceChanged: ");
        GLES20.glViewport(0,0,width,height);
        this.width = width;
        this.height = height;
//        glResize();
        if (changeFlag == false) {
            initTexture();
            initVR();
//            changeFlag = true;
        }

    }
    //初始化顶点缓冲对象
    void initVBO(){
        for (Integer cid:binFile.cidList){
//        for (int cid=0;cid<cameraNum;cid++){

            int channel;
            switch (binFile.info.camCount){
                case 5:
                    if (cid==1)
                        channel = 3;
                    else if (cid==3)
                        channel = 1;
                    else
                        channel = cid;
                    break;
                case 8:
                    if (cid>0 && cid<7)
                        channel = 7-cid;
                    else
                        channel = cid;
                    break;
                default:
                    channel = cid;
                    break;
            }

            //从bin文件获取顶点坐标
            if (Define.flip == false){
                verCoordbuffer = binFile.camConfigs.get(channel).verCoordbuffer;
            }else {
                verCoordbuffer = binFile.camConfigs.get(channel).verCoordbufferFlip;
            }
            //从bin文件获取纹理坐标
            texCoordbuffer = binFile.camConfigs.get(channel).texCoordbuffer;

            /**
             * 函数：void glGenBuffers(int n, int[] buffers, int offset)
             * 作用：向OpenGL ES申请开辟新的VBO，并通过buffers数组获取VBO handle，handle的类型为整型。
             * @param int n　　　　　　申请的VBO个数
             * @param int[] buffers　　用于存储VBO handle的数组
             * @param int offset　　　　buffers数组的偏移量，即从buffers的第offset个位置开始存储handle
             * 注意：需要满足 n + offset <= buffers.length
             */
            GLES20.glGenBuffers(1,vboVer[cid],0);
            /**
             * 函数：void glBindBuffer(int target, int buffer)
             * 作用：通过handle绑定指定的VBO，同一时间只能绑定一个同类型的VBO，只有当前被绑定的VBO才会被用户操作。通过绑定handle为0的VBO，可以取消对所有同类型VBO的绑定。
             * @param int target　　　　指定绑定的VBO类型，具体类型有GL_ARRAY_BUFFER（用于为顶点数组传值）和GL_ELEMENT_ARRAY_BUFFER（用于为索引数组传值）
             * @param int buffer　　　　指定绑定的VBO handle
             */
            GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER,vboVer[cid][0]);
            /**
             * 函数：void glBufferData(int target, int size, Buffer data, int usage)
             * 作用：将数据传递给当前绑定的VBO。
             * @param int target　　　　指定VBO类型，同上
             * @param int size　　　　　指定VBO的大小，单位为byte
             * @param Buffer data　　　指定需要传递的数据
             * @param int usage　　　　指定VBO的存储方式，例如GL_STATIC_DRAW或GL_DYNAMIC_DRAW
             */
            GLES20.glBufferData(GLES20.GL_ARRAY_BUFFER,binFile.camConfigs.get(channel).verCoordLength*4,verCoordbuffer,GLES20.GL_STATIC_DRAW);
            GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER,0);

            GLES20.glGenBuffers(1,vboTex[cid],0);
            GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER,vboTex[cid][0]);
            GLES20.glBufferData(GLES20.GL_ARRAY_BUFFER,binFile.camConfigs.get(channel).texCoordLength*4,texCoordbuffer,GLES20.GL_STATIC_DRAW);
            GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER,0);
        }
    }
    void initTexture(){
        PEMatrix.makeVec3(glRotateXAxis, 1, 0, 0);
        PEMatrix.makeVec3(glRotateZAxis, 0, 0, 1);

//        GLES20.glDisable(GLES20.GL_DEPTH_TEST);
        GLES20.glBlendFunc(GLES20.GL_SRC_ALPHA, GLES20.GL_ONE_MINUS_SRC_ALPHA);//设置图像融合参数
//        GLES20.glEnable(GLES20.GL_BLEND);//启用图像融合
//        GLES20.glEnable(GLES20.GL_TEXTURE_2D);

        for (Integer cid : binFile.cidList) {
//        for (int cid=0;cid<cameraNum;cid++){
            GLES20.glGenTextures(7, textures[cid], 0);
            GLES20.glGenTextures(3, texturesTurn[cid], 0);
            Size size = binFile.camConfigs.get(cid).fullSize;
            for (int i = 0; i < 7; i++) {
                Log.d(TAG, "initTexture: textures:"+textures[cid][i]);

                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[cid][i]);
                //set filtering
                GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
                GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_NEAREST);

                GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);//GL_CLAMP_TO_EDGE
                GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);

                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
            }

            int channel;
            switch (binFile.info.camCount){
                case 5:
                    if (cid==1)
                        channel = 3;
                    else if (cid==3)
                        channel = 1;
                    else
                        channel = cid;
                    break;
                case 8:
                    if (cid>0 && cid<7)
                        channel = 7-cid;
                    else
                        channel = cid;
                    break;
                default:
                    channel = cid;
                    break;
            }

            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[cid][3]);
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, size.width, size.height, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, binFile.camConfigs.get(channel).alphaData);
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);

            if (Define.isCmbExist){
//            PEColorTune.undateIfNeedTurn7(cid,textures[cid]);//绑定调色数据
                PEColorTune.undateIfNeedTurn(cid,texturesTurn[cid]);//绑定调色数据

                for (int i = 0; i < 3; i++) {
                    Log.e(TAG, "initTexture: texturesTurn:"+texturesTurn[cid][i]);

                    GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texturesTurn[cid][i]);
                    //glTexParameteri():纹理过滤函数
                    GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
                    GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_NEAREST);

                    GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_REPEAT);//GL_CLAMP_TO_EDGE
                    GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_REPEAT);//GL_NONE

                    GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
                }
            }
        }

//        PEMatrix.makeUnit(mModeViewMatrix);
//        PEMatrix.makeUnit(mProjMatrix);
//        PEMatrix.makeUnit(mMatrix);
    }
    void initVR(){
        //创建纹理
        GLES20.glGenTextures(1, textureId);
        int a= textureId.get(0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId.get(0));
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR_MIPMAP_LINEAR);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_GENERATE_MIPMAP_HINT, GLES20.GL_TRUE);
        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGB, width, height, 0, GLES20.GL_RGB, GLES20.GL_UNSIGNED_BYTE, null);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        //创建renderBuffer
        GLES20.glGenRenderbuffers(1, rboId);
        int b=rboId.get(0);
        GLES20.glBindRenderbuffer(GLES20.GL_RENDERBUFFER, rboId.get(0));
        GLES20.glRenderbufferStorage(GLES20.GL_RENDERBUFFER, GLES20.GL_DEPTH_COMPONENT, width, height);//allocate a memory space for rboId
        GLES20.glBindRenderbuffer(GLES20.GL_RENDERBUFFER, 0);
        //创建Frame Buffer Object (FBO)
        GLES20.glGenFramebuffers(1, fboId);
        int c=fboId.get(0);
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, fboId.get(0));
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES20.GL_TEXTURE_2D, textureId.get(0), 0);//switch 2D texture objects
        GLES20.glFramebufferRenderbuffer(GLES20.GL_FRAMEBUFFER, GLES20.GL_DEPTH_ATTACHMENT, GLES20.GL_RENDERBUFFER, rboId.get(0));       //switch rboId objects.
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);

//        GLES20.glFinish();
    }
    void useProgram(){
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

        turn_y          = GLES20.glGetUniformLocation(program,"TurnY");
        turn_u          = GLES20.glGetUniformLocation(program,"TurnU");
        turn_v          = GLES20.glGetUniformLocation(program,"TurnV");
    }

    public void glDraw() throws PEError {
        if (Define.isNeedFilp == true){
            initVBO();
            Define.isNeedFilp = false;
        }

        if (Define.showVR == true){
            GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, fboId.get(0));
        }else {
            GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER,0);
        }

        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
//        GLES20.glClearColor(0.8f,0.8f,0.8f,1.0f);

        float xd = (Define.x/180*PI);
        float yd = (Define.y/180*PI);
        float zd = (Define.z/180*PI);

        PEMatrix.makeUnit(mModeViewMatrix);

        if (Define.mode == Define.MODE_GLOBULAR){
            radius = binFile.info.radius;
            radius = 450;
            PEMatrix.makeVec3(glTranslate, 0, 0, -(radius * 7 / 8));//-(radius * 7 / 8)
            PEMatrix.translate(mModeViewMatrix, glTranslate);   //平移

            PEMatrix.rotate(mModeViewMatrix,-yd, glRotateXAxis);//旋转
            PEMatrix.rotate(mModeViewMatrix,-xd, glRotateZAxis);
            float[] rollAxis = {(float)Math.sin(-xd),(float)Math.cos(-xd),0};
            PEMatrix.rotate(mModeViewMatrix,-zd,rollAxis);

            GLES20.glEnable(GLES20.GL_BLEND);//启用图像融合
        }
        if (Define.mode == Define.MODE_ORIGINAL){
            PEMatrix.makeVec3(glTranslate, xd, -yd, 0);
            PEMatrix.translate(mModeViewMatrix, glTranslate);

            GLES20.glDisable(GLES20.GL_BLEND);//禁用图像融合
        }

        PEMatrix.calMulti(mMatrix, mProjMatrix, mModeViewMatrix);//矩阵乘运算

        PESquare[] squares;
        squares = new PESquare[cameraNum];
        for (int i = 0;i<cameraNum;i++){
            squares[i]=new PESquare();
        }
        if (Define.mode == Define.MODE_ORIGINAL){
            GLES20.glUniform1i(tex_type, 2);
            // 将投影和视口变换传递给着色器shader
            GLES20.glUniformMatrix4fv(shaderMatrix, 1, false, mMatrix, 0);
        }

        for (int cid : binFile.cidList){
            decodeBuffer.undateIfNeed(cid, textures[cid]);//更新解码好的缓存数据
//                 PEVideoTexture videoTexture;
//                try {
//                    videoTexture =(PEVideoTexture)decodeBuffer.textureDict.get(cid);//得到PEVideoTexture类对象
//                 }catch (PEError e){
//                    Log.d(TAG, "glDraw: videoTexture为空");
//                }
            //选择由纹理函数进行修改的当前纹理单位
            GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
//                        videoTexture.bindTextureY();
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[cid][0]);
            GLES20.glUniform1i(tex_y, 0);

            GLES20.glActiveTexture(GLES20.GL_TEXTURE1);
//                        videoTexture.bindTextureU();
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[cid][1]);
            GLES20.glUniform1i(tex_u, 1);

            GLES20.glActiveTexture(GLES20.GL_TEXTURE2);
//                        videoTexture.bindTextureV();
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[cid][2]);
            GLES20.glUniform1i(tex_v, 2);

            GLES20.glActiveTexture(GLES20.GL_TEXTURE3);
//                        videoTexture.bindTextureA();
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[cid][3]);
            GLES20.glUniform1i(tex_a, 3);

            if (Define.isCmbExist){
                if (PEColorTune.updateGainList[cid] == 0){
//                PEColorTune.undateIfNeedTurn7(cid,textures[cid]);//更新调色数据
                    PEColorTune.undateIfNeedTurn(cid,texturesTurn[cid]);//更新调色数据
                    PEColorTune.updateGainList[cid] = cid+1;
                }

//            if (PEColorTune.colorTurnFlag){
//                PEColorTune.undateIfNeedTurn(cid,textures[cid]);
//            }

                GLES20.glActiveTexture(GLES20.GL_TEXTURE4);
//            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[cid][4]);
                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texturesTurn[cid][0]);
                GLES20.glUniform1i(turn_y, 4);

                GLES20.glActiveTexture(GLES20.GL_TEXTURE5);
//            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[cid][5]);
                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texturesTurn[cid][1]);
                GLES20.glUniform1i(turn_u, 5);

                GLES20.glActiveTexture(GLES20.GL_TEXTURE6);
//            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[cid][6]);
                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texturesTurn[cid][2]);
                GLES20.glUniform1i(turn_v, 6);
            }

            if (Define.mode == Define.MODE_GLOBULAR){
                drawGlobular(cid);
            }
            if (Define.mode == Define.MODE_ORIGINAL){
                drawOriginal(cid,squares);
            }
        }
        if (Define.showVR == true){
            GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER,0);
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D,0);

            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId.get(0));
            GLES20.glGenerateMipmap(GLES20.GL_TEXTURE_2D);
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D,0);


            GLES20.glUniform1i(tex_type,0);
            GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId.get(0));
            GLES20.glUniform1i(tex_y,0);
            PEMatrix.makeUnit(mMatrix);

            GLES20.glUniformMatrix4fv(shaderMatrix,1,false,mMatrix,0);

            GLES20.glVertexAttribPointer(shaderVertex,2,GLES20.GL_FLOAT,false,0,VR_VERTEX_COORD);
            GLES20.glEnableVertexAttribArray(shaderVertex);

            GLES20.glVertexAttribPointer(shaderTexture,2,GLES20.GL_FLOAT,false,0,VR_TEXTURE_COORD);
            GLES20.glEnableVertexAttribArray(shaderTexture);

            GLES20.glDrawArrays(GLES20.GL_TRIANGLES,0,12);
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D,0);
        }
    }

    private void drawOriginal(int i,PESquare squares[]) {
//        GLES20.glUniform1i(tex_type, 2);
        do {
            switch (cameraNum){
                case 5:
                    squares[i].mVertexBuffer5.position(18*i);
                    GLES20.glVertexAttribPointer(shaderVertex, 3,GLES20.GL_FLOAT,false, 12,squares[i].mVertexBuffer5);
                    GLES20.glVertexAttribPointer(shaderTexture,2,GLES20.GL_FLOAT,false,0,squares[i].mTextureFlipBuffer5);
                    break;
                case 8:
                    squares[i].mVertexBuffer8.position(18*i);
                    GLES20.glVertexAttribPointer(shaderVertex, 3,GLES20.GL_FLOAT,false,12,squares[i].mVertexBuffer8);
                    GLES20.glVertexAttribPointer(shaderTexture,2,GLES20.GL_FLOAT,false,0,squares[i].mTextureFlipBuffer8);
                    break;
                default:
                    squares[i].mVertexBuffer.position(18*i);
                    GLES20.glVertexAttribPointer(shaderVertex, 3,GLES20.GL_FLOAT,false,12,squares[i].mVertexBuffer);
                    GLES20.glVertexAttribPointer(shaderTexture,2,GLES20.GL_FLOAT,false,0,squares[i].mTextureBuffer);
                    break;
            }
//                    // 允许顶点位置数据数组
            GLES20.glEnableVertexAttribArray(shaderVertex);
            GLES20.glEnableVertexAttribArray(shaderTexture);

            GLES20.glDrawArrays(GLES20.GL_TRIANGLES, 0, 6);

            // 禁用这个指向三角形的顶点数组(注释无异)
            GLES20.glDisableVertexAttribArray(shaderVertex);
            GLES20.glDisableVertexAttribArray(shaderTexture);
        }while (false);
    }

    private void drawGlobular(int cid) {
        //给uniform变量传值，变量的值应该由getUniformLocation函数返回，
        GLES20.glUniform1i(tex_type, 1);
        //给uniform变量传矩阵：参数依次为：更改的变量，矩阵个数，是否要转置矩阵，矩阵的指针，偏移。
        GLES20.glUniformMatrix4fv(shaderMatrix, 1, false, mMatrix, 0);//更改矩阵或矩阵数组

        //绑定vboVer,顶点位置坐标
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER,vboVer[cid][0]);
        GLES20.glVertexAttribPointer(shaderVertex, 3, GLES20.GL_FLOAT, false, 0, 0);
        GLES20.glEnableVertexAttribArray(shaderVertex);
        //绑定vboTex,纹理位置坐标
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER,vboTex[cid][0]);
        GLES20.glVertexAttribPointer(shaderTexture, 2, GLES20.GL_FLOAT, false, 0, 0);
        GLES20.glEnableVertexAttribArray(shaderTexture);
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0);

        GLES20.glViewport(0,0,width,height);
        GLES20.glDrawArrays(GLES20.GL_TRIANGLES, 0, verCoordbuffer.capacity() / 3);

        GLES20.glDisableVertexAttribArray(shaderVertex);
        GLES20.glDisableVertexAttribArray(shaderTexture);
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

}

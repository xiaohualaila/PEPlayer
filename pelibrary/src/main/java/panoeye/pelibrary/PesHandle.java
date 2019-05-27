package panoeye.pelibrary;

import android.opengl.GLSurfaceView;

/**
 * Created by tir on 2017/3/20.
 */

public class PesHandle{
    PESPano pano;
    PEDecodeBuffer decodeBuffer;
    PESDecodeManager decodeManager;//解码管理类
    public Float duration;//pse文件第一个I帧到最后一帧之间的最小时间间隔，单位为秒。
    public int camCount;//镜头模组个数
    private static final String TAG = "PesHandle";
    //构造函数
    PesHandle(String name) throws PEError {
        pano = new PESPano(name);
        decodeBuffer = new PEDecodeBuffer(pano.binFile);
        decodeManager = new PESDecodeManager(pano,decodeBuffer);
        duration = pano.minDuration;//接收pse文件第一个I帧到最后一帧之间的最小时间间隔，单位为秒。
        camCount = pano.binFile.info.camCount;//接收镜头模组个数
    }
    //设置视图
    void setView(GLSurfaceView view)throws PEError{
        PERenderer myRenderer = new PERenderer(pano.binFile,decodeBuffer);//构造渲染器myRenderer
        view.setEGLContextClientVersion(2);//选择OpenGL ES 2.0的环境
        view.setRenderer(myRenderer);//给view设置渲染器
    }

}

package panoeye.pelibrary;

import android.opengl.GLSurfaceView;

/**
 * Created by jun on 2017/12/14.
 */

public class PerHandle{
//    Pano pano;
    PEBinFile binFile;
    PEDecodeBuffer decodeBuffer;
    PEDecodeManager decodeManager;//解码管理类
    public Float duration;//per文件第一个I帧到最后一帧之间的最小时间间隔，单位为秒。
    public int camCount;//镜头模组个数
    private static final String TAG = "PerHandle";
    PerHandle(String name) throws PEError {
        PERPano pano = new PERPano(name);
        binFile= pano.binFile;
        decodeBuffer = new PEDecodeBuffer(binFile);
        decodeManager = new PEDecodeManager(pano,decodeBuffer);
        duration = pano.minDuration;//接收per文件第一个I帧到最后一帧之间的最小时间间隔，单位为秒。
        camCount = binFile.info.camCount;//接收镜头模组个数
    }
    //设置视图
    void setView(GLSurfaceView view)throws PEError{
//        PERenderer myRenderer = new PERenderer(binFile,decodeBuffer);//构造渲染器myRenderer
//        PERendererOnline myRenderer = new PERendererOnline(binFile,decodeBuffer);
        PERendererGain myRenderer = new PERendererGain(binFile,decodeBuffer);
        view.setEGLContextClientVersion(2);//选择OpenGL ES 2.0的环境
        view.setRenderer(myRenderer);//给view设置渲染器
//        PESurfaceView view = new PESurfaceView(this,pano,decodeBuffer);
    }
}

//package panoeye.pelibrary;
//
///**
// * Created by Administrator on 2018/1/24.
// */
//
//public abstract class PanoHandle {
//    //    Pano pano;
//    PEBinFile binFile = null;
//    PEDecodeBuffer decodeBuffer = null;
////    PEDecodeManager decodeManager;//解码管理类
//    Float duration = null;//per文件第一个I帧到最后一帧之间的最小时间间隔，单位为秒。
//    int camCount = 0;//镜头模组个数
////    PanoHandle(PanoType panoType, String name) throws PEError {
////        if (panoType == PanoType.PER){
////            PERPano pano = new PERPano(name);
////            binFile= pano.binFile;
////            decodeBuffer = new PEDecodeBuffer(binFile);
////            decodeManager = new PEDecodeManager(pano,decodeBuffer);
////            duration = pano.minDuration;//接收pes文件第一个I帧到最后一帧之间的最小时间间隔，单位为秒。
////        }else {
////            PESPano pano = new PESPano(name);
////            binFile = pano.binFile;
////            decodeBuffer = new PEDecodeBuffer(binFile);
////            PESDecodeManager decodeManager = new PESDecodeManager(pano,decodeBuffer);
////            duration = pano.minDuration;//接收pes文件第一个I帧到最后一帧之间的最小时间间隔，单位为秒。
////        }
////        camCount = binFile.info.camCount;//接收镜头模组个数
////    }
////    //设置视图
////    void setView(GLSurfaceView view)throws PEError
////    {
////        PERenderer myRenderer = new PERenderer(binFile,decodeBuffer);//构造渲染器myRenderer
////        view.setEGLContextClientVersion(2);//选择OpenGL ES 2.0的环境
////        view.setRenderer(myRenderer);//给view设置渲染器
//////        PESurfaceView view = new PESurfaceView(this,pano,decodeBuffer);
////    }
//}

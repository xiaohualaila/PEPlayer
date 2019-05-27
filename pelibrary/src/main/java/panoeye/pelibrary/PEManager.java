package panoeye.pelibrary;

import android.opengl.GLSurfaceView;

/**
 * Created by tir on 2017/3/15.
 */

public class PEManager{
//    public static PanoHandle buildPano(PanoType panoType, String name)throws PEError{
//        return new PanoHandle(panoType,name);
//    }
    public static PerHandle buildPer(String name)throws PEError{
        return new PerHandle(name);
    }
    public static PesHandle buildPes(String name)throws PEError{
        return new PesHandle(name);
    }
    public static void setView(PerHandle handle, GLSurfaceView view)throws PEError{
        handle.setView(view);
    }
    public static void setView(PesHandle handle, GLSurfaceView view)throws PEError{
        handle.setView(view);
    }
    public static void start(PerHandle handle)throws PEError{
        handle.decodeManager.start();
    }
    public static void pause(PerHandle handle) throws PEError{
        handle.decodeManager.pause();
    }
    public static void restart(PerHandle handle) throws PEError{
        handle.decodeManager.restart();
    }
    public static void reView(PerHandle handle) throws PEError{
        handle.decodeManager.reView();
    }
    public static void invalidate(PerHandle handle) throws PEError{
        handle.decodeManager.invalidate();
    }
    public static void jumpToTime(PerHandle handle, float time)throws PEError{
        handle.decodeManager.setJumpToTime(time);
    }
    public static PEDecodeManagerStatus getStatus(PerHandle handle) throws PEError{
        return handle.decodeManager.status.get();
    }
    public static Float getTimeStamp(PerHandle handle){
        return handle.decodeManager.timeline.get();
    }
    public static void setVrMode(){
        Define.showVR = true;
    }
    //设置显示模式
    public static void setNormal(){
        Define.showVR = false;
    }
    public static void setFlip(boolean ze){
        if (ze == true){
            Define.flip = true;
        }else {
            Define.flip = false;
        }
        Define.isNeedFilp = true;
    }

    public static void start(PesHandle handle)throws PEError{
        handle.decodeManager.start();
    }
    public static void pause(PesHandle handle) throws PEError{
        handle.decodeManager.pause();
    }
    public static void restart(PesHandle handle) throws PEError{
        handle.decodeManager.restart();
    }
    public static void reView(PesHandle handle) throws PEError{
        handle.decodeManager.reView();
    }
    public static void invalidate(PesHandle handle) throws PEError{
        handle.decodeManager.invalidate();
    }
    public static void jumpToTime(PesHandle handle, float time)throws PEError{
        handle.decodeManager.setJumpToTime(time);
    }
    public static PEDecodeManagerStatus getStatus(PesHandle handle) throws PEError{
        return handle.decodeManager.status.get();
    }
    public static Float getTimeStamp(PesHandle handle){
        return handle.decodeManager.timeline.get();
    }
}

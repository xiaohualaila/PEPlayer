package com.panoeye.peplayer.peonline;

import android.content.Context;
import android.opengl.GLSurfaceView;

import panoeye.pelibrary.PEBinFile;
import panoeye.pelibrary.PEDecodeBuffer;
import panoeye.pelibrary.PEError;
import panoeye.pelibrary.PERendererOnline;

/**
 * Created by tir on 2017/4/17.
 */

public class FEDraw {
    GLSurfaceView view;
    PEDecodeBuffer decodeBuffer;
    Context context;
    PEBinFile binFile;
    public FEDraw(GLSurfaceView view,PEDecodeBuffer decodeBuffer,Context context){
        this.view = view;
        this.decodeBuffer = decodeBuffer;
        this.context = context;
    }
    public FEDraw(GLSurfaceView view,PEDecodeBuffer decodeBuffer,PEBinFile binFile){
        this.view = view;
        this.decodeBuffer = decodeBuffer;
        this.binFile = binFile;
    }
    public void drawRaw(){
//        FERender render = new FERender(decodeBuffer,context);
        RawRenderer render = new RawRenderer(decodeBuffer,context);
        view.setEGLContextClientVersion(2);
        view.setRenderer(render);
    }
    public void drawFishEye(){
        FERender render = new FERender(decodeBuffer,context);
        view.setEGLContextClientVersion(2);
        view.setRenderer(render);
    }
    public void drawPano(){
        PERendererOnline render = null;//构造渲染器myRenderer
//        PERendererGain render = null;//构造渲染器myRenderer
        try {
            render = new PERendererOnline(binFile,decodeBuffer);
//            render = new PERendererGain(binFile,decodeBuffer);
        } catch (PEError peError) {
            peError.printStackTrace();
        }
        view.setEGLContextClientVersion(2);
        view.setRenderer(render);
    }
}

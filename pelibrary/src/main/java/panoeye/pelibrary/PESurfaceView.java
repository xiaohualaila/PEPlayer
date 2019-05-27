package panoeye.pelibrary;

import android.content.Context;
import android.opengl.GLSurfaceView;


/**
 * Created by tir on 2016/12/27.
 */

 public class PESurfaceView extends GLSurfaceView{
     private PERenderer myRenderer;
     public PESurfaceView(Context context, Pano pano, PEDecodeBuffer decodeBuffer){
        super(context);
        try {
            myRenderer = new PERenderer(pano.binFile,decodeBuffer);
            setEGLContextClientVersion(2);
            setRenderer(myRenderer);
        }catch (PEError e){
            e.printStackTrace();
        }
    }
}

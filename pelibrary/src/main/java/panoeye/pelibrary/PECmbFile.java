package panoeye.pelibrary;

import android.util.Log;

import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.nio.FloatBuffer;

/**
 * Created by Administrator on 2018/3/23.
 */

public class PECmbFile {
    private static final String TAG = "PECmbFile";
    int num,col,row;

    ByteBuffer cmb_mask[];
    FloatBuffer map_x[];
    FloatBuffer map_y[];

    public PECmbFile(String cmbPath)throws PEError{
//        java.io.PEFile cmbFile = new java.io.PEFile(cmbPath);
//        if (cmbFile.exists()) {
            RandomAccessFile pCmbFile = PEFile.fopen(cmbPath);//打开bin文件
            readPECmbFileInfo(pCmbFile);
//        }else {
//            Log.e(TAG, "PECmbFile: 找不到cmb调色文件！");
//        }
    }

    void readPECmbFileInfo(RandomAccessFile fp)throws PEError{
        num = PEFile.freadInteger(fp); //8
        col = PEFile.freadInteger(fp); //320
        row = PEFile.freadInteger(fp); //240
        cmb_mask = new ByteBuffer[num];
        map_x = new FloatBuffer[num];
        map_y = new FloatBuffer[num];
        for (int i = 0; i < num; i++){
            Log.e(TAG, "readPECmbFileInfo: i:"+i);
            cmb_mask[i] = PEFile.freadByteBuffer(fp,col*row);//读取row*col个字节//76800
            map_x[i] = PEFile.freadFloatBuffer(fp,col*row);//读取row*col个Float类型数据//76800*4
            map_y[i] = PEFile.freadFloatBuffer(fp,col*row);//读取row*col个Float类型数据//76800*4
        }
    }
}

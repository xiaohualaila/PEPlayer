package panoeye.pelibrary;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

/**
 * Created by tir on 2016/12/18.
 * 本类封装了Float类型缓冲区的初始化操作
 */

 public class FloatBufferInit {
    //缓冲区单元初始化
     public static FloatBuffer bufferUtilInit(Integer size){
         //分配size*4大小的缓冲区单元
        FloatBuffer buffer = ByteBuffer.allocateDirect(size*4).order(ByteOrder.nativeOrder()).asFloatBuffer();
        return buffer;
    }
    //将arr中的数据写入缓冲区floatBuffer
     static void putFloat(FloatBuffer floatBuffer,float arr){
        floatBuffer.put(arr);
    }
    //将arr[]中的数据写入缓冲区floatBuffer
     static void putFloatArray(FloatBuffer floatBuffer,float arr[]){
        floatBuffer.put(arr);
    }
    //将缓冲区floatBuffer的位置置零
     static void bufferPositionToZero(FloatBuffer floatBuffer){
        floatBuffer.position(0);
    }

}

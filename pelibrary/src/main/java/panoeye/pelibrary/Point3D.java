package panoeye.pelibrary;



import java.nio.FloatBuffer;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by tir on 2016/12/8.
 */
//三维点类
 class Point3D {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
   //构造函数
     Point3D(float x,float y,float z){
        this.x = x;
        this.y = y;
        this.z = z;
    }
   //默认构造函数
     Point3D(){}

     FloatBuffer toFloatBuffer(){
        FloatBuffer tt = FloatBufferInit.bufferUtilInit(3);
        FloatBufferInit.putFloat(tt,x);
        FloatBufferInit.putFloat(tt,y);
        FloatBufferInit.putFloat(tt,z);
        return tt;
    }
    //将一个三维点封装成列表的形式并返回
     List<Float> toList(){
        List<Float> list = new ArrayList<>();
        list.add(x);
        list.add(y);
        list.add(z);
        return list;
    }
    //将一个三维点的z坐标取负，然后封装成列表的形式并返回
     List<Float> toListZ(){
        List<Float> list = new ArrayList<>();
        list.add(x);
        list.add(y);
        list.add(-z);
        return list;
    }

}

package panoeye.pelibrary;


import java.nio.FloatBuffer;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by tir on 2016/12/8.
 */
//二维点类
 class Point2D {
     float x = 0.0f;
     float y = 0.0f;
    //构造函数
     Point2D(float x, float y){
        this.x = x;
        this.y = y;
    }
    //默认构造函数
     Point2D(){}

     FloatBuffer toFloatBuffer() {
        FloatBuffer tt = FloatBufferInit.bufferUtilInit(2);
        FloatBufferInit.putFloat(tt,x);
        FloatBufferInit.putFloat(tt,y);
        return tt;
    }
    //将一个二维点封装成列表的形式并返回
     List<Float> toList(){
        List<Float> list = new ArrayList<>();
        list.add(x);
        list.add(y);
        return list;
    }
}

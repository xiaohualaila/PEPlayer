package panoeye.pelibrary;

import java.io.RandomAccessFile;
import java.nio.FloatBuffer;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by tir on 2016/12/8.
 */

 class Triangle2D {
    Point2D a = new Point2D();
    Point2D b = new Point2D();
    Point2D c = new Point2D();
    //构造函数
     Triangle2D(Point2D a,Point2D b,Point2D c){
        this.a = a;
        this.b = b;
        this.c = c;
    }
     FloatBuffer toFloatBuffer(){
        FloatBuffer ret = FloatBufferInit.bufferUtilInit(6);
        ret.put(a.toFloatBuffer());
        ret.put(b.toFloatBuffer());
        ret.put(c.toFloatBuffer());

        return ret;
    }
    //将一个二维点封装成列表的形式并返回
     List<Float> toList(){
        List<Float> ret = new ArrayList<>();
        ret.addAll(a.toList());//addAll()函数将点追加到ret中
        ret.addAll(b.toList());
        ret.addAll(c.toList());
        return ret;
    }
   //读取24个字节，返回一个2维三角形的顶点类对象
     static Triangle2D read(RandomAccessFile fp) throws PEError{
        float ax = PEFile.freadFloat(fp);
        float bx = PEFile.freadFloat(fp);
        float cx = PEFile.freadFloat(fp);
        float ay = PEFile.freadFloat(fp);
        float by = PEFile.freadFloat(fp);
        float cy = PEFile.freadFloat(fp);
        Point2D pa = new Point2D(ax,ay);
        Point2D pb = new Point2D(bx,by);
        Point2D pc = new Point2D(cx,cy);
        return new Triangle2D(pa,pb,pc);
    }



}

package panoeye.pelibrary;


import java.io.RandomAccessFile;
import java.nio.FloatBuffer;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by tir on 2016/12/8.
 */

 class Triangle3D {
    Point3D a = new Point3D();
    Point3D b = new Point3D();
    Point3D c = new Point3D();

     Triangle3D(Point3D a,Point3D b,Point3D c){
        this.a = a;
        this.b = b;
        this.c = c;
    }
   //将一个三维点封装成列表的形式并返回
     List<Float> toList(){
        List<Float> ret = new ArrayList<>();
        ret.addAll(a.toList());//addAll()函数将点追加到ret中
        ret.addAll(b.toList());
        ret.addAll(c.toList());
        return ret;
    }
   //将一个三维点的z坐标取负，然后封装成列表的形式并返回
     List<Float> toListZ(){
        List<Float> ret = new ArrayList<>();
        ret.addAll(a.toListZ());//addAll()函数将点追加到ret中
        ret.addAll(b.toListZ());
        ret.addAll(c.toListZ());
        return ret;
    }
     FloatBuffer toFloatBuffer(){
        FloatBuffer ret;
        ret = FloatBufferInit.bufferUtilInit(9);
        ret.put(a.toFloatBuffer());
        ret.put(b.toFloatBuffer());
        ret.put(c.toFloatBuffer());
        return ret;
    }

   //读取36个字节，返回一个3维三角形的顶点类对象
     static Triangle3D read(RandomAccessFile fp) throws PEError{
        float ax = PEFile.freadFloat(fp);
        float bx = PEFile.freadFloat(fp);
        float cx = PEFile.freadFloat(fp);
        float ay = PEFile.freadFloat(fp);
        float by = PEFile.freadFloat(fp);
        float cy = PEFile.freadFloat(fp);
        float az = PEFile.freadFloat(fp);
        float bz = PEFile.freadFloat(fp);
        float cz = PEFile.freadFloat(fp);
        Point3D pa = new Point3D(ax,ay,az);
        Point3D pb = new Point3D(bx,by,bz);
        Point3D pc = new Point3D(cx,cy,cz);
        return new Triangle3D(pa,pb,pc);
    }
}

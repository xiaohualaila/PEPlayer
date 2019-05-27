package panoeye.pelibrary;



import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.nio.FloatBuffer;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by tir on 2016/12/8.
 */

 public class PECameraConfig {
    Integer cid = 0;//cameraID
    public Size fullSize;//主码流
    Size subSize;//子码流
    Float lambda = 0.0f;//表达式
    public ByteBuffer alphaData;//透明度
    public FloatBuffer verCoordbuffer ;//顶点坐标
    public FloatBuffer texCoordbuffer ;//纹理坐标
    public int verCoordLength;//顶点坐标长度
    public int texCoordLength;//纹理坐标长度
    public FloatBuffer verCoordbufferFlip;//顶点坐标反转
    //默认构造函数
    PECameraConfig(){}

     static PECameraConfig read(RandomAccessFile fp) throws PEError{
         PECameraConfig cam = new PECameraConfig();//初始化
         cam.cid = PEFile.freadInteger(fp);//读取4个字节
         cam.lambda = PEFile.freadFloat(fp);//读取4个字节
         PEFile.fseek(fp,4, FileSeekFlag.CUR);//fp右移跳过4个字节
         Integer row = PEFile.freadInteger(fp);//读取4个字节//width=960
         Integer col = PEFile.freadInteger(fp);//读取4个字节//height=1280
         cam.fullSize = new Size(col,row);//设置主码流尺寸大小1280*960
         PEFile.fseek(fp,568,FileSeekFlag.CUR);//fp右移跳过568个字节
         cam.alphaData = PEFile.freadByteBuffer(fp,row*col);//读取row*col个字节//1228800

         Integer triCount = PEFile.freadInteger(fp);//读取4个字节//triCount=294
/*
         cam.verCoord = FloatBufferInit.bufferUtilInit(triCount * 9);
         cam.texCoord = FloatBufferInit.bufferUtilInit(triCount * 6);*/

         List<Float> texCoord = new ArrayList<>();//纹理坐标
         List<Float> verCoord = new ArrayList<>();//顶点坐标(半球挂着）
         List<Float> verCoordFlip = new ArrayList<>();//顶点坐标反转(半球放着)

         Integer seekCount = PEFile.freadInteger(fp);//读取4个字节//112
         for (Integer i = 0; i < triCount; i++) {
             Triangle2D.read(fp);//读取24个字节但没接收返回值，相当于跳过24个字节
             Triangle2D texture = Triangle2D.read(fp);//texture 结构、纹理
             Triangle3D sphere = Triangle3D.read(fp); //sphere  范围、球体
             texCoord.addAll(texture.toList());//addAll()函数将三角形顶点追加到texCoord中
             verCoord.addAll(sphere.toListZ());
             verCoordFlip.addAll(sphere.toList());
         }
/*
         FloatBufferInit.bufferPositionToZero(cam.verCoord);
         FloatBufferInit.bufferPositionToZero(cam.texCoord);*/

         float[] gg = new float[texCoord.size()];
         for (int i=0;i<gg.length;i++) {
             gg[i] = texCoord.get(i);//从列表中取出数据
         }

         float[] jj = new float[verCoord.size()];
         for (int i=0;i<jj.length;i++) {
             jj[i] = verCoord.get(i);
         }

         float[] jjFlip = new float[verCoordFlip.size()];
         for (int i=0;i<jjFlip.length;i++) {
             jjFlip[i] = verCoordFlip.get(i);
         }

         cam.texCoordLength = gg.length;//gg.length=1764(294*6)
         cam.texCoordbuffer = FloatBufferInit.bufferUtilInit(gg.length);//缓冲区初始化
         FloatBufferInit.putFloatArray(cam.texCoordbuffer,gg);//将gg中的数据写入缓冲区cam.texCoordbuffer
         FloatBufferInit.bufferPositionToZero(cam.texCoordbuffer);//将缓冲区cam.texCoordbuffer的位置置零

         cam.verCoordLength = jj.length;//jj.length=2646(294*9)
         cam.verCoordbuffer = FloatBufferInit.bufferUtilInit(jj.length);
         FloatBufferInit.putFloatArray(cam.verCoordbuffer,jj);
         FloatBufferInit.bufferPositionToZero(cam.verCoordbuffer);

         cam.verCoordbufferFlip = FloatBufferInit.bufferUtilInit(jjFlip.length);
         FloatBufferInit.putFloatArray(cam.verCoordbufferFlip,jjFlip);
         FloatBufferInit.bufferPositionToZero(cam.verCoordbufferFlip);

         PEFile.fseek(fp,84*seekCount,FileSeekFlag.CUR);//fp右移跳过84*seekCount个字节
         return cam;
    }
}

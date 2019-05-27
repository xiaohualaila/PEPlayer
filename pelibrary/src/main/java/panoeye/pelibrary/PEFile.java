package panoeye.pelibrary;


import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.util.Arrays;

/**
 * Created by tir on 2016/12/7.
 */

 class PEFile {

     static RandomAccessFile fopen(String name) throws PEError {
        try {
            return new RandomAccessFile(name,"r");
        }catch (IOException e){
            throw PEError.gen(PEErrorType.fileOpenFailed,name+"文件读取失败!");
        }
    }

     static RandomAccessFile fopenMode(String name , String mode) throws PEError {
        try {
            return new RandomAccessFile(name,mode);
        }catch (IOException e){
            throw PEError.gen(PEErrorType.fileReadFailed , name + "文件读写失败!");
        }
    }

     //设置文件指针fp的位置。SET:是绝对定位；CUR:是相对定位。
     static Long fseek(RandomAccessFile fp, Integer offset, FileSeekFlag flags) throws PEError {
        try{
            switch (flags) {
                case SET:
                    //seek() 用于设置文件指针位置，设置后会从当前指针的下一位读取到或写入，是绝对定位。
                    fp.seek(offset);
                    break;
                case CUR:
                    if (offset > 0) {//往后跳
                        //skipBytes() 是在当前位置的基础上再跳过指定字节数，是相对定位。
                        fp.skipBytes(offset);
                    }
                    if(offset <= 0){//往前跳
                        Long point = fp.getFilePointer();
                        fp.seek(point + offset.longValue());
                    }
                    break;
            }
            //getFilePointer() 返回当前文件指针的位置
            return fp.getFilePointer();
        }catch (IOException e){
            throw PEError.gen(PEErrorType.fileSeekFailed,"seekFile");
        }
    }
     //获取文件指针位置，并转化为Integer类型返回
     static Integer ftell(RandomAccessFile fp)throws PEError{
        try {
            return new Long(fp.getFilePointer()).intValue();
        }catch (IOException e){
            throw PEError.gen(PEErrorType.fileReadFailed,"getFilePointer");
        }
    }
    //获取文件长度，并转化为Integer类型返回
     static Integer flength(RandomAccessFile fp) throws PEError{
        try {
            return new Long(fp.length()).intValue();
        }catch (IOException e){
            throw PEError.gen(PEErrorType.fileReadFailed,"getFileLength");
        }
    }
    //通过文件指针fp读取count个字节
     static ByteBuffer freadByteBuffer(RandomAccessFile fp, Integer count) throws PEError {
        try{
//            if (count<0||count>1228800) {
//                Log.d("PEFile", "freadByteBuffer:count:" + count);
//            }
            ByteBuffer buf = ByteBuffer.allocate(count);//allocate() 方法分配一个具有指定大小的底层数组，并将它包装到一个缓冲区对象中 — 在本例中是buf。
            //readFully(byte[] b)方法是读取流上指定长度的字节数组，也就是说如果声明了长度为len的字节数组，readFully(byte[] b)方法只有读取len长度个字节的时候才返回，否则阻塞等待
            fp.readFully(buf.array());//array2()函数 将缓冲区对象拆封成byte[]类型对象
            return buf;
        }catch (IOException e){
            throw PEError.gen(PEErrorType.fileReadFailed,"ReadByteBuffer");
        }
    }

    //通过文件指针fp读取count个Float类型到FloatBuffer
    static FloatBuffer freadFloatBuffer(RandomAccessFile fp, Integer count) throws PEError {
        try{
            ByteBuffer bbf = ByteBuffer.allocate(count*4);
            bbf.order(ByteOrder.nativeOrder());//LITTLE_ENDIAN
            fp.readFully(bbf.array());
            FloatBuffer fbf = bbf.asFloatBuffer();
//            fbf.position(0);

//            float f[] = new float[count];
//            for (int i = 0;i<count;i++){
//                f[i] = Float.intBitsToFloat(bbf.getInt());
//            }
//            FloatBuffer fbf = FloatBuffer.wrap(f) ;
            return fbf;
        }catch (IOException e){
            throw PEError.gen(PEErrorType.fileReadFailed,"ReadByteBuffer");
        }
    }

    //效率低
    static FloatBuffer freadFloatBuffer2(RandomAccessFile fp, Integer count) throws PEError {
        float f[] = new float[count];
        for (int i = 0;i<count;i++){
            f[i] = freadFloat(fp);
        }
        FloatBuffer fbf = FloatBuffer.wrap(f) ;
        return fbf;
    }

     static void fwriteArray(RandomAccessFile fp , ByteBuffer buffer) throws PEError{
        try {
            fp.seek(fp.length());
            fp.write(buffer.array());
        }catch (IOException e){
            throw PEError.gen(PEErrorType.fileWriteFailed,"写入文件失败!");
        }
    }

    static void fwriteArray(RandomAccessFile fp , FloatBuffer fBuffer) throws PEError{
        try {

            ByteBuffer byteBuffer = ByteBuffer.allocate(fBuffer.capacity() * 4);
            byteBuffer.asFloatBuffer().put(fBuffer);
            byte[] byteArray = byteBuffer.array();
            fp.write(byteArray);
        }catch (IOException e){
            throw PEError.gen(PEErrorType.fileWriteFailed,"写入文件失败!");
        }
    }

     static void fwriteArray(RandomAccessFile fp , byte[] b) throws PEError{
        try {
            fp.write(b);
        }catch (IOException e){
            throw PEError.gen(PEErrorType.fileWriteFailed,"写入文件失败!");
        }
    }
     //通过文件指针fp读取4个字节，并作移位叠加的运算
     static Integer freadInteger(RandomAccessFile fp) throws  PEError{
         try {
             int ch4 = fp.read();//read()方法 读取一个字节的数据，并以int类型返回这个字节数据对应的byte类型的值(0-255)
             int ch3 = fp.read();
             int ch2 = fp.read();
             int ch1 = fp.read();
             //calSize();
             // << 左移运算符，num << 1,相当于num乘以2
             int a=((ch1 << 24) + (ch2 << 16) + (ch3 << 8) + (ch4 ));//450
             return a;
         }catch (IOException e){
             throw PEError.gen(PEErrorType.fileReadFailed,"ReadInteger");
         }

    }
    //读取4个字节
    static int freadInt(RandomAccessFile fp) throws  PEError{
        try {
            int a= fp.readInt();
            return a;
        }catch (IOException e){
            throw PEError.gen(PEErrorType.fileReadFailed,"ReadInt");
        }

    }

     //调用freadInteger()，并转化成Float类型返回
     static Float freadFloat(RandomAccessFile fp) throws PEError{
        try {
            //intBitsToFloat(int bits)方法 返回对应于给定位表示形式的 float 值。
            return Float.intBitsToFloat(freadInteger(fp));
        }catch (PEError e){
            throw e.gen(PEErrorType.fileReadFailed,"readFloat");
        }
    }

     //通过文件指针fp读取count个字节，并转化成String类型返回
     static String freadString(RandomAccessFile fp,Integer count)throws PEError{
        try {
            ByteBuffer buf=ByteBuffer.allocate(count);//allocate() 方法分配一个具有指定大小的底层数组，并将它包装到一个缓冲区对象中 — 在本例中是buf。
            //readFully(byte[] b)方法是读取流上指定长度的字节数组，也就是说如果声明了长度为len的字节数组，readFully(byte[] b)方法只有读取len长度个字节的时候才返回，否则阻塞等待
            fp.readFully(buf.array());//array2()函数 将缓冲区对象拆封成byte[]类型对象
            int bytelength=0;
            while (bytelength < 64) {
                if (buf.array()[bytelength] == 0) {//buf.array2()[]里存的是数据是ASCII的形式，0为NUL(null)，表示结束
                    break;
                }
                bytelength++;
            }
            byte[] ret = new byte[bytelength];
            for (int index = 0; index < bytelength; index++) {
                ret[index] = buf.array()[index];
            }
            String returnstring=new String(ret);//String(byte bytes[])将ASCII形式字符转变成字符串形式
            return returnstring;
        }catch (IOException e){
            throw PEError.gen(PEErrorType.fileReadFailed,"readString");
        }

    }
    //通过文件指针fp读取1个字节
    static byte freadByte(RandomAccessFile fp)throws PEError{
        try {
            return fp.readByte();//readByte()方法 读取一个字节的数据，并返回byte类型的值(0-255)
        }catch (IOException e){
            throw PEError.gen(PEErrorType.fileReadFailed,"readByte");
        }

    }
     //通过文件指针fp读取1个字节
     static Integer freadByte2(RandomAccessFile fp)throws PEError{
        try {
            return fp.read();//read()方法 读取一个字节的数据，并以int类型返回这个字节数据对应的byte类型的值(0-255)
        }catch (IOException e){
            throw PEError.gen(PEErrorType.fileReadFailed,"readByte");
        }

    }

     static void fclose(RandomAccessFile fp) throws  PEError{
        try {
            fp.close();
        }catch (IOException e){
            throw PEError.gen(PEErrorType.fileCloseFailed,"closeFile");
        }

    }
     //判断文件头部是否相符
     static boolean headEquals(RandomAccessFile fp)throws PEError{
        try {
            ByteBuffer temp = PEFile.freadByteBuffer(fp,4);//读取4个字节到temp
            byte[] tempp = temp.array();//取出temp中的数据(4个字节)取出并赋值给tempp
            byte[] tempt = {0,0,0,1};
            int ini = PEFile.freadByte2(fp);//读取1个字节并赋值给ini//255
            return (ini==0xff)&&Arrays.equals(tempp,tempt);//若ini值为255且两byte[]变量相等返回true
        }catch (PEError e){
            throw e.gen(PEErrorType.fileReadFailed,"headEquals");
        }

    }

    //判断文件头部是否相符//0x000001BB
    static boolean perHeadEquals(RandomAccessFile fp)throws PEError{
        try {
//            int sync_byte = PEFile.freadInteger(fp);//读取4个字节,并赋值给sync_byte//
//            return  (sync_byte == 0xbb010000);//-1157562368
            int sync_byte = PEFile.freadInt(fp);//读取4个字节,并赋值给sync_byte//
            return  (sync_byte == 0x01bb);//443
        }catch (PEError e){
            throw e.gen(PEErrorType.fileReadFailed,"headEquals");
        }

    }

     static String readFileLine(RandomAccessFile fp)throws PEError{
        try {
            return fp.readLine();
        }catch (IOException e){
            throw PEError.gen(PEErrorType.fileReadFailed,"文件读取行失败!");
        }
    }

     static String readFileContentStr(String name)throws PEError{
            String readOutStr = null;
            String line = null;
            RandomAccessFile fp = PEFile.fopen(name);
            while ((line = readFileLine(fp))!=null){
                readOutStr += line;
            }
            PEFile.fclose(fp);
        return readOutStr;
    }

     static void calSize() {
        System.out.println("Integer: " + Integer.SIZE/8);           // 4
        System.out.println("Short: " + Short.SIZE/8);               // 2
        System.out.println("Long: " + Long.SIZE/8);                 // 8
        System.out.println("Byte: " + Byte.SIZE/8);                 // 1
        System.out.println("Character: " + Character.SIZE/8);       // 2
        System.out.println("Float: " + Float.SIZE/8);               // 4
        System.out.println("Double: " + Double.SIZE/8);             // 8
    }
}

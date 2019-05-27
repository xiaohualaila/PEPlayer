package panoeye.pelibrary;

import android.opengl.GLES20;
import android.util.Log;

import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.nio.FloatBuffer;

/**
 * Created by Administrator on 2018/3/28.
 */

public class PEColorTune {

//    static PEBinFile binFile;
    PECmbFile cmbFile;
    static int cameraNum;
    static CoordMapProc coordMapProc;
    static CoordMapData mapData;
    static PgmImage[] gainY;
    static PgmImage[] gainU;
    static PgmImage[] gainV;
    private static final String TAG = "PEColorTune";
    public static boolean colorTurnFlag = false;//调色开关
    public static boolean isNeedOrgImg = false;//需要获取原始图像输入标志 isNeedColorTune
    public static boolean isGainReady = false; //gain完成标志
    public static int[] orgImgReadyList = {0,0,0,0,0,0,0,0};
    public static int[] updateGainList = {0,0,0,0,0,0,0,0};
    static int row = 240;
    static int col = 320;
    static ByteBuffer zero;
    static ByteBuffer gray;

    public class CoordMapProc{
        int num,row,col;
        PgmImage org_pgmY[];
        PgmImage org_pgmU[];
        PgmImage org_pgmV[];
        PgmImage org_mask[];

        PgmImage side_pgmY[];
        PgmImage side_pgmU[];
        PgmImage side_pgmV[];
    }

    public class CoordMapData{
        PgmImage gainY[];
        PgmImage gainU[];
        PgmImage gainV[];
        PgmImage cmb_mask[];
        FloatImage mapX[];
        FloatImage mapY[];
    }

    public class PgmImage{
        int row = 240;//height
        int col = 320;//width
//        int widthStep = 320;
        ByteBuffer data;
        ByteBuffer img[];
        byte[] array1;
//        byte[][] array2;
        public PgmImage(){
            data = ByteBuffer.allocate(col*row);
            img = new ByteBuffer[row];
            array1 = new byte[col*row];
            array1=data.array();
//            array2 = new byte[row][col];
            for (int r = 0; r<row; r++){
                data.position(r*col).limit((r+1)*col);
                img[r] = data.slice();
//                System.arraycopy(array1,r*col,array2[r],0,col);//对一维数组进行值拷贝
//                array2[r]= img[r].array();
//                Log.d(TAG, "PgmImage: pos:"+data.position()+",lim:"+data.limit()+",cap:"+data.capacity());
//                Log.d(TAG, "PgmImage: pos:"+img[r].position()+",lim:"+img[r].limit()+",cap:"+img[r].capacity());
//                Log.d(TAG, "PgmImage: len:"+ array2[r].length+",val:"+ array2[r].toString());
//                for (int c = 0; c<col; c++){
//                    array2[r][c] = array1[r*col+c];
//                }
            }
//            Log.e(TAG, "PgmImage: len:"+array1.length+",val:"+ array1.toString());
//            Log.e(TAG, "PgmImage: len:"+array2.length+",val:"+ array2.toString());
            data.rewind();//position置零
        }
    }
    public static class FloatImage{
        int row = 240;
        int col = 320;
//        int widthStep;
        FloatBuffer data;
        FloatBuffer img[];
        float[] array1;
        float[][] array2;
        public FloatImage(){
            data = FloatBuffer.allocate(col*row);
            img = new FloatBuffer[row];
            array1 = new float[col*row];
            array1=data.array();
//            array2 = new float[row][col];
            for (int r = 0; r<row; r++){
                data.position(r*col).limit((r+1)*col);
                img[r] = data.slice();
            }
            data.rewind();//position置零
        }
    }

    public PEColorTune(PECmbFile cmbF){
//        this.binFile = binFile;
        this.cameraNum = Define.cameraCount;
        this.cmbFile = cmbF;
//        String cmbPath = Define.PARAM_PATH+"E10010078.cmb";
//        try {
//            cmbFile = new PECmbFile(cmbPath);
//        } catch (PEError peError) {
//            peError.printStackTrace();
//        }
        coordMapProc = new CoordMapProc();
        coordMapProc.org_pgmY = new PgmImage[cameraNum];
        coordMapProc.org_pgmU = new PgmImage[cameraNum];
        coordMapProc.org_pgmV = new PgmImage[cameraNum];
        coordMapProc.org_mask = new PgmImage[cameraNum];
        coordMapProc.side_pgmY = new PgmImage[cameraNum];
        coordMapProc.side_pgmU = new PgmImage[cameraNum];
        coordMapProc.side_pgmV = new PgmImage[cameraNum];
        mapData = new CoordMapData();
        mapData.gainY = new PgmImage[cameraNum];
        mapData.gainU = new PgmImage[cameraNum];
        mapData.gainV = new PgmImage[cameraNum];
        mapData.cmb_mask = new PgmImage[cameraNum];
        mapData.mapX = new FloatImage[cameraNum];
        mapData.mapY = new FloatImage[cameraNum];

        gainY = new PgmImage[cameraNum];
        gainU = new PgmImage[cameraNum];
        gainV = new PgmImage[cameraNum];

        for(int cid = 0;cid<cameraNum;cid++) {
            coordMapProc.org_pgmY[cid] = new PgmImage();
            coordMapProc.org_pgmU[cid] = new PgmImage();
            coordMapProc.org_pgmV[cid] = new PgmImage();
            coordMapProc.org_mask[cid] = new PgmImage();
            coordMapProc.side_pgmY[cid] = new PgmImage();
            coordMapProc.side_pgmU[cid] = new PgmImage();
            coordMapProc.side_pgmV[cid] = new PgmImage();

            mapData.gainY[cid] = new PgmImage();
            mapData.gainU[cid] = new PgmImage();
            mapData.gainV[cid] = new PgmImage();
            mapData.cmb_mask[cid] = new PgmImage();
            mapData.mapX[cid] = new FloatImage();
            mapData.mapY[cid] = new FloatImage();

//            mapData.cmb_mask[cid] = cmbFile.cmb_mask[cid];
//            mapData.mapX[cid] = cmbFile.map_x[cid];
//            mapData.mapY[cid] = cmbFile.map_y[cid];

            mapData.cmb_mask[cid].data.put(cmbFile.cmb_mask[cid]);
            mapData.mapX[cid].data.put(cmbFile.map_x[cid]);
            mapData.mapY[cid].data.put(cmbFile.map_y[cid]);
            mapData.cmb_mask[cid].data.rewind();
            mapData.mapX[cid].data.rewind();
            mapData.mapY[cid].data.rewind();

            gainY[cid] = new PgmImage();
            gainU[cid] = new PgmImage();
            gainV[cid] = new PgmImage();
        }
        zero = ByteBuffer.allocate(col*row);
        gray = ByteBuffer.allocate(col*row);
        for (int i = 0;i<gray.capacity();i++){
            gray.put((byte) 128);
        }
        gray.position(0);
        initGain(cameraNum);


//        ByteBuffer buffer = ByteBuffer.allocate(10);
//        for (int i = 0; i < buffer.capacity(); i++) {
//            buffer.put((byte)i);
//        }
//        buffer.position(3).limit(7);
//        ByteBuffer slice = buffer.slice();
//        for (int i = 0; i < slice.capacity(); i++) {
//            byte b = slice.get(i);
//            b *= 11;
//            slice.put(i, b);
//        }
//        buffer.position(0).limit(buffer.capacity());
//        while (buffer.hasRemaining()) {
//            Log.d(TAG, "PEColorTune: slice:" + buffer.get());
//        }
    }

    static int cnt = 0;
    public static void getOrgImg(int cid,ByteBuffer orgImg){
        //        Log.d(TAG, "decode: cid:"+ cid);
        if (orgImg.get(0)==0||orgImg.get(16)==16)
            return;
        orgImg.rewind();//position置零
//        Log.e(TAG, "getOrgImg: "+orgImg.get());
        if (orgImgReadyList[cid] == 0){
            Log.e(TAG, "getOrgImg: "+orgImg.get());
            orgImgReadyList[cid] = cid+1;
            cnt++;
            Log.d(TAG, "decode: cnt:"+ cnt);
            ByteBuffer YBuf = ByteBuffer.allocate(1228800);//1228800
            orgImg.position(0).limit(1228800);
            YBuf.put(orgImg);
            ByteBuffer UBuf = ByteBuffer.allocate(307200);//307200
            orgImg.position(1228800).limit(1536000);
            UBuf.put(orgImg);
            ByteBuffer VBuf = ByteBuffer.allocate(307200);//307200
            orgImg.position(1536000).limit(1843200);
            VBuf.put(orgImg);

//            coordMapProc.org_pgmY[cid].data = YBuf;
//            coordMapProc.org_pgmU[cid].data = UBuf;
//            coordMapProc.org_pgmV[cid].data = VBuf;
//            coordMapProc.org_mask[cid].data = binFile.camConfigs.get(cid).alphaData;

            resize(YBuf,Define.CUR_WIDTH,Define.CUR_HEIGHT,coordMapProc.org_pgmY[cid].data,Define.GAIN_WIDTH,Define.GAIN_HEIGHT);
            resize(UBuf,Define.CUR_WIDTH/2,Define.CUR_HEIGHT/2,coordMapProc.org_pgmU[cid].data,Define.GAIN_WIDTH,Define.GAIN_HEIGHT);
            resize(VBuf,Define.CUR_WIDTH/2,Define.CUR_HEIGHT/2,coordMapProc.org_pgmV[cid].data,Define.GAIN_WIDTH,Define.GAIN_HEIGHT);
            //resize(binFile.camConfigs.get(cid).alphaData,Define.CUR_WIDTH,Define.CUR_HEIGHT,coordMapProc.org_mask[cid].data,Define.GAIN_WIDTH,Define.GAIN_HEIGHT);

//            writePgmFormat(binFile.camConfigs.get(cid).alphaData,cid);
//            writePgmFormat(coordMapProc.org_mask[cid].data,cid);
        }
    }

    public static void getGain(int cameraNum){
        PEColorTune.isNeedOrgImg =false;
        for (int cid = 0; cid < cameraNum; cid++)
        {
            Log.d(TAG, "getGain: cid-->"+cid);
            for (int nid = 0; nid < cameraNum; nid++)
            {
//                Log.d(TAG, "getGain: nid-->"+nid);
                boolean writeFlag = false;
                if (nid == cid)
                    continue;

                coordMapProc.side_pgmY[nid].data.put(zero);
                coordMapProc.side_pgmY[nid].data.clear();
                zero.clear();
                coordMapProc.side_pgmU[nid].data.put(zero);
                coordMapProc.side_pgmU[nid].data.clear();
                zero.clear();
                coordMapProc.side_pgmV[nid].data.put(zero);
                coordMapProc.side_pgmV[nid].data.clear();
                zero.clear();

                int val = (nid + 1) * 20;
                for (int r = 0; r < row; r++)
                {
//                    Log.d(TAG, "getGain: row-->"+r);
                    for (int c = 0; c < col; c++)
                    {
//                        Log.d(TAG, "getGain: col-->"+c);\
                        if ((mapData.cmb_mask[cid].array1[r*col+c]&0xff) == val)//==优先级高于&
                        {
                            writeFlag = true;
                            float x = mapData.mapX[cid].array1[r*col+c];
                            float y = mapData.mapY[cid].array1[r*col+c];
                            int xi = (int)Math.floor(x);
                            int yi = (int)Math.floor(y);

                            if (xi >= 0 && xi < col - 1 && yi >= 0 && yi < row - 1)
                            {
                                float dx = x - xi;
                                float dy = y - yi;
                                int data00 = coordMapProc.org_pgmY[nid].array1[yi*col+xi]&0xff;
                                int data01 = coordMapProc.org_pgmY[nid].array1[yi*col+xi+1]&0xff;
                                int data10 = coordMapProc.org_pgmY[nid].array1[(yi+1)*col+xi]&0xff;
                                int data11 = coordMapProc.org_pgmY[nid].array1[(yi+1)*col+xi+1]&0xff;
                                float color0 = dx*data01 + (1 - dx)*data00;
                                float color1 = dx*data11 + (1 - dx)*data10;
                                float color = dy*color1 + (1 - dy)*color0;
                                coordMapProc.side_pgmY[nid].array1[r*col+c]=(byte)Math.round(color);

                                data00 = coordMapProc.org_pgmU[nid].array1[yi*col+xi]&0xff;
                                data01 = coordMapProc.org_pgmU[nid].array1[yi*col+xi+1]&0xff;
                                data10 = coordMapProc.org_pgmU[nid].array1[(yi+1)*col+xi]&0xff;
                                data11 = coordMapProc.org_pgmU[nid].array1[(yi+1)*col+xi+1]&0xff;
                                color0 = dx*data01 + (1 - dx)*data00;
                                color1 = dx*data11 + (1 - dx)*data10;
                                color = dy*color1 + (1 - dy)*color0;
                                coordMapProc.side_pgmU[nid].array1[r*col+c]=(byte)Math.round(color);

                                data00 = coordMapProc.org_pgmV[nid].array1[yi*col+xi]&0xff;
                                data01 = coordMapProc.org_pgmV[nid].array1[yi*col+xi+1]&0xff;
                                data10 = coordMapProc.org_pgmV[nid].array1[(yi+1)*col+xi]&0xff;
                                data11 = coordMapProc.org_pgmV[nid].array1[(yi+1)*col+xi+1]&0xff;
                                color0 = dx*data01 + (1 - dx)*data00;
                                color1 = dx*data11 + (1 - dx)*data10;
                                color = dy*color1 + (1 - dy)*color0;
                                coordMapProc.side_pgmV[nid].array1[r*col+c]=(byte)Math.round(color);
                            }
                        }
                    }
                }
//                if (writeFlag)
//                    writePgmFormat(coordMapProc.side_pgmY[nid].data,cid,nid);
            }
            PgmImage finalY = mapData.gainY[cid];
            PgmImage finalU = mapData.gainU[cid];
            PgmImage finalV = mapData.gainV[cid];

            finalY.data.put(coordMapProc.org_pgmY[cid].data);
            finalU.data.put(coordMapProc.org_pgmU[cid].data);
            finalV.data.put(coordMapProc.org_pgmV[cid].data);
            finalY.data.clear();
            coordMapProc.org_pgmY[cid].data.clear();
            finalU.data.clear();
            coordMapProc.org_pgmU[cid].data.clear();
            finalV.data.clear();
            coordMapProc.org_pgmV[cid].data.clear();

            for (int nid = 0; nid < cameraNum; nid++)
            {
                Log.e(TAG, "getGain: nid-->"+nid);
                if (cid != nid)
                {
                    int val = (nid + 1) * 20;
                    PgmImage inputY = coordMapProc.side_pgmY[nid];
                    PgmImage inputU = coordMapProc.side_pgmU[nid];
                    PgmImage inputV = coordMapProc.side_pgmV[nid];
                    PgmImage cmb_mask = mapData.cmb_mask[cid];
                    for (int r = 0; r < row; r++)
                    {
                        for (int c = 0; c < col; c++)
                        {
                            int a = cmb_mask.array1[r*col+c]&0xff;
                            if (a == val)
                            {
                                finalY.array1[r*col+c]=inputY.array1[r*col+c];
                                finalU.array1[r*col+c]=inputU.array1[r*col+c];
                                finalV.array1[r*col+c]=inputV.array1[r*col+c];
                            }
                        }
                    }
                }
            }

            PoissonEditing000(finalY, finalU, finalV, coordMapProc.org_pgmY[cid], coordMapProc.org_pgmU[cid], coordMapProc.org_pgmV[cid], 30);

            for (int r = 0; r < row; r++)
            {
                for (int c = 0; c < col; c++)
                {
                    //注意：Java中+-运算符优先级高于&运算符（c++是&优先级高于+-）
                    finalY.array1[r*col+c]=(byte)Math.min(Math.max((finalY.array1[r*col+c]&0xff) - (coordMapProc.org_pgmY[cid].array1[r*col+c]&0xff)+128,0),255);
                    finalU.array1[r*col+c]=(byte)Math.min(Math.max((finalU.array1[r*col+c]&0xff) - (coordMapProc.org_pgmU[cid].array1[r*col+c]&0xff)+128,0),255);//133
                    finalV.array1[r*col+c]=(byte)Math.min(Math.max((finalV.array1[r*col+c]&0xff) - (coordMapProc.org_pgmV[cid].array1[r*col+c]&0xff)+128,0),255);//98
                }
            }

//            writePgmFormat(cid,coordMapProc.org_pgmY[cid].data,coordMapProc.org_pgmU[cid].data,coordMapProc.org_pgmV[cid].data);
//            writePgmFormat(cid,finalY.data,finalU.data,finalV.data);

            gainY[cid].data.put(finalY.data);
            gainY[cid].data.position(0);
            finalY.data.position(0);
            gainU[cid].data.put(finalU.data);
            gainU[cid].data.position(0);
            finalU.data.position(0);
            gainV[cid].data.put(finalV.data);
            gainV[cid].data.position(0);
            finalV.data.position(0);
        }
        isGainReady = true;
    }

    public static void getGain111(int cameraNum){
        for (int cid = 0; cid < cameraNum; cid++)
        {
            Log.d(TAG, "getGain: cid-->"+cid);
            for (int nid = 0; nid < cameraNum; nid++)
            {
//                Log.d(TAG, "getGain: nid-->"+nid);
                boolean writeFlag = false;
                if (nid == cid)
                    continue;

                coordMapProc.side_pgmY[nid].data.put(zero);
                coordMapProc.side_pgmY[nid].data.clear();
                zero.clear();
                coordMapProc.side_pgmU[nid].data.put(zero);
                coordMapProc.side_pgmU[nid].data.clear();
                zero.clear();
                coordMapProc.side_pgmV[nid].data.put(zero);
                coordMapProc.side_pgmV[nid].data.clear();
                zero.clear();

                int val = (nid + 1) * 20;
                for (int r = 0; r < row; r++)
                {
//                    Log.d(TAG, "getGain: row-->"+r);
                    for (int c = 0; c < col; c++)
                    {
//                        Log.d(TAG, "getGain: col-->"+c);\
//                        Log.d(TAG, "getGain: 0xff:"+(mapData.cmb_mask[cid].img[r].get(c)&0xff));
//                        if ((mapData.cmb_mask[cid].data.get(r*col+c)&0xff) == val)//==优先级高于&
                        if ((mapData.cmb_mask[cid].img[r].get(c)&0xff) == val)//==优先级高于&
                        {
                            writeFlag = true;
                            float x = mapData.mapX[cid].img[r].get(c);
                            float y = mapData.mapY[cid].img[r].get(c);
                            int xi = (int)Math.floor(x);
                            int yi = (int)Math.floor(y);

                            if (xi >= 0 && xi < col - 1 && yi >= 0 && yi < row - 1)
                            {
                                float dx = x - xi;
                                float dy = y - yi;
                                int data00 = coordMapProc.org_pgmY[nid].img[yi].get(xi)&0xff;
                                int data01 = coordMapProc.org_pgmY[nid].img[yi].get(xi + 1)&0xff;
                                int data10 = coordMapProc.org_pgmY[nid].img[yi + 1].get(xi)&0xff;
                                int data11 = coordMapProc.org_pgmY[nid].img[yi + 1].get(xi + 1)&0xff;
                                float color0 = dx*data01 + (1 - dx)*data00;
                                float color1 = dx*data11 + (1 - dx)*data10;
//                                float color0 = dx*coordMapProc.org_pgmY[nid].img[yi].get(xi + 1) + (1 - dx)*coordMapProc.org_pgmY[nid].img[yi].get(xi);
//                                float color1 = dx*coordMapProc.org_pgmY[nid].img[yi + 1].get(xi + 1) + (1 - dx)*coordMapProc.org_pgmY[nid].img[yi + 1].get(xi);
                                float color = dy*color1 + (1 - dy)*color0;
                                coordMapProc.side_pgmY[nid].img[r].put(c,(byte)Math.round(color));

                                data00 = coordMapProc.org_pgmU[nid].img[yi].get(xi)&0xff;
                                data01 = coordMapProc.org_pgmU[nid].img[yi].get(xi + 1)&0xff;
                                data10 = coordMapProc.org_pgmU[nid].img[yi + 1].get(xi)&0xff;
                                data11 = coordMapProc.org_pgmU[nid].img[yi + 1].get(xi + 1)&0xff;
                                color0 = dx*data01 + (1 - dx)*data00;
                                color1 = dx*data11 + (1 - dx)*data10;
//                                color0 = dx*coordMapProc.org_pgmU[nid].img[yi].get(xi + 1) + (1 - dx)*coordMapProc.org_pgmU[nid].img[yi].get(xi);
//                                color1 = dx*coordMapProc.org_pgmU[nid].img[yi + 1].get(xi + 1) + (1 - dx)*coordMapProc.org_pgmU[nid].img[yi + 1].get(xi);
                                color = dy*color1 + (1 - dy)*color0;
//                                Log.d(TAG, "getGain: color:"+color);
                                coordMapProc.side_pgmU[nid].img[r].put(c,(byte)Math.round(color));

                                data00 = coordMapProc.org_pgmV[nid].img[yi].get(xi)&0xff;
                                data01 = coordMapProc.org_pgmV[nid].img[yi].get(xi + 1)&0xff;
                                data10 = coordMapProc.org_pgmV[nid].img[yi + 1].get(xi)&0xff;
                                data11 = coordMapProc.org_pgmV[nid].img[yi + 1].get(xi + 1)&0xff;
                                color0 = dx*data01 + (1 - dx)*data00;
                                color1 = dx*data11 + (1 - dx)*data10;
//                                color0 = dx*coordMapProc.org_pgmV[nid].img[yi].get(xi + 1) + (1 - dx)*coordMapProc.org_pgmV[nid].img[yi].get(xi);
//                                color1 = dx*coordMapProc.org_pgmV[nid].img[yi + 1].get(xi + 1) + (1 - dx)*coordMapProc.org_pgmV[nid].img[yi + 1].get(xi);
                                color = dy*color1 + (1 - dy)*color0;
//                                Log.d(TAG, "getGain: color:"+color);
                                coordMapProc.side_pgmV[nid].img[r].put(c,(byte)Math.round(color));
                            }
                        }
                    }
                }
//                if (writeFlag)
//                    writePgmFormat(coordMapProc.side_pgmY[nid].data,cid,nid);
            }
            PgmImage finalY = mapData.gainY[cid];
            PgmImage finalU = mapData.gainU[cid];
            PgmImage finalV = mapData.gainV[cid];
//            Log.d(TAG, "getGain: position:"+finalY.data.position()+",limit:"+finalY.data.limit());
//            Log.d(TAG, "getGain: position:"+coordMapProc.org_pgmY[cid].data.position()+",remaining"+coordMapProc.org_pgmY[cid].data.remaining());
            finalY.data.put(coordMapProc.org_pgmY[cid].data);
            finalU.data.put(coordMapProc.org_pgmU[cid].data);
            finalV.data.put(coordMapProc.org_pgmV[cid].data);
            finalY.data.clear();
            coordMapProc.org_pgmY[cid].data.clear();
            finalU.data.clear();
            coordMapProc.org_pgmU[cid].data.clear();
            finalV.data.clear();
            coordMapProc.org_pgmV[cid].data.clear();
//            mapData.gainY[cid] = coordMapProc.org_pgmY[cid];
//            mapData.gainU[cid] = coordMapProc.org_pgmU[cid];
//            mapData.gainV[cid] = coordMapProc.org_pgmV[cid];

//            writePgmFormat(mapData.mapX[cid].data,cid,mapData.mapY[cid].data);

            for (int nid = 0; nid < cameraNum; nid++)
            {
                Log.e(TAG, "getGain: nid-->"+nid);
                if (cid != nid)
                {
                    int val = (nid + 1) * 20;
                    PgmImage inputY = coordMapProc.side_pgmY[nid];
                    PgmImage inputU = coordMapProc.side_pgmU[nid];
                    PgmImage inputV = coordMapProc.side_pgmV[nid];
                    PgmImage cmb_mask = mapData.cmb_mask[cid];
                    for (int r = 0; r < row; r++)
                    {
                        for (int c = 0; c < col; c++)
                        {
                            int a = cmb_mask.img[r].get(c)&0xff;
                            if (a == val)
                            {
                                finalY.img[r].put(c,inputY.img[r].get(c));
                                finalU.img[r].put(c,inputU.img[r].get(c));
                                finalV.img[r].put(c,inputV.img[r].get(c));
                            }
                        }
                    }
                }
            }

//            PoissonEditing(finalY, finalU, finalV, coordMapProc.org_pgmY[cid], coordMapProc.org_pgmU[cid], coordMapProc.org_pgmV[cid], 300);

            for (int r = 0; r < row; r++)
            {
                for (int c = 0; c < col; c++)
                {
                    //注意：Java中+-运算符优先级高于&运算符（c++是&优先级高于+-）
                    finalY.img[r].put(c,(byte)Math.min(Math.max((finalY.img[r].get(c)&0xff) - (coordMapProc.org_pgmY[cid].img[r].get(c)&0xff)+128,0),255));
                    finalU.img[r].put(c,(byte)Math.min(Math.max((finalU.img[r].get(c)&0xff) - (coordMapProc.org_pgmU[cid].img[r].get(c)&0xff)+128,0),255));//133
                    finalV.img[r].put(c,(byte)Math.min(Math.max((finalV.img[r].get(c)&0xff) - (coordMapProc.org_pgmV[cid].img[r].get(c)&0xff)+128,0),255));//98
                }
            }

//            writePgmFormat(coordMapProc.org_pgmY[cid].data,cid);
//            writePgmFormat(mapData.cmb_mask[cid].data,cid);
//            writePgmFormat(finalY.data,cid);

//            writePgmFormat(coordMapProc.org_pgmY[cid].data,cid,finalY.data);
//            writePgmFormat(coordMapProc.org_pgmY[cid].data,cid,mapData.gainY[cid].data);
//            writePgmFormat(cid,finalY.data,finalU.data,finalV.data);

            gainY[cid].data.put(finalY.data);
            gainY[cid].data.position(0);
            finalY.data.position(0);
            gainU[cid].data.put(finalU.data);
            gainU[cid].data.position(0);
            finalU.data.position(0);
            gainV[cid].data.put(finalV.data);
            gainV[cid].data.position(0);
            finalV.data.position(0);
        }
        isGainReady = true;
    }

    public static void getGain000(int cameraNum){
        for (int cid = 0; cid < cameraNum; cid++)
        {
            Log.d(TAG, "getGain: cid-->"+cid);
            for (int nid = 0; nid < cameraNum; nid++)
            {
                Log.d(TAG, "getGain: nid-->"+nid);
                boolean writeFlag = false;
                if (nid == cid)
                    continue;

//                Log.e(TAG, "getGain: zero.remaining() "+ zero.remaining());
                coordMapProc.side_pgmY[nid].data.put(zero);
                coordMapProc.side_pgmY[nid].data.clear();
                zero.clear();
                coordMapProc.side_pgmU[nid].data.put(zero);
                coordMapProc.side_pgmU[nid].data.clear();
                zero.clear();
                coordMapProc.side_pgmV[nid].data.put(zero);
                coordMapProc.side_pgmV[nid].data.clear();
                zero.clear();

                int val = (nid + 1) * 20;
                for (int r = 0; r < row; r++)
                {
//                Log.d(TAG, "getGain: row-->"+r);
                    for (int c = 0; c < col; c++)
                    {
//                    Log.d(TAG, "getGain: col-->"+c);
                        int ubyte = mapData.cmb_mask[cid].data.get(r*col+c)&0xff;//有符号（byte范围-127~128）转无符号（0~255）
//                        Log.d(TAG, "getGain: "+ ubyte);
                        if (ubyte == val)
                        {
                            writeFlag=true;
                            float x = mapData.mapX[cid].data.get(r*col+c);
                            float y = mapData.mapY[cid].data.get(r*col+c);
                            int xi = (int)Math.floor(x);
                            int yi = (int)Math.floor(y);
//                            Log.e(TAG, "getGain: "+r+","+c+":"+x+","+y);
//                            Log.d(TAG, "getGain: "+r+","+c+":"+xi+","+yi);
                            if (xi >= 0 && xi < col - 1 && yi >= 0 && yi < row - 1)
                            {
                                float dx = x - xi;
                                float dy = y - yi;
                                int data00 = coordMapProc.org_pgmY[nid].data.get(yi*col+xi)&0xff;
                                int data01 = coordMapProc.org_pgmY[nid].data.get(yi*col + xi + 1)&0xff;
                                int data10 = coordMapProc.org_pgmY[nid].data.get((yi + 1)*col+xi)&0xff;
                                int data11 = coordMapProc.org_pgmY[nid].data.get((yi + 1)*col+xi + 1)&0xff;
                                float color0 = dx*data01 + (1 - dx)*data00;
                                float color1 = dx*data11 + (1 - dx)*data10;
                                float color = dy*color1 + (1 - dy)*color0;
                                coordMapProc.side_pgmY[nid].data.put(r*col+c,(byte)Math.round(color));//Math.round():四舍五入取整
//                                Log.d(TAG, "getGain: "+r+","+c+"："+dx+","+dy);
//                                Log.d(TAG, "getGain: "+r+","+c+"："+coordMapProc.org_pgmY[nid].data.get(r*col + c));
//                                Log.d(TAG, "getGain: "+r+","+c+"："+coordMapProc.org_pgmY[nid].data.get(yi*col + xi + 1)+","+coordMapProc.org_pgmY[nid].data.get(yi*col+xi));
//                                Log.d(TAG, "getGain: "+r+","+c+"："+coordMapProc.org_pgmY[nid].data.get((yi + 1)*col+xi + 1)+","+coordMapProc.org_pgmY[nid].data.get((yi + 1)*col+xi));
//                                Log.d(TAG, "getGain: "+r+","+c+"："+color0+","+color1);
//                                Log.d(TAG, "getGain: "+r+","+c+"："+Math.round(color));
//                                Log.e(TAG, "getGain: "+r+","+c+"："+(byte)Math.round(color));

                                data00 = coordMapProc.org_pgmU[nid].data.get(yi*col+xi)&0xff;
                                data01 = coordMapProc.org_pgmU[nid].data.get(yi*col + xi + 1)&0xff;
                                data10 = coordMapProc.org_pgmU[nid].data.get((yi + 1)*col+xi)&0xff;
                                data11 = coordMapProc.org_pgmU[nid].data.get((yi + 1)*col+xi + 1)&0xff;
                                color0 = dx*data01 + (1 - dx)*data00;
                                color1 = dx*data11 + (1 - dx)*data10;
//                                color0 = dx*coordMapProc.org_pgmU[nid].data.get(yi*col+xi + 1) + (1 - dx)*coordMapProc.org_pgmU[nid].data.get(yi*col+xi);
//                                color1 = dx*coordMapProc.org_pgmU[nid].data.get((yi + 1)*col+xi + 1) + (1 - dx)*coordMapProc.org_pgmU[nid].data.get((yi + 1)*col+xi);
                                color = dy*color1 + (1 - dy)*color0;
                                coordMapProc.side_pgmU[nid].data.put(r*col+c,(byte)Math.round(color));

                                data00 = coordMapProc.org_pgmV[nid].data.get(yi*col+xi)&0xff;
                                data01 = coordMapProc.org_pgmV[nid].data.get(yi*col + xi + 1)&0xff;
                                data10 = coordMapProc.org_pgmV[nid].data.get((yi + 1)*col+xi)&0xff;
                                data11 = coordMapProc.org_pgmV[nid].data.get((yi + 1)*col+xi + 1)&0xff;
                                color0 = dx*data01 + (1 - dx)*data00;
                                color1 = dx*data11 + (1 - dx)*data10;
//                                color0 = dx*coordMapProc.org_pgmV[nid].data.get(yi*col+xi + 1) + (1 - dx)*coordMapProc.org_pgmV[nid].data.get(yi*col+xi);
//                                color1 = dx*coordMapProc.org_pgmV[nid].data.get((yi + 1)*col+xi + 1) + (1 - dx)*coordMapProc.org_pgmV[nid].data.get((yi + 1)*col+xi);
                                color = dy*color1 + (1 - dy)*color0;
                                coordMapProc.side_pgmV[nid].data.put(r*col+c,(byte)Math.round(color));
                            }else {
                                Log.e(TAG, "getGain: "+r+","+c+":"+x+","+y);
                                Log.d(TAG, "getGain: "+r+","+c+":"+xi+","+yi);
                            }
                        }
                    }
                }
//                if (writeFlag)
//                    writePgmFormat(coordMapProc.side_pgmY[nid].data,cid,nid);
            }

            PgmImage finalY = mapData.gainY[cid];
            PgmImage finalU = mapData.gainU[cid];
            PgmImage finalV = mapData.gainV[cid];
//            Log.d(TAG, "getGain: position:"+finalY.data.position()+",limit:"+finalY.data.limit());
//            Log.d(TAG, "getGain: remaining:"+coordMapProc.org_pgmY[cid].data.remaining());
//            coordMapProc.org_pgmY[cid].data.clear();
            finalY.data.put(coordMapProc.org_pgmY[cid].data);
            finalU.data.put(coordMapProc.org_pgmU[cid].data);
            finalV.data.put(coordMapProc.org_pgmV[cid].data);
//            Log.d(TAG, "getGain: position:"+finalY.data.position()+",limit:"+finalY.data.limit());
//            Log.d(TAG, "getGain: remaining:"+coordMapProc.org_pgmY[cid].data.remaining());
            finalY.data.clear();
            coordMapProc.org_pgmY[cid].data.clear();
            finalU.data.clear();
            coordMapProc.org_pgmU[cid].data.clear();
            finalV.data.clear();
            coordMapProc.org_pgmV[cid].data.clear();
//            writePgmFormat(coordMapProc.org_pgmY[cid].data,cid,finalY.data);
//            writePgmFormat(mapData.cmb_mask[cid].data,cid);
//            writePgmFormat(mapData.mapX[cid].data,cid,mapData.mapY[cid].data);

            for (int nid = 0; nid < cameraNum; nid++)
            {
//                Log.e(TAG, "getGain: nid-->"+nid);
                if (cid != nid)
                {
                    int val = (nid + 1) * 20;
                    PgmImage inputY = coordMapProc.side_pgmY[nid];
                    PgmImage inputU = coordMapProc.side_pgmU[nid];
                    PgmImage inputV = coordMapProc.side_pgmV[nid];
                    PgmImage mask = mapData.cmb_mask[cid];

                    for (int r = 0; r < row; r++)
                    {
                        for (int c = 0; c < col; c++)
                        {
                            int a = mask.data.get(r*col+c)&0xff;//有符号（byte范围-127~128）转无符号（0~255）
//                            Log.e(TAG, "getGain: a"+a);
                            if (a == val)
                            {
//                                Log.d(TAG, "getGain: val"+val);
                                finalY.data.put(r*col+c,inputY.data.get(r*col+c));
                                finalU.data.put(r*col+c,inputU.data.get(r*col+c));
                                finalV.data.put(r*col+c,inputV.data.get(r*col+c));
                            }
                        }
                    }
                }
            }

//            PoissonEditing(finalY, finalU, finalV, coordMapProc.org_pgmY[cid], coordMapProc.org_pgmU[cid], coordMapProc.org_pgmV[cid], 300);

            for (int r = 0; r < row; r++)
            {
                for (int c = 0; c < col; c++)
                {
//                    int dataY = (finalY.data.get(r*col+c)&0xff) - (coordMapProc.org_pgmY[cid].data.get(r*col+c)&0xff);
//                    int mathY = Math.min(Math.max(dataY+128,0),255);
//                    finalY.data.put(r*col+c,(byte)mathY);
                    //注意：Java中+-运算符优先级高于&运算符（c++是&优先级高于+-）
                    finalY.data.put(r*col+c,(byte)Math.min(Math.max((finalY.data.get(r*col+c)&0xff) - (coordMapProc.org_pgmY[cid].data.get(r*col+c)&0xff)+128,0),255));
                    finalU.data.put(r*col+c,(byte)Math.min(Math.max((finalU.data.get(r*col+c)&0xff) - (coordMapProc.org_pgmU[cid].data.get(r*col+c)&0xff)+128,0),255));
                    finalV.data.put(r*col+c,(byte)Math.min(Math.max((finalV.data.get(r*col+c)&0xff) - (coordMapProc.org_pgmV[cid].data.get(r*col+c)&0xff)+128,0),255));
                }
            }

//            writePgmFormat(coordMapProc.org_pgmY[cid].data,cid,mapData.gainY[cid].data);

        }
        isGainReady = true;
    }

    public static void initGain(int cameraNum){
        Log.d(TAG, "initGain: ");
        for (int cid = 0; cid < cameraNum; cid++)
        {
            gainY[cid].data.put(gray);
//            writePgmFormat(coordMapProc.org_pgmY[cid].data,cid,gainY[cid].data);
            gainY[cid].data.rewind(); //position置零
            gray.rewind();            //position置零
            gainU[cid].data.put(gray);
            gainU[cid].data.rewind(); //position置零
            gray.rewind();            //position置零
            gainV[cid].data.put(gray);
            gainV[cid].data.rewind(); //position置零
            gray.rewind();            //position置零
        }
    }

    public static void setGain(int cameraNum) {
        Log.d(TAG, "setGain: ");
        for (int cid = 0; cid < cameraNum; cid++) {
            gainY[cid].data.put(mapData.gainY[cid].data);
//            writePgmFormat(coordMapProc.org_pgmY[cid].data,cid,gainY[cid].data);
            gainY[cid].data.rewind();           //position置零
            mapData.gainY[cid].data.rewind();   //position置零
            gainU[cid].data.put(mapData.gainU[cid].data);
            gainU[cid].data.rewind();           //position置零
            mapData.gainU[cid].data.rewind();   //position置零
            gainV[cid].data.put(mapData.gainV[cid].data);
            gainV[cid].data.rewind();           //position置零
            mapData.gainV[cid].data.rewind();   //position置零
        }
    }

    //泊松融合
    private static void PoissonEditing000(PgmImage bgY,PgmImage bgU,PgmImage bgV,PgmImage fgY,PgmImage fgU,PgmImage fgV,int itr) {
        FloatImage f_r = new FloatImage();
        FloatImage f_g = new FloatImage();
        FloatImage f_b = new FloatImage();
        FloatImage v_r = new FloatImage();
        FloatImage v_g = new FloatImage();
        FloatImage v_b = new FloatImage();

        int dir[][] = { { 1, 0 },{ -1, 0 }, { 0, 1 }, { 0, -1 } };

        float w = 1.8f;

        for (int r = 0; r < row; r++)
        {
            for (int c = 0; c < col; c++)
            {
                f_r.img[r].put(c,bgY.img[r].get(c)&0xff);
                f_g.img[r].put(c,bgU.img[r].get(c)&0xff);
                f_b.img[r].put(c,bgV.img[r].get(c)&0xff);
                v_r.img[r].put(c,fgY.img[r].get(c)&0xff);
                v_g.img[r].put(c,fgU.img[r].get(c)&0xff);
                v_b.img[r].put(c,fgV.img[r].get(c)&0xff);
            }
        }
        for (int k = 0; k < itr; k++)
        {
            for (int r = 0; r < row; r++)
            {
                for (int c = 0; c < col; c++)
                {
                    float fr = 0;
                    float fg = 0;
                    float fb = 0;
                    //float sr = 0;
                    //float sg = 0;
                    //float sb = 0;
                    float vr = 0;
                    float vg = 0;
                    float vb = 0;
                    int num = 0;
                    for (int i = 0; i < 4; i++)
                    {
                        int r1 = r + dir[i][1];
                        int c1 = c + dir[i][0];
                        if (r1 >= 0 && r1 < row && c1 >= 0 && c1 < col)
                        {
                            fr += f_r.img[r1].get(c1);
                            vr += v_r.img[r].get(c) - v_r.img[r1].get(c1);
                            fg += f_g.img[r1].get(c1);
                            vg += v_g.img[r].get(c) - v_g.img[r1].get(c1);
                            fb += f_b.img[r1].get(c1);
                            vb += v_b.img[r].get(c) - v_b.img[r1].get(c1);
                            num++;
                        }
                    }
                    if (num > 0)
                    {
                        f_r.img[r].put(c,(1 - w)*f_r.img[r].get(c) + w*(fr + vr) / num);
                        f_g.img[r].put(c,(1 - w)*f_g.img[r].get(c) + w*(fg + vg) / num);
                        f_b.img[r].put(c,(1 - w)*f_b.img[r].get(c) + w*(fb + vb) / num);
                    }
                }
            }
        }
        for (int r = 0; r < row; r++)
        {
            for (int c = 0; c < col; c++)
            {
                bgY.img[r].put(c,(byte)Math.round(Math.min((int)Math.max(f_r.img[r].get(c), 0), 255)));
                bgU.img[r].put(c,(byte)Math.round(Math.min((int)Math.max(f_g.img[r].get(c), 0), 255)));
                bgV.img[r].put(c,(byte)Math.round(Math.min((int)Math.max(f_b.img[r].get(c), 0), 255)));
            }
        }
    }
    //泊松融合改进
    private static void PoissonEditing(PgmImage bgY,PgmImage bgU,PgmImage bgV,PgmImage oY,PgmImage oU,PgmImage oV,int itr) {
        FloatImage f_r = new FloatImage();
        FloatImage f_g = new FloatImage();
        FloatImage f_b = new FloatImage();
        FloatImage o_r = new FloatImage();
        FloatImage o_g = new FloatImage();
        FloatImage o_b = new FloatImage();

        int dir[][] = { { 1, 0 },{ -1, 0 }, { 0, 1 }, { 0, -1 } };
        float w = 1.8f;

        for (int r = 0; r < row; r++)
        {
            for (int c = 0; c < col; c++)
            {
                f_r.img[r].put(c,bgY.img[r].get(c)&0xff);
//                f_g.img[r].put(c,bgU.img[r].get(c)&0xff);
//                f_b.img[r].put(c,bgV.img[r].get(c)&0xff);
                o_r.img[r].put(c,oY.img[r].get(c)&0xff);
//                o_g.img[r].put(c,oU.img[r].get(c)&0xff);
//                o_b.img[r].put(c,oV.img[r].get(c)&0xff);
            }
        }
        for (int k = 0; k < itr; k++)
        {
            for (int r = 0; r < row; r++)
            {
                for (int c = 0; c < col; c++)
                {
                    float fr = 0;
                    float fg = 0;
                    float fb = 0;
                    float vr = 0;
                    float vg = 0;
                    float vb = 0;
                    int num = 0;
                    for (int i = 0; i < 4; i++)
                    {
                        int r1 = r + dir[i][1];
                        int c1 = c + dir[i][0];
                        if (r1 >= 0 && r1 < row && c1 >= 0 && c1 < col)
                        {
                            fr += f_r.img[r1].get(c1);
                            vr += o_r.img[r].get(c) - o_r.img[r1].get(c1);
//                            fg += f_g.img[r1].get(c1);
//                            vg += o_g.img[r].get(c) - o_g.img[r1].get(c1);
//                            fb += f_b.img[r1].get(c1);
//                            vb += o_b.img[r].get(c) - o_b.img[r1].get(c1);
                            num++;
                        }
                    }
                    if (num > 0)
                    {
                        f_r.img[r].put(c,(1 - w)*f_r.img[r].get(c) + w*(fr + vr) / num);
//                        f_g.img[r].put(c,(1 - w)*f_g.img[r].get(c) + w*(fg + vg) / num);
//                        f_b.img[r].put(c,(1 - w)*f_b.img[r].get(c) + w*(fb + vb) / num);
                    }
                }
            }
        }
        for (int r = 0; r < row; r++)
        {
            for (int c = 0; c < col; c++)
            {
                bgY.img[r].put(c,(byte)Math.round(Math.min((int)Math.max(f_r.img[r].get(c), 0), 255)));
//                bgU.img[r].put(c,(byte)Math.round(Math.min((int)Math.max(f_g.img[r].get(c), 0), 255)));
//                bgV.img[r].put(c,(byte)Math.round(Math.min((int)Math.max(f_b.img[r].get(c), 0), 255)));
            }
        }
    }

    private static void resize(ByteBuffer src,int src_width,int src_height, ByteBuffer dst,int dst_width,int dst_height) {
        bilinearInterpolation(src,src_width,src_height,dst,dst_width,dst_height);
        dst.rewind();//position置零
    }


    //双线性插值算法实现图像缩放
    static void bilinearInterpolation(ByteBuffer src, int in_width, int in_height, ByteBuffer dst, int out_width, int out_height){
        float sc = out_width / (float)in_width;
        float sr = out_height / (float)in_height;

        for (int row = 0; row < out_height; row += 1) {
            for (int col = 0; col < out_width; col += 1) {
                //filter(col, row);
                // 源图像中的坐标（可能是一个浮点）
                float srcCol = Math.min(in_width - 1, col / sc);
                float srcRow = Math.min(in_height - 1, row / sr);
                int intCol = ((int) srcCol);
                int intRow = (int)srcRow;
                // 计算u和v
                float u = srcCol - intCol;
                float v = srcRow - intRow;
                // 1-u与1-v
                float u1 = 1 - u;
                float v1 = 1 - v;

                // 真实的index，因为数组是一维的
                int dstI = (row * out_width) + col;

                int data00 = getValue(src, in_width, in_height, intRow + 0, intCol + 0);
                int data01 = getValue(src, in_width, in_height, intRow + 0, intCol + 1);
                int data10 = getValue(src, in_width, in_height, intRow + 1, intCol + 0);
                int data11 = getValue(src, in_width, in_height, intRow + 1, intCol + 1);

                float partV = v * ((u1 * data10) + (u * data11));
                float partV1 = v1 * ((u1 * data00) + (u * data01));
                byte color = (byte) (partV+partV1);
//                Log.d(TAG, "bilinearInterpolation: "+color);
                dst.put(dstI,color);
            }
        }

    }

    static int getValue(ByteBuffer data, int srcWidth, int srcHeight, int row, int col) {
        int newRow = row;
        int newCol = col;

        if (newRow >= srcHeight) {
            newRow = srcHeight - 1;
        }
        else if (newRow < 0) {
            newRow = 0;
        }

        if (newCol >= srcWidth) {
            newCol = srcWidth - 1;
        }
        else if (newCol < 0) {
            newCol = 0;
        }

        int newIndex = (newRow * srcWidth) + newCol;

        //newIndex *= 4;

        return data.get(newIndex)&0xff;
    }

    public static void writePgmFormat (ByteBuffer pgmDataBuf,int cid){
            try {
//                RandomAccessFile fp = PEFile.fopenMode(Define.ROOT_PATH+"/pgm/alpha_cid_"+cid+".pgm","rw");//打开文件
//                String version = "P5\n1280 960\n255\n";
//                RandomAccessFile fp = PEFile.fopenMode(Define.ROOT_PATH+"/pgm/gain_cid_"+cid+".pgm","rw");//打开文件
                RandomAccessFile fp = PEFile.fopenMode(Define.ROOT_PATH+"/pgm/mask_cid_"+cid+".pgm","rw");//打开文件
                String version = "P5\n320 240\n255\n";
                ByteBuffer byteBuffer = ByteBuffer.wrap(version.getBytes());
                PEFile.fwriteArray(fp,byteBuffer);
                PEFile.fwriteArray(fp,pgmDataBuf);
                PEFile.fclose(fp);//释放文件
            } catch (PEError peError) {
                peError.printStackTrace();
            }
    }

    public static void writePgmFormat (ByteBuffer sideY,int cid,int nid){
        try {
            RandomAccessFile fp = PEFile.fopenMode(Define.ROOT_PATH+"/pgm/side_cid_"+cid+"_nid_"+nid+".pgm","rw");//打开文件
            String version = "P5\n320 240\n255\n";
            ByteBuffer byteBuffer = ByteBuffer.wrap(version.getBytes());
            PEFile.fwriteArray(fp,byteBuffer);
            PEFile.fwriteArray(fp,sideY);
            PEFile.fclose(fp);//释放文件
        } catch (PEError peError) {
            peError.printStackTrace();
        }
    }

    public static void writePgmFormat (ByteBuffer orgY,int cid,ByteBuffer gainY){
        try {
            RandomAccessFile fp = PEFile.fopenMode(Define.ROOT_PATH+"/pgm/org_cid_"+cid+".pgm","rw");//打开文件
            String version = "P5\n320 240\n255\n";
            ByteBuffer byteBuffer = ByteBuffer.wrap(version.getBytes());
            PEFile.fwriteArray(fp,byteBuffer);
            PEFile.fwriteArray(fp,orgY);
            PEFile.fclose(fp);//释放文件

            RandomAccessFile fpGainY = PEFile.fopenMode(Define.ROOT_PATH+"/pgm/gain_cid_"+cid+".pgm","rw");//打开文件
            PEFile.fwriteArray(fpGainY,byteBuffer);
            PEFile.fwriteArray(fpGainY,gainY);
            PEFile.fclose(fpGainY);//释放文件
        } catch (PEError peError) {
            peError.printStackTrace();
        }
    }

    public static void writePgmFormat (int cid,ByteBuffer gainY,ByteBuffer gainU,ByteBuffer gainV){
        String version = "P5\n320 240\n255\n";
        ByteBuffer byteBuffer = ByteBuffer.wrap(version.getBytes());
        try {
            RandomAccessFile fp = PEFile.fopenMode(Define.ROOT_PATH+"/pgm/gainY_cid_"+cid+".pgm","rw");//打开文件
            PEFile.fwriteArray(fp,byteBuffer);
            PEFile.fwriteArray(fp,gainY);
            PEFile.fclose(fp);//释放文件

            RandomAccessFile fpU = PEFile.fopenMode(Define.ROOT_PATH+"/pgm/gainU_cid_"+cid+".pgm","rw");//打开文件
            PEFile.fwriteArray(fpU,byteBuffer);
            PEFile.fwriteArray(fpU,gainU);
            PEFile.fclose(fpU);//释放文件

            RandomAccessFile fpV = PEFile.fopenMode(Define.ROOT_PATH+"/pgm/gainV_cid_"+cid+".pgm","rw");//打开文件
            PEFile.fwriteArray(fpV,byteBuffer);
            PEFile.fwriteArray(fpV,gainU);
            PEFile.fclose(fpV);//释放文件
        } catch (PEError peError) {
            peError.printStackTrace();
        }
    }

    public static void writePgmFormatBig (ByteBuffer gainY,int cid,ByteBuffer gainBigY){
        try {
            RandomAccessFile fp = PEFile.fopenMode(Define.ROOT_PATH+"/pgm/gain_cid_"+cid+".pgm","rw");//打开文件
            String version = "P5\n320 240\n255\n";
            ByteBuffer byteBuffer = ByteBuffer.wrap(version.getBytes());
            PEFile.fwriteArray(fp,byteBuffer);
            PEFile.fwriteArray(fp,gainY);
            PEFile.fclose(fp);//释放文件

            version = "P5\n1280 960\n255\n";
            byteBuffer = ByteBuffer.wrap(version.getBytes());
            RandomAccessFile fpGainY = PEFile.fopenMode(Define.ROOT_PATH+"/pgm/gainBigY_cid_"+cid+".pgm","rw");//打开文件
            PEFile.fwriteArray(fpGainY,byteBuffer);
            PEFile.fwriteArray(fpGainY,gainBigY);
            PEFile.fclose(fpGainY);//释放文件
        } catch (PEError peError) {
            peError.printStackTrace();
        }
    }

    public static void writePgmFormat (FloatBuffer mapX,int cid,FloatBuffer mapY){
        try {
            RandomAccessFile fpX = PEFile.fopenMode(Define.ROOT_PATH+"/pgm/cmb_mapX_cid_"+cid+".pgm","rw");//打开文件
            String version = "P5\n320 240\n255\n";
            ByteBuffer byteBuffer = ByteBuffer.wrap(version.getBytes());
            PEFile.fwriteArray(fpX,byteBuffer);
            PEFile.fwriteArray(fpX,mapX);
            PEFile.fclose(fpX);//释放文件

            RandomAccessFile fpY = PEFile.fopenMode(Define.ROOT_PATH+"/pgm/cmb_mapY_cid_"+cid+".pgm","rw");//打开文件
            PEFile.fwriteArray(fpY,byteBuffer);
            PEFile.fwriteArray(fpY,mapY);
            PEFile.fclose(fpY);//释放文件
        } catch (PEError peError) {
            peError.printStackTrace();
        }
    }

    public static boolean undateIfNeedTurn(int cid,int []textures){
        {
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[0]);
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, col, row, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, gainY[cid].data);
//            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);

            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[1]);
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, col, row, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, gainU[cid].data);
//            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);

            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[2]);
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, col, row, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, gainV[cid].data);
//            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
            return true;
        }
//        Size size = null;
//        size.width = col;
//        size.height = row;
//        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[0]);
//        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, size.width, size.height, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, mapData.gainY[cid].data);
//
//        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[1]);
//        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, size.width/2, size.height/2, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, mapData.gainU[cid].data);
//
//        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[2]);
//        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, size.width/2, size.height/2, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, mapData.gainV[cid].data);
//        return false;
    }

    public static void undateIfNeedTurn7(int cid,int []textures){
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[4]);
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, col, row, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, gainY[cid].data);
//            writePgmFormat(coordMapProc.org_pgmY[cid].data,cid,gainY[cid].data);
//            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);

            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[5]);
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, col, row, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, gainU[cid].data);
//            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);

            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[6]);
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, col, row, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, gainV[cid].data);
//            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
    }
}

package panoeye.pelibrary;



import java.io.RandomAccessFile;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Created by tir on 2016/12/8.
 */
/*调用了PEBinFileInfo和PECameraConfig*/
 public class PEBinFile {
    private static final String TAG = "PEBinFile";
    public PEBinFileInfo info = new PEBinFileInfo();//bin文件信息
     public Map<Integer,PECameraConfig> camConfigs = new HashMap<>();//镜头id跟相机配置对照表
     public List<Integer> cidList = new ArrayList<>();//镜头id列表

     PEBinFile(){}
     //构造函数
     public PEBinFile(String path) throws PEError{
        RandomAccessFile pBinFile = PEFile.fopen(path);//打开bin文件
         //获取bin文件信息到info
        info = new PEBinFileInfo(pBinFile);

        Size subSize = getSubSize();//获取子码流尺寸大小
         //依次读取每个模组配置
        for (Integer i = 0; i < info.camCount; i++) {
            PECameraConfig camConfig = PECameraConfig.read(pBinFile);//获取相机配置到camConfig
            camConfig.subSize = subSize;
            camConfigs.put(camConfig.cid,camConfig);//bin文件中一个cid对应一个相机配置，存放次序不同
            cidList.add(camConfig.cid);//例如首先读取出来的可能是2号相机及配置，到时候根据cid找到相应的配置
//            Log.d(TAG, "PEBinFile: cid:"+camConfig.cid);//0,6,5,4,3,2,1,7
        }
        PEFile.fclose(pBinFile);//释放bin文件

    }
    //根据相机型号设置子码流尺寸的大小
    private Size getSubSize(){
        //默认大小为352*288
        Size ret = new Size(Define.CIF_COL,Define.CIF_ROW);
        if (info.model_type == Define.PE_B) {
            ret = new Size(Define.QCIF_COL, Define.QCIF_ROW);
        }
        if(info.model_type == Define.PE_M && info.sub_model_type == 0){
            ret = new Size(Define.QCIF_COL, Define.QCIF_ROW);
        }
        return ret;

    }
}

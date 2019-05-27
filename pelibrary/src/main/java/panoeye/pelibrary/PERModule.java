package panoeye.pelibrary;

/**
 * Created by tir on 2016/12/9.
 * pes文件类
 */
 class PERModule {
//   List<Float> timestampList = new ArrayList<>();//时间戳列表
//   HashMap<Float, Integer> timestampDict = new HashMap<>();//时间戳-文件指针位置对照表
//   List<Float> iFrameList = new ArrayList<>();//I帧列表
//   HashMap<Float, Integer> IFrameDict = new HashMap<>();//时间戳-文件指针位置对照表
   private Float firstIFrameTimestamp = 0.0f;//第一个I帧时间戳
   //设置第一个I帧时间戳
   boolean setFirstIFrameTimestamp(Float timestamp) {
      if (this.firstIFrameTimestamp == 0.0f) {
         this.firstIFrameTimestamp = timestamp;
         return true;
      } else
         return false;
   }
   //取出第一个I帧时间戳
   Float getFirstIFrameTimestamp() {
      return firstIFrameTimestamp;
   }
}

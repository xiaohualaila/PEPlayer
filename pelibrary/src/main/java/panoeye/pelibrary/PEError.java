package panoeye.pelibrary;

/**
 * Created by tir on 2016/12/13.
 */

 public class PEError extends Exception {

     public Integer code;
     public String name;
     public String msg;

    public PEError() {
        code = 0;
        name = "null";
        msg = "null";
    }

    public PEError(Integer code, String name, String msg) {
        this.code = code;
        this.name = name;
        this.msg = msg;
    }


    public static PEError gen(PEErrorType type, String msg) {
        switch (type) {
            case none: return new PEError(0000, "无错误", msg);
            case unknow:return new PEError(0001, "未知错误", msg);
            case notFound:return new PEError(0101, "未找到对象", msg);
            case fileOpenFailed:return new PEError(1101, "文件打开失败", msg);
            case fileSeekFailed:return new PEError(1102, "文件移动失败", msg);
            case fileReadFailed:return new PEError(1103, "文件读取失败", msg);
            case fileWriteFailed:return new PEError(1104, "文件写入失败", msg);
            case fileCloseFailed:return new PEError(1105, "文件关闭失败", msg);
            case fileEofFailed:return new PEError(1106, "文件结尾检查失败", msg);
        }
        return new PEError();
    }
}

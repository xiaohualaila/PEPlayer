package panoeye.pelibrary;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

/**
 * Created by Administrator on 2018/1/22.
 * 此类定义全景的矩形显示模式
 */

public class PESquare {
    public FloatBuffer mVertexBuffer;    // 顶点缓冲
    public FloatBuffer mVertexBuffer5;   // 顶点缓冲
    public FloatBuffer mVertexBuffer8;   // 顶点缓冲
    public FloatBuffer mTextureBuffer;         // 纹理缓冲
    public FloatBuffer mTextureFlipBuffer5;    // 纹理缓冲
    public FloatBuffer mTextureFlipBuffer8;    // 纹理缓冲

    static final float[] VERTEX = {   // in counterclockwise order:
            -1, 0.4f, 0,  // 左上
            -1, -0.4f, 0, // 左下
            -0.4f, -0.4f, 0,  // 右下
            -1, 0.4f, 0,  // 左上
            -0.4f, -0.4f, 0,   // 右下
            -0.4f, 0.4f, 0,   // 右上

            -0.4f, 0.4f, 0,  // 左上
            -0.4f, -0.4f, 0, // 左下
            0.2f, -0.4f, 0,  // 右下
            -0.4f, 0.4f, 0,  // 左上
            0.2f, -0.4f, 0,   // 右下
            0.2f, 0.4f, 0,   // 右上
    };
    static final float[] VERTEX_5 = {   // in counterclockwise order:
            -1, 1, 0,  // 左上
            -1, 0, 0, // 左下
            -0.2f, 0, 0,  // 右下
            -1, 1, 0,  // 左上
            -0.2f, 0, 0,   // 右下
            -0.2f, 1, 0,   // 右上

            -0.2f, 1, 0,  // 左上
            -0.2f, 0, 0, // 左下
            0.6f, 0, 0,  // 右下
            -0.2f, 1, 0,  // 左上
            0.6f, 0, 0,   // 右下
            0.6f, 1, 0,   // 右上

            0.6f, 1, 0,  // 左上
            0.6f, 0, 0, // 左下
            1.4f, 0, 0,  // 右下
            0.6f, 1, 0,  // 左上
            1.4f, 0, 0,   // 右下
            1.4f, 1, 0,   // 右上


            -1, 0, 0,  // 左上
            -1, -1, 0, // 左下
            0.2f, -1, 0,  // 右下
            -1, 0, 0,  // 左上
            0.2f, -1, 0,   // 右下
            0.2f, 0, 0,   // 右上

            0.2f, 0, 0,  // 左上
            0.2f, -1, 0, // 左下
            1.4f, -1, 0,  // 右下
            0.2f, 0, 0,  // 左上
            1.4f, -1, 0,   // 右下
            1.4f, 0, 0,   // 右上
    };
    static final float[] VERTEX_8 = {
            -1, 0.8f, 0,  // 左上
            -1, 0, 0, // 左下
            -0.4f, 0, 0,  // 右下
            -1, 0.8f, 0,  // 左上
            -0.4f, 0, 0,   // 右下
            -0.4f, 0.8f, 0,   // 右上

            -0.4f, 0.8f, 0,  // 左上
            -0.4f, 0, 0, // 左下
            0.2f, 0, 0,  // 右下
            -0.4f, 0.8f, 0,  // 左上
            0.2f, 0, 0,   // 右下
            0.2f, 0.8f, 0,   // 右上

            0.2f, 0.8f, 0,  // 左上
            0.2f, 0, 0, // 左下
            0.8f, 0, 0,  // 右下
            0.2f, 0.8f, 0,  // 左上
            0.8f, 0, 0,   // 右下
            0.8f, 0.8f, 0,   // 右上

            0.8f, 0.8f, 0,  // 左上
            0.8f, 0, 0, // 左下
            1.4f, 0, 0,  // 右下
            0.8f, 0.8f, 0,  // 左上
            1.4f, 0, 0,   // 右下
            1.4f, 0.8f, 0,   // 右上


            -1, 0, 0,  // 左上
            -1, -0.8f, 0, // 左下
            -0.4f, -0.8f, 0,  // 右下
            -1, 0, 0,  // 左上
            -0.4f, -0.8f, 0,   // 右下
            -0.4f, 0, 0,   // 右上

            -0.4f, 0, 0,  // 左上
            -0.4f, -0.8f, 0, // 左下
            0.2f, -0.8f, 0,  // 右下
            -0.4f, 0, 0,  // 左上
            0.2f, -0.8f, 0,   // 右下
            0.2f, 0, 0,   // 右上

            0.2f, 0, 0,  // 左上
            0.2f, -0.8f, 0, // 左下
            0.8f, -0.8f, 0,  // 右下
            0.2f, 0, 0,  // 左上
            0.8f, -0.8f, 0,   // 右下
            0.8f, 0, 0,   // 右上

            0.8f, 0, 0,  // 左上
            0.8f, -0.8f, 0, // 左下
            1.4f, -0.8f, 0,  // 右下
            0.8f, 0, 0,  // 左上
            1.4f, -0.8f, 0,   // 右下
            1.4f, 0, 0,   // 右上
    };

    private static final float[] TEX_VERTEX = {   // in clockwise order:
            // 原始图像贴图（按顶点绘制顺序贴图)，5路倒着，8路向右。
            0, 1f,  // 左上
            0, 0,   // 左下
            1f, 0,  // 右下
            0, 1f,  // 左上
            1f, 0,  // 右下
            1f, 1f, // 右上
    };
    private static final float[] TEX_VERTEX_FLIP_5 = {
            //5路站着(对角线45°)
            0, 0,   // 左下
            0, 1f,  // 左上
            1f, 1f, // 右上
            0, 0,   // 左下
            1f, 1f, // 右上
            1f, 0,  // 右下
            //5路站着(对角线-45°)
//            1f, 0,  // 右下
//            1f, 1f, // 右上
//            0, 1f,  // 左上
//            1f, 0,  // 右下
//            0, 1f,  // 左上
//            0, 0,   // 左下
    };
    private static final float[] TEX_VERTEX_FLIP_8 = {
//            //8路站着(对角线45°)
//            0, 0,   // 左下
//            1f, 0,  // 右下
//            1f, 1f, // 右上
//            0, 0,   // 左下
//            1f, 1f, // 右上
//            0, 1f,  // 左上
            //8路站着(对角线-45°)
            0, 1f,  // 左上
            1f, 1f, // 右上
            1f, 0,  // 右下
            0, 1f,  // 左上
            1f, 0,  // 右下
            0, 0,   // 左下
            //8路倒着(对角线-45°)
//            1f, 0,  // 右下
//            0, 0,   // 左下
//            0, 1f,  // 左上
//            1f, 0,  // 右下
//            0, 1f,  // 左上
//            1f, 1f, // 右上
    };

    public static float[] VR_VERTEX_COORD = {
            -1, -1,
            0, -1,
            0, 1,
            -1, -1,
            0, 1,
            -1, 1,

            0, -1,
            1, -1,
            1, 1,
            0, -1,
            1, 1,
            0, 1,
    };
    public static float[] VR_TEXTURE_COORD = {
            0, 0,
            1, 0,
            1, 1,
            0, 0,
            1, 1,
            0, 1,

            0, 0,
            1, 0,
            1, 1,
            0, 0,
            1, 1,
            0, 1,
    };

    //构造函数
    public PESquare() {
        mVertexBuffer = ByteBuffer.allocateDirect(VERTEX.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(VERTEX);
        mVertexBuffer.position(0);
        mVertexBuffer5 = ByteBuffer.allocateDirect(VERTEX_5.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(VERTEX_5);
        mVertexBuffer5.position(0);
        mVertexBuffer8 = ByteBuffer.allocateDirect(VERTEX_8.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(VERTEX_8);
        mVertexBuffer8.position(0);

        mTextureBuffer = ByteBuffer.allocateDirect(TEX_VERTEX.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(TEX_VERTEX);
        mTextureBuffer.position(0);
        mTextureFlipBuffer5 = ByteBuffer.allocateDirect(TEX_VERTEX_FLIP_5.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(TEX_VERTEX_FLIP_5);
        mTextureFlipBuffer5.position(0);
        mTextureFlipBuffer8 = ByteBuffer.allocateDirect(TEX_VERTEX_FLIP_8.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(TEX_VERTEX_FLIP_8);
        mTextureFlipBuffer8.position(0);
    }
}

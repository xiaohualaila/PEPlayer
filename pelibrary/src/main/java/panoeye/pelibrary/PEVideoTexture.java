package panoeye.pelibrary;

import android.opengl.GLES20;

import java.nio.ByteBuffer;

/**
 * Created by tir on 2016/12/14.
 * 全景视频纹理类
 */

 public class PEVideoTexture {

    private int[] textures;

     PEVideoTexture(PEFrameBuffer frameBuffer,int[] textures ) {
        ByteBuffer buffer;
        ByteBuffer Ybuffer;
        ByteBuffer Ubuffer;
        ByteBuffer Vbuffer;
        Size size;
        size = frameBuffer.size;
        buffer = frameBuffer.get();//获取PEFrameBuffer类对象frameBuffer的成员_imgBuffer到ByteBuffer类对象buffer
        this.textures = textures;
        Ybuffer = ByteBuffer.allocateDirect(size.height * size.width);
        Ubuffer = ByteBuffer.allocateDirect(size.height * size.width/4);
        Vbuffer = ByteBuffer.allocateDirect(size.height * size.width/4);

        frameBuffer.lock();
        buffer.position(0);
        buffer.limit(size.height * size.width);//1228800
        Ybuffer.clear();
        Ybuffer.put(buffer);
        Ybuffer.clear();

        buffer.limit(size.height * size.width + size.height * size.width / 4);//1536000
        Ubuffer.clear();
        Ubuffer.put(buffer);
        Ubuffer.clear();

        buffer.limit(size.height * size.width + size.height * size.width / 2);//1843200
        Vbuffer.clear();
        Vbuffer.put(buffer);
        Vbuffer.clear();
        frameBuffer.unlock();

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[0]);
        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, size.width, size.height, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, Ybuffer);

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[1]);
        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, size.width/2, size.height/2, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, Ubuffer);

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[2]);
        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, size.width/2, size.height/2, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, Vbuffer);

    }
     public void bindTextureY() {
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[0]);
    }

     public void bindTextureU() {
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[1]);
    }

     public void bindTextureV() {
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[2]);
    }

     public void bindTextureA() {
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[3]);
    }


}

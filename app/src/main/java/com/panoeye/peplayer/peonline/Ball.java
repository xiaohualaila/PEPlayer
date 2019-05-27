package com.panoeye.peplayer.peonline;

import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.util.ArrayList;

import panoeye.pelibrary.Define;

/**
 * Created by tir on 2017/4/17.
 */

public class Ball {
    int vCount = 0;
    final float UNIT_SIZE=1f;
    float r = 100f;
    FloatBuffer mVertexBuffer;//顶点坐标数据缓冲
    FloatBuffer mTextureBuffer;

    FloatBuffer vertexStretch;//拉伸面数据
    FloatBuffer vertexStretchFlip;

    public Ball(int vAngle1,int vAngle2){
        initVertexData(vAngle1,vAngle2);
    }

    public void initVertexData(int vAngle1,int vAngle2){
        final int angleSpan = 1;// 将球进行单位切分的角度
        ArrayList<Float> alVertex = new ArrayList<>();//存放顶点坐标的ArrayList
        ArrayList<Float> alTexture = new ArrayList<>();

        ArrayList<Float> alVertexStretch = new ArrayList<>();//拉伸
        ArrayList<Float> alVertexStretchFlip = new ArrayList<>();

        for (int vAngle = vAngle1;vAngle<vAngle2;vAngle = vAngle + angleSpan) {// 垂直方向angleSpan度一份
            for (int hAngle = 0; hAngle < 360; hAngle = hAngle + angleSpan) {// 水平方向angleSpan度一份
                // 纵向横向各到一个角度后计算对应的此点在球面上的坐标
                float x0 = (float) (r * UNIT_SIZE
                        * Math.cos(Math.toRadians(vAngle)) * Math.cos(Math
                        .toRadians(hAngle)));
                float y0 = (float) (r * UNIT_SIZE
                        * Math.cos(Math.toRadians(vAngle)) * Math.sin(Math
                        .toRadians(hAngle)));
                float z0 = (float) (r * UNIT_SIZE * Math.sin(Math
                        .toRadians(vAngle)));

                float x1 = (float) (r * UNIT_SIZE
                        * Math.cos(Math.toRadians(vAngle)) * Math.cos(Math
                        .toRadians(hAngle + angleSpan)));
                float y1 = (float) (r * UNIT_SIZE
                        * Math.cos(Math.toRadians(vAngle)) * Math.sin(Math
                        .toRadians(hAngle + angleSpan)));
                float z1 = (float) (r * UNIT_SIZE * Math.sin(Math
                        .toRadians(vAngle)));

                float x2 = (float) (r * UNIT_SIZE
                        * Math.cos(Math.toRadians(vAngle + angleSpan)) * Math
                        .cos(Math.toRadians(hAngle + angleSpan)));
                float y2 = (float) (r * UNIT_SIZE
                        * Math.cos(Math.toRadians(vAngle + angleSpan)) * Math
                        .sin(Math.toRadians(hAngle + angleSpan)));
                float z2 = (float) (r * UNIT_SIZE * Math.sin(Math
                        .toRadians(vAngle + angleSpan)));

                float x3 = (float) (r * UNIT_SIZE
                        * Math.cos(Math.toRadians(vAngle + angleSpan)) * Math
                        .cos(Math.toRadians(hAngle)));
                float y3 = (float) (r * UNIT_SIZE
                        * Math.cos(Math.toRadians(vAngle + angleSpan)) * Math
                        .sin(Math.toRadians(hAngle)));
                float z3 = (float) (r * UNIT_SIZE * Math.sin(Math
                        .toRadians(vAngle + angleSpan)));
                if (vAngle1>-80){
                    x1 = -x1;
                    x0 = -x0;
                    x2 = -x2;
                    x3 = -x3;
                }
                alTexture.add(x1);
                alTexture.add(y1);

                alTexture.add(x3);
                alTexture.add(y3);
                alTexture.add(x0);
                alTexture.add(y0);
                alTexture.add(x1);
                alTexture.add(y1);
                alTexture.add(x2);
                alTexture.add(y2);
                alTexture.add(x3);
                alTexture.add(y3);
            }
        }

        for (int vAngle = vAngle1;vAngle<vAngle2;vAngle = vAngle + angleSpan){// 垂直方向angleSpan度一份
            for (int hAngle = 0;hAngle<360;hAngle = hAngle + angleSpan){// 水平方向angleSpan度一份
                // 纵向横向各到一个角度后计算对应的此点在球面上的坐标
                float x0 = (float) (r * UNIT_SIZE
                        * Math.cos(Math.toRadians(vAngle)) * Math.cos(Math
                        .toRadians(hAngle)));
                float y0 = (float) (r * UNIT_SIZE
                        * Math.cos(Math.toRadians(vAngle)) * Math.sin(Math
                        .toRadians(hAngle)));
                float z0 = (float) (r * UNIT_SIZE * Math.sin(Math
                        .toRadians(vAngle)));

                float x1 = (float) (r * UNIT_SIZE
                        * Math.cos(Math.toRadians(vAngle)) * Math.cos(Math
                        .toRadians(hAngle + angleSpan)));
                float y1 = (float) (r * UNIT_SIZE
                        * Math.cos(Math.toRadians(vAngle)) * Math.sin(Math
                        .toRadians(hAngle + angleSpan)));
                float z1 = (float) (r * UNIT_SIZE * Math.sin(Math
                        .toRadians(vAngle)));

                float x2 = (float) (r * UNIT_SIZE
                        * Math.cos(Math.toRadians(vAngle + angleSpan)) * Math
                        .cos(Math.toRadians(hAngle + angleSpan)));
                float y2 = (float) (r * UNIT_SIZE
                        * Math.cos(Math.toRadians(vAngle + angleSpan)) * Math
                        .sin(Math.toRadians(hAngle + angleSpan)));
                float z2 = (float) (r * UNIT_SIZE * Math.sin(Math
                        .toRadians(vAngle + angleSpan)));

                float x3 = (float) (r * UNIT_SIZE
                        * Math.cos(Math.toRadians(vAngle + angleSpan)) * Math
                        .cos(Math.toRadians(hAngle)));
                float y3 = (float) (r * UNIT_SIZE
                        * Math.cos(Math.toRadians(vAngle + angleSpan)) * Math
                        .sin(Math.toRadians(hAngle)));
                float z3 = (float) (r * UNIT_SIZE * Math.sin(Math
                        .toRadians(vAngle + angleSpan)));

                // 将计算出来的XYZ坐标加入存放顶点坐标的ArrayList
                alVertex.add(x1);
                alVertex.add(y1);
                alVertex.add(z1);

                alVertexStretch.add((float)(hAngle + angleSpan));
                alVertexStretch.add((float)vAngle);

                alVertexStretchFlip.add((float)(hAngle + angleSpan));
                alVertexStretchFlip.add(-(float)vAngle);

                alVertex.add(x3);
                alVertex.add(y3);
                alVertex.add(z3);

                alVertexStretch.add((float)hAngle);
                alVertexStretch.add((float)(vAngle + angleSpan));

                alVertexStretchFlip.add((float)hAngle);
                alVertexStretchFlip.add(-(float)(vAngle + angleSpan));

                alVertex.add(x0);
                alVertex.add(y0);
                alVertex.add(z0);

                alVertexStretch.add((float)hAngle);
                alVertexStretch.add((float)vAngle);

                alVertexStretchFlip.add((float)hAngle);
                alVertexStretchFlip.add(-(float)vAngle);

                alVertex.add(x1);
                alVertex.add(y1);
                alVertex.add(z1);

                alVertexStretch.add((float)(hAngle + angleSpan));
                alVertexStretch.add((float)vAngle);

                alVertexStretchFlip.add((float)(hAngle + angleSpan));
                alVertexStretchFlip.add(-(float)vAngle);

                alVertex.add(x2);
                alVertex.add(y2);
                alVertex.add(z2);

                alVertexStretch.add((float)(hAngle + angleSpan));
                alVertexStretch.add((float)(vAngle + angleSpan));

                alVertexStretchFlip.add((float)(hAngle + angleSpan));
                alVertexStretchFlip.add(-(float)(vAngle + angleSpan));

                alVertex.add(x3);
                alVertex.add(y3);
                alVertex.add(z3);

                alVertexStretch.add((float)hAngle);
                alVertexStretch.add((float)(vAngle + angleSpan));

                alVertexStretchFlip.add((float)hAngle);
                alVertexStretchFlip.add(-(float)(vAngle + angleSpan));
            }
        }

        vCount = alVertex.size()/3;// 顶点的数量为坐标值数量的1/3，因为一个顶点有3个坐标



        float vertexsStretch[] = new float[vCount*2];//拉伸顶点
        for (int i=0;i<vCount*2;i++){
            vertexsStretch[i] = alVertexStretch.get(i);
        }

        float vertexsStretchFlip[] = new float[vCount*2];//拉伸顶点
        for (int i=0;i<vCount*2;i++){
            vertexsStretchFlip[i] = alVertexStretchFlip.get(i);
        }

        //将alVertex中的坐标值转存到一个float数组中
        float vertexs[] = new float[vCount*3];// 球顶点
        for (int i=0;i<alVertex.size();i++){
            vertexs[i] = alVertex.get(i);
        }

        float texture[] = new float[vCount*2];//纹理坐标
        for (int j = 0;j<vCount*2;){
            //texture[j] = alTexture.get(j)/(2f*r*1.375f)+0.5353f;//1280/(2*(464.35 = r))  圆心x 700/1280
            texture[j] = alTexture.get(j)/(r*2* Define.xRatio)+Define.xCircle;
            j+=2;                                               //圆心坐标实际值有偏差
        }
        for (int j = 1;j<vCount*2;){
            //texture[j] = alTexture.get(j)/(2f*r*1.031f)+0.496f;
            texture[j] = alTexture.get(j)/(r*2*Define.yRatio)+Define.yCircle;
            j+=2;
        }
        Log.d("vCount", "initVertexData: "+vCount);
        // 创建顶点坐标数据缓冲
        // vertices.length*4是因为一个整数四个字节
        ByteBuffer vbb = ByteBuffer.allocateDirect(vertexs.length*4);
        vbb.order(ByteOrder.nativeOrder());//设置字节顺序
        mVertexBuffer = vbb.asFloatBuffer();// 转换为float型缓冲
        mVertexBuffer.put(vertexs);// 向缓冲区中放入顶点坐标数据
        mVertexBuffer.position(0);// 设置缓冲区起始位置

        ByteBuffer Tbb = ByteBuffer.allocateDirect(texture.length*4);
        Tbb.order(ByteOrder.nativeOrder());//设置字节顺序
        mTextureBuffer = Tbb.asFloatBuffer();// 转换为float型缓冲
        mTextureBuffer.put(texture);// 向缓冲区中放入顶点坐标数据
        mTextureBuffer.position(0);// 设置缓冲区起始位置

        ByteBuffer vbbStre = ByteBuffer.allocateDirect(vertexsStretch.length*4);
        vbbStre.order(ByteOrder.nativeOrder());//设置字节顺序
        vertexStretch = vbbStre.asFloatBuffer();// 转换为float型缓冲
        vertexStretch.put(vertexsStretch);// 向缓冲区中放入顶点坐标数据
        vertexStretch.position(0);// 设置缓冲区起始位置

        ByteBuffer vbbStreFlip = ByteBuffer.allocateDirect(vertexsStretchFlip.length*4);
        vbbStreFlip.order(ByteOrder.nativeOrder());//设置字节顺序
        vertexStretchFlip = vbbStreFlip.asFloatBuffer();// 转换为float型缓冲
        vertexStretchFlip.put(vertexsStretchFlip);// 向缓冲区中放入顶点坐标数据
        vertexStretchFlip.position(0);// 设置缓冲区起始位置
    }
}

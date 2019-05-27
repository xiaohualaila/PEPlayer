package panoeye.pelibrary;

/**
 * Created by tir on 2016/12/20.
 */

public class PEMatrix {
     static Integer getIndex(Integer row,Integer col){
        return row*4+col;
    }

     public static void makeVec3(float[] result, float x, float y, float z){
        result[0] = x;
        result[1] = y;
        result[2] = z;
    }

     public static void makeUnit(float[] matrix){
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (i == j) {
                    matrix[getIndex(i, j)] = 1;
                } else {
                    matrix[getIndex(i, j)] = 0;
                }
            }
        }
    }

     static void makeEmpty(float[] matrix){
        for (int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                matrix[getIndex(i,j)] = 0;
            }
        }
    }

     static void makeCopy(float[] result,float[] source){
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                result[getIndex(row, col)] = source[getIndex(row, col)];
            }
        }
    }
     //计算两个矩阵的乘积：m2左乘m1
     public static void calMulti(float[] result, float[] m1, float[] m2){
        makeEmpty(result);
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                for (int k = 0; k < 4; k++) {
//                    result[getIndex(j, i)] += m1[getIndex(k, i)] * m2[getIndex(j, k)];
                    result[getIndex(i,j)] += m2[getIndex(i,k)] * m1[getIndex(k, j)];
                }
            }
        }
    }

     public static void translate(float[] matrix, float[] vec3){
        for (int col = 0; col < 3; col ++) {
            matrix[getIndex(3, col)] = matrix[getIndex(0, col)] * vec3[col]
                    + matrix[getIndex(1, col)] * vec3[col]
                    + matrix[getIndex(2, col)] * vec3[col]
                    + matrix[getIndex(3, col)];
        }
    }

     public static void rotate(float[] matrix, float angle, float[] vec3){
        float rotate[] = new float[16];
        float axis[] = new float[3];
        float temp[] = new float[3];
        double tempp = new Float(angle).doubleValue();
        float c = new Double(Math.cos(tempp)).floatValue();
        float s = new Double(Math.sin(tempp)).floatValue();
        for (int col = 0; col < 3; col++) {
            axis[col] = vec3[col];
        }
        for (int col = 0; col < 3; col++) {
            temp[col] = (1 - c) * axis[col];
        }
        rotate[getIndex(0, 0)] = c + temp[0] * axis[0];
        rotate[getIndex(0, 1)] = 0 + temp[0] * axis[1] + s * axis[2];
        rotate[getIndex(0, 2)] = 0 + temp[0] * axis[2] - s * axis[1];

        rotate[getIndex(1, 0)] = 0 + temp[1] * axis[0] - s * axis[2];
        rotate[getIndex(1, 1)] = c + temp[1] * axis[1];
        rotate[getIndex(1, 2)] = 0 + temp[1] * axis[2] + s * axis[0];

        rotate[getIndex(2, 0)] = 0 + temp[2] * axis[0] + s * axis[1];
        rotate[getIndex(2, 1)] = 0 + temp[2] * axis[1] - s * axis[0];
        rotate[getIndex(2, 2)] = c + temp[2] * axis[2];

        float[] mCopy = new float[16];
        makeCopy(mCopy, matrix);
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                matrix[getIndex(row, col)] = mCopy[getIndex(0, col)] * rotate[getIndex(row, 0)]
                        + mCopy[getIndex(1, col)] * rotate[getIndex(row, 1)]
                        + mCopy[getIndex(2, col)] * rotate[getIndex(row, 2)];
            }
        }
    }

     public static void frustum(float[] matrix,float left,float right,float bottom,float top,float zNear,float zFar){
        makeUnit(matrix);
        matrix[getIndex(0, 0)] = (2 * zNear) / (right - left);
        matrix[getIndex(1, 1)] = (2 * zNear) / (top - bottom);
        matrix[getIndex(2, 0)] = (right + left) / (right - left);
        matrix[getIndex(2, 1)] = (top + bottom) / (top - bottom);
        matrix[getIndex(2, 2)] = -(zFar + zNear) / (zFar - zNear);
        matrix[getIndex(2, 3)] = -1;
        matrix[getIndex(3, 2)] = -(2 * zFar * zNear) / (zFar - zNear);
    }
}

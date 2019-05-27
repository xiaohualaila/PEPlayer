package com.panoeye.peplayer.map;

/**
 * Created by Administrator on 2018/3/14.
 */

public class Camera  {
    int id;
    String IP;
    String bin;
    int cameraType;
    String showName;

    public void setId(int id) {
        this.id = id;
    }

    public void setIP(String IP) {
        this.IP = IP;
    }

    public void setBin(String bin) {
        this.bin = bin;
    }

    public void setCameraType(int cameraType) {
        this.cameraType = cameraType;
    }

    public void setShowName(String showName) {
        this.showName = showName;
    }

    @Override
    public String toString() {
        return "Camera [id=" + id + ",IP=" + IP + ", bin=" + bin + ", cameraType=" + cameraType + "]";
    }

    public int getId() {
        return id;
    }

    public String getIP() {
        return IP;
    }

    public String getBin() {
        return bin;
    }

    public int getCameraType() {
        return cameraType;
    }
}

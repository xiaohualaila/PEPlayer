package com.panoeye.peplayer;

/**
 * Created by Administrator on 2017/12/12.
 */

public class VideoItem {
    private String videoName;
    private int imageId;
    private long videoSize;
    private String videoStartTime;

    public VideoItem(String videoName, int imageId){
        this.videoName = videoName;
        this.imageId = imageId;
    }

    public VideoItem(String videoName, int imageId,long videoSize){
        this.videoName = videoName;
        this.imageId = imageId;
        this.videoSize = videoSize;
    }

    public VideoItem(String videoName, int imageId, long videoSize, String videoStartTime){
        this.videoName = videoName;
        this.imageId = imageId;
        this.videoSize = videoSize;
        this.videoStartTime = videoStartTime;
    }

    public String getVideoName(){
        return videoName;
    }

    public int getImageId(){
        return imageId;
    }

    public float getVideoSize(){
        return videoSize/1024.0f/1024.0f;//单位换算成MB
    }

    public String getVideoStartTime(){
        return videoStartTime;
    }
}

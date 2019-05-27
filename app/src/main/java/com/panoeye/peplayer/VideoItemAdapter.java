package com.panoeye.peplayer;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.List;

/**
 * Created by Administrator on 2017/12/12.
 * VideoItem的适配器类，负责将数据适配到ListView的
 */

public class VideoItemAdapter extends ArrayAdapter<VideoItem> {
    private int resourceId;

//public VideoItemAdapter(@NonNull Context context, int resource, @NonNull List<VideoItem> objects) {
public VideoItemAdapter(Context context, int resource, List<VideoItem> objects) {
    super(context, resource, objects);
        resourceId = resource;
    }

    /**
     * getView()方法：在每个子项被滚动到屏幕内时会被调用
     *
     * @param position
     * @param convertView
     * @param parent
     * @return
     */
    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        //getItem()方法：获取当前项的Fruit实例
        VideoItem videoItem = getItem(position);
        /**
         * 使用LayoutInflater（布局填充器）来为这个子项加载我们传入的布局
         * @Note LayoutInflater的inflate()方法接收3个参数，第3个参数设为false表示只让我们在父布局中声明的layout属性生效，
         *        但不为这个View添加父布局，因为一旦View有了父布局之后，他就不能再添加到ListView中了。
         */
        View view = LayoutInflater.from(getContext()).inflate(resourceId, parent, false);
        ImageView videoImage =  view.findViewById(R.id.video_image);            //调用View的findViewById()方法：获取到ImageView的实例
        TextView videoName =  view.findViewById(R.id.video_name);               //调用View的findViewById()方法：获取到TextView的实例
        TextView videoSize =  view.findViewById(R.id.video_size);               //调用View的findViewById()方法：获取到TextView的实例
        TextView videoStartTime =  view.findViewById(R.id.video_start_time);    //调用View的findViewById()方法：获取到TextView的实例
        videoImage.setImageResource(videoItem.getImageId());                            //setImageResource()方法：设置图片资源
        videoName.setText(videoItem.getVideoName());                                    //setText()方法：设置文字：录像文件名
        if (videoItem.getVideoSize() != 0.0f){
//            videoSize.setText(videoItem.getVideoSize() + "MB |");                           //setText()方法：设置文字：录像大小
            videoSize.setText(String.format("%.1f",videoItem.getVideoSize()) + "MB  |");    //setText()方法：设置文字：录像大小，小数点后保留一位
        }
        if (videoItem.getVideoStartTime()!= null){
            videoStartTime.setText(videoItem.getVideoStartTime());                  //setText()方法：设置文字：录像开始时间
        }
        return  view;
    }
}

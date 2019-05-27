package com.panoeye.peplayer;

import android.app.ProgressDialog;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.Nullable;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;

import java.io.File;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import panoeye.pelibrary.Define;
import panoeye.pelibrary.PEError;
import panoeye.pelibrary.PEManager;

/**
 * Created by Administrator on 2017/10/11.
 */

public class ListVideo extends AppCompatActivity {
    private static final String TAG = "ListVideo";
    WeakReference<ListVideo> weakReference;
    private ListView listView;
    ArrayAdapter adapter;
//    private ArrayList<String> fileList = new ArrayList<>();
    private List<VideoItem> videoItemList = new ArrayList<>();//录像文件列表
    Button listMode;//显示模式
    int currentListMode;//当前显示模式//0-->PER+PES//1-->PER



    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.d(TAG, "onCreate: ");
        setContentView(R.layout.activity_list_video);

        weakReference  = new WeakReference<ListVideo>(this);
        listView = findViewById(R.id.listView);
        listMode = findViewById(R.id.list_mode);
        currentListMode = 1;
        ListOnlyPer(null);


    }
    public void back(View btn){
        this.finish();
    }
    public void ListAllPano(View btn){
//        adapter = new ArrayAdapter<String>(this,android.R.layout.simple_expandable_list_item_1,fileList);
        adapter = new VideoItemAdapter(this,R.layout.video_item,videoItemList);
        listView.setAdapter(adapter);
        listView.setOnItemClickListener(adapterListener);

//        fileList.clear();
        videoItemList.clear();
        File panoDir = new File(Define.ROOT_PATH);
        if (panoDir.exists()){
            File[] files = panoDir.listFiles();//获取该文件夹下所有文件和文件夹
            if (files == null){
                new AlertDialog.Builder(this)
                        .setTitle("Error")
                        .setMessage("Can not read SD card!")
                        .setPositiveButton("OK",null)
                        .setCancelable(false)
                        .show();
            }else {
                for (File file:files){
                    //读取per格式文件到列表
                    String fileName = file.getName();
                    long fileSize = file.length();
                    String prefix = fileName.substring(fileName.lastIndexOf(".")+1);
                    if ((prefix.equals("per"))||(prefix.equals("PER"))&&(file.exists())){
//                        if (file.exists()){
                        if (isMatchPerRecordRegulation(fileName)){
//                            fileList.add(fileName);//per
                            String splitStr[] = fileName.split("_");
//                            StringTokenizer token = new StringTokenizer(fileName,"_");//
                            int year = Integer.parseInt(splitStr[2].substring(0,4));
                            int month = Integer.parseInt(splitStr[2].substring(4,6));
                            int day = Integer.parseInt(splitStr[2].substring(6,8));
                            String Y_M_D = year+"/"+month+"/"+day;
                            int hour = Integer.parseInt(splitStr[3].substring(0,2));
                            int minute = Integer.parseInt(splitStr[3].substring(2,4));
                            int second = Integer.parseInt(splitStr[3].substring(4,6));
                            String H_M_S = hour + ":" + minute;
//                            String H_M_S = hour + ":" + minute + ":" + second;
                            String fileStartTime;
                            if (hour<13){
                                fileStartTime = Y_M_D + " 上午"+ H_M_S;
                            }else {
                                H_M_S = hour-12 + ":" + minute;
//                                H_M_S = hour-12 + ":" + minute + ":" + second;
                                fileStartTime = Y_M_D + " 下午"+ H_M_S;
                            }
                            VideoItem videoItem = new VideoItem(fileName,R.drawable.per_format,fileSize,fileStartTime);
                            videoItemList.add(videoItem);
                        }
                        else {
                            VideoItem videoItem = new VideoItem(fileName,R.drawable.per_format,fileSize);
                            videoItemList.add(videoItem);
                        }
                    }else {
                        //读取pes格式文件到列表
                        File binFile = new File(Define.ROOT_PATH+fileName+"/"+fileName+".bin");
                        if (binFile.exists()){
//                            fileList.add(fileName);
                            VideoItem videoItem = new VideoItem(fileName,R.drawable.pes_format);
                            videoItemList.add(videoItem);
                        }
                    }
                }
            }
        }
        adapter.notifyDataSetChanged();
    }
    public void ListOnlyPes(View btn){
        adapter = new VideoItemAdapter(this,R.layout.video_item_pes,videoItemList);
        listView.setAdapter(adapter);
        listView.setOnItemClickListener(adapterListener);

        videoItemList.clear();
        File panoDir = new File(Define.ROOT_PATH);
        if (panoDir.exists()){
            File[] files = panoDir.listFiles();//获取该文件夹下所有文件和文件夹
            if (files == null){
                new AlertDialog.Builder(this)
                        .setTitle("Error")
                        .setMessage("Can not read SD card!")
                        .setPositiveButton("OK",null)
                        .setCancelable(false)
                        .show();
            }else {
                for (File file:files){
                    String fileName = file.getName();
                    //读取pes格式文件到列表
                    File binFile = new File(Define.ROOT_PATH+fileName+"/"+fileName+".bin");
                    if (binFile.exists()){
                        VideoItem videoItem = new VideoItem(fileName,R.drawable.pes_format);
                        videoItemList.add(videoItem);
                    }
                }
            }
        }
        adapter.notifyDataSetChanged();
    }
    public void ListOnlyPer(View btn){
        adapter = new VideoItemAdapter(this,R.layout.video_item,videoItemList);
        listView.setAdapter(adapter);
        listView.setOnItemClickListener(adapterListener);

        videoItemList.clear();
        File panoDir = new File(Define.ROOT_PATH);
        if (panoDir.exists()){
            File[] files = panoDir.listFiles();//获取该文件夹下所有文件和文件夹
            if (files == null){
                new AlertDialog.Builder(this)
                        .setTitle("Error")
                        .setMessage("Can not read SD card!")
                        .setPositiveButton("OK",null)
                        .setCancelable(false)
                        .show();
            }else {
                for (File file:files){
                    //读取per格式文件到列表
                    String fileName = file.getName();
                    long fileSize = file.length();
                    String prefix = fileName.substring(fileName.lastIndexOf(".")+1);
                    if ((prefix.equals("per"))||(prefix.equals("PER"))&&(file.exists())){
                        if (isMatchPerRecordRegulation(fileName)){
                            String splitStr[] = fileName.split("_");
                            int year = Integer.parseInt(splitStr[2].substring(0,4));
                            int month = Integer.parseInt(splitStr[2].substring(4,6));
                            int day = Integer.parseInt(splitStr[2].substring(6,8));
                            String Y_M_D = year+"/"+month+"/"+day;
                            int hour = Integer.parseInt(splitStr[3].substring(0,2));
                            int minute = Integer.parseInt(splitStr[3].substring(2,4));
                            int second = Integer.parseInt(splitStr[3].substring(4,6));
                            String H_M_S = hour + ":" + minute;
//                            String H_M_S = hour + ":" + minute + ":" + second;
                            String fileStartTime;
                            if (hour<13){
                                fileStartTime = Y_M_D + " 上午"+ H_M_S;
                            }else {
                                H_M_S = hour-12 + ":" + minute;
//                                H_M_S = hour-12 + ":" + minute + ":" + second;
                                fileStartTime = Y_M_D + " 下午"+ H_M_S;
                            }
                            VideoItem videoItem = new VideoItem(fileName,R.drawable.per_format,fileSize,fileStartTime);
                            videoItemList.add(videoItem);
                        }
                        else {
                            VideoItem videoItem = new VideoItem(fileName,R.drawable.per_format,fileSize);
                            videoItemList.add(videoItem);
                        }
                    }
                }
            }
        }
        adapter.notifyDataSetChanged();
    }
    public void changeListMode(View btn){
        switch (currentListMode){
            case 0:
                listMode.setText("仅PER格式");
                ListOnlyPer(null);
                currentListMode =1;//改变显示模式标志
                break;
            case 1:
                listMode.setText("仅PES格式");
                ListOnlyPes(null);
                currentListMode =2;//改变显示模式标志
                break;
            case 2:
                listMode.setText("所有全景格式");
                ListAllPano(null);
                currentListMode =0;//改变显示模式标志
                break;
            default:
                break;
        }
    }

    final private Handler handler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what){
                case 0:
                    pd.dismiss();
            }
        }
    };

    private ProgressDialog pd;
    final private AdapterView.OnItemClickListener adapterListener = new AdapterView.OnItemClickListener() {
        @Override
        public void onItemClick(AdapterView<?> parent, final View view, int position, long id) {
            //Toast.makeText(ListVideo.this,"AdapterView.OnItemClickListener",Toast.LENGTH_SHORT).show();
            final int row = (int)id;
            pd = ProgressDialog.show(ListVideo.this,"loading...","don't rotate screen");
            new Thread(new Runnable() {
                @Override
                public void run() {
//                    String fileName = fileList.get(row);\
                    String fileName = videoItemList.get(row).getVideoName();
                    Log.d("ListVideo", "run:fileName=" + fileName);
                    String prefix = fileName.substring(fileName.lastIndexOf(".")+1);
                    //Log.d("ListVideo", "run:prefix=" + prefix);
                    if (prefix.equals("per")||prefix.equals("PER")){
                        Log.d(TAG, "run: 选择了per录像文件");

                        try {
                            PlayAV.panoHandle = PEManager.buildPer(fileName);
//                            PlayAV.panoHandle = PEManager.buildPano(PanoType.PER,fileName);
//                            PEManager peManager = new PEManager(fileName);
                            Intent intent = new Intent(weakReference.get(), PlayAV.class);
                            startActivity(intent);
                        } catch (PEError peError) {
                            peError.printStackTrace();
                        }
                    }
                    else {
                        Log.d(TAG, "run: 选择了pes录像文件");
                        try {
                            PlayAVS.pesHandle = PEManager.buildPes(fileName);
//                            PlayAVS.pesHandle = PEManager.buildPano(PanoType.PES,fileName);
                            Intent intent = new Intent(weakReference.get(), PlayAVS.class);
                            startActivity(intent);
                        }catch (PEError e){
                            e.printStackTrace();
                        }
                    }
                    handler.sendEmptyMessage(0);
//                    Intent intent = new Intent(weakReference.get(), PlayAV.class);
////                    Bundle bundle = new Bundle();
////                    bundle.putSerializable("NAME", fileList.get(row));
////                    intent.putExtras(bundle);
//                    startActivity(intent);
                }
            }).start();
        }
    };

    /**
     * onCreateOptionsMenu()方法：借助local.xml 创建菜单功能
     * @param menu 菜单
     * @return
     */
//    @Override
//    public boolean onCreateOptionsMenu(Menu menu) {
//        getMenuInflater().inflate(R.menu.local,menu);
//        return true;
//        //return super.onCreateOptionsMenu(menu);
//    }

    /**
     * onOptionsItemSelected()方法：对选中的菜单项进行响应
     * @param item 菜单项
     * @return
     */
//    @Override
//    public boolean onOptionsItemSelected(MenuItem item) {
//        switch (item.getItemId()){
//            case R.id.add_item:
//                Toast.makeText(this,"You clicked Add",Toast.LENGTH_SHORT).show();
//                break;
//            case R.id.remove_item:
//                Toast.makeText(this,"You clicked Remove",Toast.LENGTH_SHORT).show();
//                break;
//            default:
//        }
//        return true;
//        //return super.onOptionsItemSelected(item);
//    }

    /**
     * 判断per录像文件命名是否命名规范
     * @param strFileName 录像文件名
     * @return
     */
    public static boolean isMatchPerRecordRegulation(String strFileName) {
//        String strPattern = "\\w{9}_.{1,}_\\d{4}\\d{2}\\d{2}_\\d{6}_\\d{4}.[pP][eE][rR]";
        String strPattern = "^\\w{9}_.{1,}_\\d{4}\\d{2}\\d{2}_\\d{6}_\\d{4}.[pP][eE][rR]$";//^表示开始 $表示结束
        Pattern p = Pattern.compile(strPattern);
        Matcher m = p.matcher(strFileName);
        return m.matches();
    }


}

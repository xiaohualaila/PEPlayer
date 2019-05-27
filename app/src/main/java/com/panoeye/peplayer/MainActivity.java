package com.panoeye.peplayer;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.design.widget.NavigationView;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.baidu.mapapi.SDKInitializer;
import com.panoeye.peplayer.map.LocationDemo;
import com.panoeye.peplayer.map.MapControlDemo;
import com.panoeye.peplayer.map.OfflineDemo;
import com.panoeye.peplayer.map.PanoMap;
import com.panoeye.peplayer.peonline.FishEyeCfgActivity;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import panoeye.pelibrary.Define;

public class MainActivity extends AppCompatActivity
        implements NavigationView.OnNavigationItemSelectedListener {

    private static final String TAG = "MainActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.d(TAG, "onCreate: ");
        setContentView(R.layout.activity_main);

        // 在使用 百度地图SDK 各组间之前初始化 context 信息，传入 ApplicationContext
        SDKInitializer.initialize(getApplicationContext());

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(
                this, drawer, toolbar, R.string.navigation_drawer_open, R.string.navigation_drawer_close);
        drawer.addDrawerListener(toggle);
        toggle.syncState();

        NavigationView navigationView = (NavigationView) findViewById(R.id.nav_view);
        navigationView.setNavigationItemSelectedListener(this);

        List<String> permissionList = new ArrayList<>();

        if (ContextCompat.checkSelfPermission(MainActivity.this,Manifest.permission.ACCESS_COARSE_LOCATION)!= PackageManager.PERMISSION_GRANTED){
//            ActivityCompat.requestPermissions(MainActivity.this,new String[]{Manifest.permission.ACCESS_COARSE_LOCATION},1);
            permissionList.add(Manifest.permission.ACCESS_FINE_LOCATION);
        }
        if (ContextCompat.checkSelfPermission(MainActivity.this,Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED){
//            ActivityCompat.requestPermissions(MainActivity.this,new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE},1);
            permissionList.add(Manifest.permission.WRITE_EXTERNAL_STORAGE);
        }

        //如果权限列表（需要申请的权限列表）不为空，则申请权限，否则表示权限都有了，直接执行else。
        if (!permissionList.isEmpty()){
            //toArray()方法：List转换成数组
            String[] permissions = permissionList.toArray(new String[permissionList.size()]);
            //requestPermissions()方法：一次性申请数组内的所有权限
            ActivityCompat.requestPermissions(MainActivity.this,permissions,1);
        }

        Button onlineBtn = (Button)findViewById(R.id.onlineBtn);
        onlineBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
//                Intent intent = new Intent(MainActivity.this,ConnectPage.class);
                Intent intent = new Intent(MainActivity.this,OnlineServiceSettingActivity.class);
                startActivity(intent);
            }
        });
        Button listBtn = (Button)findViewById(R.id.listBtn);
        listBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this,ListVideo.class);
                startActivity(intent);
            }
        });

        //直连
//        Intent intent = new Intent(MainActivity.this,ConnectPage.class);
//        startActivity(intent);
        //服务器
//        Intent intent = new Intent(MainActivity.this,OnlineServiceSettingActivity.class);
//        startActivity(intent);
        //录像
//        WeakReference<MainActivity> weakReference;
//        weakReference  = new WeakReference<MainActivity>(this);
//
//        try
//        {
//            PlayAV.panoHandle = PEManager.buildPer("E30010389_入口全景_20170721_140958_0027.per");
//            Intent intent = new Intent(weakReference.get(), PlayAV.class);
//            startActivity(intent);
//        } catch (PEError peError)
//        {
//            peError.printStackTrace();
//        }

    }

    //对权限请求结果的处理
    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        //super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        switch (requestCode){
            case 1:
                if (grantResults.length > 0){
                    for (int result:grantResults){
                        if (result != PackageManager.PERMISSION_GRANTED){
                            Toast.makeText(this,"必须同意所有权限才能使用本程序",Toast.LENGTH_SHORT).show();
                            finish();//如果有任何权限被拒绝，关闭程序。
                            return;
                        }
                    }
                }else {
                    Toast.makeText(this,"发生未知错误",Toast.LENGTH_SHORT).show();
                    finish();//发生未知错误，关闭程序。
                }
                break;
            default:
        }
    }

    boolean checkWritePermission(){
        File panoDir = new File(Define.ROOT_PATH);
        if (!panoDir.exists()) {
            if (panoDir.mkdirs() == false) {
                new AlertDialog.Builder(this)
                        .setTitle("Warning")
                        .setMessage("Can not read SD card!")
                        .setPositiveButton("Cancel", null)
                        .setCancelable(false)
                        .show();
                return false;
            }
        }
        return true;
    }

    private void getVideoList(){
            Intent intent = new Intent(MainActivity.this, ListVideo.class);
            startActivity(intent);
    }

    @Override
    public boolean onNavigationItemSelected(@NonNull MenuItem item) {
        // Handle navigation view item clicks here.
        int id = item.getItemId();

        if (id == R.id.nav_camera) {
            Intent intent = new Intent(MainActivity.this, OfflineDemo.class);
            startActivity(intent);
        } else if (id == R.id.nav_gallery) {
            Intent intent = new Intent(MainActivity.this, PanoMap.class);
//            Intent intent = new Intent(MainActivity.this, BaseMapDemo.class);
            startActivity(intent);
        } else if (id == R.id.nav_slideshow) {
            Intent intent = new Intent(MainActivity.this, LocationDemo.class);
            startActivity(intent);
        } else if (id == R.id.nav_manage) {
            Intent intent = new Intent(MainActivity.this, MapControlDemo.class);
            startActivity(intent);
        } else if (id == R.id.nav_share) {
//            Intent intent = new Intent(MainActivity.this, BMap.class);
            Intent intent = new Intent(MainActivity.this,ConnectPage.class);
            startActivity(intent);
        } else if (id == R.id.nav_send) {
            Intent intent = new Intent(MainActivity.this, FishEyeCfgActivity.class);
            startActivity(intent);
        }

        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        drawer.closeDrawer(GravityCompat.START);
        return true;
    }
}

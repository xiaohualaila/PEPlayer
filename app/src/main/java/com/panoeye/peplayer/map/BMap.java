package com.panoeye.peplayer.map;

import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.Intent;
import android.content.res.XmlResourceParser;
import android.graphics.Color;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;

import com.baidu.location.BDLocation;
import com.baidu.location.BDLocationListener;
import com.baidu.location.LocationClient;
import com.baidu.location.LocationClientOption;
import com.baidu.mapapi.map.BaiduMap;
import com.baidu.mapapi.map.BitmapDescriptor;
import com.baidu.mapapi.map.BitmapDescriptorFactory;
import com.baidu.mapapi.map.CircleOptions;
import com.baidu.mapapi.map.InfoWindow;
import com.baidu.mapapi.map.MapPoi;
import com.baidu.mapapi.map.MapStatus;
import com.baidu.mapapi.map.MapStatusUpdateFactory;
import com.baidu.mapapi.map.MapView;
import com.baidu.mapapi.map.Marker;
import com.baidu.mapapi.map.MarkerOptions;
import com.baidu.mapapi.map.MyLocationConfiguration;
import com.baidu.mapapi.map.MyLocationData;
import com.baidu.mapapi.model.LatLng;
import com.panoeye.peplayer.peonline.ConnectManager;
import com.panoeye.peplayer.PlayOnline;
import com.panoeye.peplayer.R;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;

import java.io.IOException;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.Collections;

import panoeye.pelibrary.Define;

import static com.panoeye.peplayer.ConnectPage.Init;
import static com.panoeye.peplayer.ConnectPage.connectManager;
//import com.panoeye.peplayer.ConnectPage.handler;

/**
 * Created by Administrator on 2018/3/8.
 */

public class BMap extends AppCompatActivity implements SensorEventListener {

    private static final String TAG = "BMap";
    // 定位相关
    LocationClient mLocClient;
    public MyLocationListenner myListener = new MyLocationListenner();
    private MyLocationConfiguration.LocationMode mCurrentMode;
    BitmapDescriptor mCurrentMarker;
    private static final int accuracyCircleFillColor = 0xAAFFFF88;
    private static final int accuracyCircleStrokeColor = 0xAA00FF00;
    private SensorManager mSensorManager;
    private Double lastX = 0.0;
    private int mCurrentDirection = 0;
    private double mCurrentLat = 0.0;
    private double mCurrentLon = 0.0;
    private float mCurrentAccracy;

    MapView mMapView;
    BaiduMap mBaiduMap;

    // UI相关
    RadioGroup.OnCheckedChangeListener radioButtonListener;
    Button requestLocButton;
    boolean isFirstLoc = true; // 是否首次定位
    private MyLocationData locData;
    private float direction;

    // 初始化全局 bitmap 信息，不用时及时 recycle
    BitmapDescriptor bdA = BitmapDescriptorFactory
            .fromResource(R.drawable.icon_marka);
    BitmapDescriptor bdB = BitmapDescriptorFactory
            .fromResource(R.drawable.icon_markb);
    BitmapDescriptor bdC = BitmapDescriptorFactory
            .fromResource(R.drawable.icon_markc);
    BitmapDescriptor bdD = BitmapDescriptorFactory
            .fromResource(R.drawable.icon_markd);
    BitmapDescriptor bd = BitmapDescriptorFactory
            .fromResource(R.drawable.icon_gcoding);
    BitmapDescriptor bdGround = BitmapDescriptorFactory
            .fromResource(R.drawable.ground_overlay);

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_location);
        requestLocButton = (Button) findViewById(R.id.button1);
        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);//获取传感器管理服务
        //设置定位模式
        mCurrentMode = MyLocationConfiguration.LocationMode.NORMAL;
        requestLocButton.setText("普通");
        View.OnClickListener btnClickListener = new View.OnClickListener() {
            public void onClick(View v) {
                switch (mCurrentMode) {
                    case NORMAL:
                        requestLocButton.setText("跟随");
                        mCurrentMode = MyLocationConfiguration.LocationMode.FOLLOWING;
                        mBaiduMap.setMyLocationConfiguration(new MyLocationConfiguration(
                                mCurrentMode, true, mCurrentMarker));
                        MapStatus.Builder builder = new MapStatus.Builder();
                        builder.overlook(0);
                        mBaiduMap.animateMapStatus(MapStatusUpdateFactory.newMapStatus(builder.build()));
                        break;
                    case COMPASS:
                        requestLocButton.setText("普通");
                        mCurrentMode = MyLocationConfiguration.LocationMode.NORMAL;
                        mBaiduMap.setMyLocationConfiguration(new MyLocationConfiguration(
                                mCurrentMode, true, mCurrentMarker));
                        MapStatus.Builder builder1 = new MapStatus.Builder();
                        builder1.overlook(0);
                        mBaiduMap.animateMapStatus(MapStatusUpdateFactory.newMapStatus(builder1.build()));
                        break;
                    case FOLLOWING:
                        requestLocButton.setText("罗盘");
                        mCurrentMode = MyLocationConfiguration.LocationMode.COMPASS;
                        mBaiduMap.setMyLocationConfiguration(new MyLocationConfiguration(
                                mCurrentMode, true, mCurrentMarker));
                        break;
                    default:
                        break;
                }
                int ret = mLocClient.requestLocation();
                Log.e("log", "onClick: "+ret+mLocClient.isStarted());//false
                if (!mLocClient.isStarted())
                {
//                        mLocClient.stop();
//                        mLocClient.start();
//                        mLocClient.restart();
                    Log.d("log", "onClick: "+mLocClient.isStarted());//false
                }
            }
        };
        requestLocButton.setOnClickListener(btnClickListener);

        RadioGroup group = (RadioGroup) this.findViewById(R.id.radioGroup);
        radioButtonListener = new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup group, int checkedId) {
                if (checkedId == R.id.defaulticon) {
                    // 传入null则，恢复默认图标
                    mCurrentMarker = null;
                    mBaiduMap.setMyLocationConfiguration(new MyLocationConfiguration(
                            mCurrentMode, true, null));
                }
                if (checkedId == R.id.customicon) {
                    // 修改为自定义marker(定位图标)
                    mCurrentMarker = BitmapDescriptorFactory .fromResource(R.drawable.arrow);//icon_geo
                    //设置自定义样式（定位模式，是否开启方向，自定义图标样式，自定义精度圈填充颜色，自定义精度圈边框颜色）
                    mBaiduMap.setMyLocationConfiguration(new MyLocationConfiguration(
                            mCurrentMode, true, mCurrentMarker,
                            accuracyCircleFillColor, accuracyCircleStrokeColor));
                }
            }
        };
        group.setOnCheckedChangeListener(radioButtonListener);

        // 地图初始化
        mMapView = (MapView) findViewById(R.id.bmapView);
        mBaiduMap = mMapView.getMap();
        // 开启定位图层
        mBaiduMap.setMyLocationEnabled(true);
        // 定位初始化
        mLocClient = new LocationClient(this);
        mLocClient.registerLocationListener(myListener);

//        mLocClient.registerNotifyLocationListener(myListener);
        LocationClientOption option = new LocationClientOption();
//        option.setOpenGps(true); // 打开gps
        option.setCoorType("bd09ll"); // 设置坐标类型
        option.setScanSpan(0);// 可选，默认0，即仅定位一次，设置发起定位请求的间隔需要大于等于1000ms才是有效的
        option.setLocationMode(LocationClientOption.LocationMode.Battery_Saving);
//        option.setServiceName("com.baidu.location.service_v2.9");
        option.setIgnoreKillProcess(true);// 可选，默认true，定位SDK内部是一个SERVICE，并放到了独立进程，设置是否在stop的时候杀死这个进程，默认不杀死
        //可选，定位SDK内部是一个service，并放到了独立进程。
        //设置是否在stop的时候杀死这个进程，默认（建议）不杀死，即setIgnoreKillProcess(true)
        mLocClient.setLocOption(option);
        mLocClient.start();

        if (isWiFiActive(this)){
            LatLng llnb = new LatLng(29.925738,121.637416);
            MapStatus.Builder builder = new MapStatus.Builder();
            builder.target(llnb).zoom(17.0f);
            mBaiduMap.animateMapStatus(MapStatusUpdateFactory.newMapStatus(builder.build()));
        }
/*        if (!isNetworkActive()){
            LatLng llnb = new LatLng(29.925738,121.637416);
            MapStatus.Builder builder = new MapStatus.Builder();
            builder.target(llnb).zoom(17.0f);
            mBaiduMap.animateMapStatus(MapStatusUpdateFactory.newMapStatus(builder.build()));
        }*/

//        Intent startIntent = new Intent(this,com.baidu.location.f.class);
//        startService(startIntent);

        Log.e("log", "onCreate: "+mLocClient.isStarted());//false
        Log.e("log", "onCreate: "+mLocClient.getVersion());//7.2.1

        initOverlay();
        initListener();
    }
    public void initOverlay() {
        // add marker overlay
        LatLng llA = new LatLng(29.92235, 121.633338);
        LatLng llB = new LatLng(29.927743, 121.641416);
        LatLng llC = new LatLng(29.92335, 121.641416);
        LatLng llD = new LatLng(29.925743, 121.637416);

        logXmlData();
        Bundle bundle = new Bundle();
        bundle.putString("IP",IP);
        bundle.putString("bin",bin);
        MarkerOptions ooA = new MarkerOptions().position(llA).icon(bdA)
                .zIndex(9).draggable(true).extraInfo(bundle);
//                if (animationBox.isChecked()) {
        // 掉下动画
        ooA.animateType(MarkerOptions.MarkerAnimateType.drop);
//                }
        mMarkerA = (Marker) (mBaiduMap.addOverlay(ooA));
        MarkerOptions ooB = new MarkerOptions().position(llB).icon(bdB)
                .zIndex(5);
//                if (animationBox.isChecked()) {
        // 掉下动画
        ooB.animateType(MarkerOptions.MarkerAnimateType.drop);
//                }
        mMarkerB = (Marker) (mBaiduMap.addOverlay(ooB));
    }

    private Marker mMarkerA;
    private Marker mMarkerB;
    private Marker mMarkerC;
    private Marker mMarkerD;
    private InfoWindow mInfoWindow;
    String IP;
    String bin;
    private void initListener() {
        mBaiduMap.setOnMapTouchListener(new BaiduMap.OnMapTouchListener() {

            @Override
            public void onTouch(MotionEvent event) {

            }
        });


        mBaiduMap.setOnMapClickListener(new BaiduMap.OnMapClickListener() {
            /**
             * 单击地图
             */
            public void onMapClick(LatLng point) {
//                touchType = "单击地图";
//                currentPt = point;
//                updateMapState();

                Toast.makeText(BMap.this,"点击位置：" + point.latitude + ", " + point.longitude,Toast.LENGTH_LONG).show();
            }

            /**
             * 单击地图中的POI点
             */
            public boolean onMapPoiClick(MapPoi poi) {
//                touchType = "单击POI点";
//                currentPt = poi.getPosition();
//                updateMapState();
                return false;
            }
        });
        mBaiduMap.setOnMapLongClickListener(new BaiduMap.OnMapLongClickListener() {
            /**
             * 长按地图
             */
            public void onMapLongClick(LatLng point) {
//                touchType = "长按";
//                currentPt = point;
//                updateMapState();

                MarkerOptions ooD = new MarkerOptions().position(point).icon(bd)
                        .zIndex(9).draggable(true);
//                if (animationBox.isChecked()) {
                // 掉下动画
                //ooD.animateType(MarkerOptions.MarkerAnimateType.drop);
//                }
                mMarkerD = (Marker) (mBaiduMap.addOverlay(ooD));

                //从xml创建要显示的View，并设置相应的值
                LayoutInflater inflater = LayoutInflater.from(getApplicationContext());
                View view = inflater.inflate(R.layout.layout_map_item, null);
                TextView txtLatLng = (TextView)view.findViewById(R.id.text_item_latlng);
                final EditText background = (EditText) view.findViewById(R.id.ed_item_background);
                final EditText keyWord = (EditText) view.findViewById(R.id.ed_item_keyword);
                Button btnSearch = (Button) view.findViewById(R.id.btn_search);
                Button btnCancel = (Button) view.findViewById(R.id.btn_cancel);
                txtLatLng.setText("纬度："+point.latitude+",经度:"+point.longitude);
                final LatLng lngFinal = point;

                //点击view上面的检索按钮调用方法
                btnSearch.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        //Log.d("WorkMainActivity","搜索附近500米");
                        mBaiduMap.clear();//清空地图上的标记
                        String circumBackground = background.getText().toString();
                        if(null==circumBackground||"".equals(circumBackground)){
                            return;
                        }else {
                            String keyWordString = keyWord.getText().toString();
                            if(null==keyWordString||"".equals(keyWordString)){
                                return;
                            }else {
                                int circum = Integer.parseInt(circumBackground);
//                                PoiNearbySearchOption poiNearbySearchOption = new PoiNearbySearchOption();
//                                poiNearbySearchOption.location(lngFinal);
                                //以长按坐标点为中心，画指定半径的圆，并制定透明度为100，作为搜索范围
                                CircleOptions circleOptions = new CircleOptions();
                                circleOptions.center(lngFinal);
                                circleOptions.radius(circum);
                                circleOptions.fillColor(Color.argb(100,28,95,167));
                                mBaiduMap.addOverlay(circleOptions);
//                                poiNearbySearchOption.keyword(keyWordString);
//                                poiNearbySearchOption.radius(circum);
//                                PoiSearch poiSearch = PoiSearch.newInstance();
//                                poiSearch.searchNearby(poiNearbySearchOption);
//                                poiSearch.setOnGetPoiSearchResultListener(WorkMainActivity.this);
                            }
                        }
                    }
                });
                //点击取消按钮
                btnCancel.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        //Log.d("WorkMainActivity","取消搜索");
                        mBaiduMap.hideInfoWindow();
                    }
                });
                InfoWindow infoWindow = new InfoWindow(view, lngFinal, -47);
                mBaiduMap.showInfoWindow(infoWindow);
            }
        });

        mBaiduMap.setOnMarkerClickListener(new BaiduMap.OnMarkerClickListener() {
            public boolean onMarkerClick(final Marker marker) {
                Button button = new Button(getApplicationContext());
                button.setBackgroundResource(R.drawable.popup);
                InfoWindow.OnInfoWindowClickListener listener = null;
                if (marker == mMarkerA) {
                    button.setText("播放全景");
                    button.setTextColor(Color.BLACK);
                    button.setWidth(300);

//                    logXmlData();

                    listener = new InfoWindow.OnInfoWindowClickListener() {
                        public void onInfoWindowClick() {
//                            LatLng ll = marker.getPosition();
//                            LatLng llNew = new LatLng(ll.latitude + 0.005,
//                                    ll.longitude + 0.005);
//                            marker.setPosition(llNew);
                            mBaiduMap.hideInfoWindow();
                            playPano(mMarkerA.getExtraInfo().getString("IP"),mMarkerA.getExtraInfo().getString("bin"));
                        }
                    };
                    LatLng ll = marker.getPosition();
                    mInfoWindow = new InfoWindow(BitmapDescriptorFactory.fromView(button), ll, -47, listener);
                    mBaiduMap.showInfoWindow(mInfoWindow);
                }else if (marker == mMarkerB) {
                    button.setText("更改图标");
                    button.setTextColor(Color.BLACK);
                    button.setOnClickListener(new View.OnClickListener() {
                        public void onClick(View v) {
                            marker.setIcon(bd);
                            mBaiduMap.hideInfoWindow();
                        }
                    });
                    LatLng ll = marker.getPosition();
                    mInfoWindow = new InfoWindow(button, ll, -47);
                    mBaiduMap.showInfoWindow(mInfoWindow);
                }else if (marker == mMarkerD) {
                    button.setText("删除");
                    button.setTextColor(Color.BLACK);
                    button.setOnClickListener(new View.OnClickListener() {
                        public void onClick(View v) {
                            marker.remove();
                            mBaiduMap.hideInfoWindow();
                        }
                    });
                    LatLng ll = marker.getPosition();
                    mInfoWindow = new InfoWindow(button, ll, -47);
                    mBaiduMap.showInfoWindow(mInfoWindow);
                }
                return true;
            }
        });
    }

    private void logXmlData() {
        XmlResourceParser xmlParser = getResources().getXml(R.xml.cameramap);

        try {
            int event = xmlParser.getEventType();   //先获取当前解析器光标在哪
            while (event != XmlPullParser.END_DOCUMENT){    //如果还没到文档的结束标志，那么就继续往下处理
                switch (event){
                    case XmlPullParser.START_DOCUMENT:
                        Log.i(TAG,"xml解析开始");
                        break;
                    case XmlPullParser.START_TAG:
                        //一般都是获取标签的属性值，所以在这里数据你需要的数据
                        Log.d(TAG,"当前标签是："+xmlParser.getName());
                        if (xmlParser.getName().equals("camera0")){
                            //两种方法获取属性值
                            Log.d(TAG,"第一个属性：" + xmlParser.getAttributeName(0)+ ": "
                                    + xmlParser.getAttributeValue(0));
                            Log.d(TAG,"第二个属性：" + xmlParser.getAttributeName(1)+ ": "
                                    + xmlParser.getAttributeValue(1));
//                            Log.d(TAG,"第一个属性：" + xmlParser.getAttributeName(0)+": "
//                                    + xmlParser.getAttributeValue(null,"ip"));
//                            Log.d(TAG,"第二个属性：" + xmlParser.getAttributeName(1)+": "
//                                    + xmlParser.getAttributeValue(null,"bin"));
                        }
                        if(xmlParser.getName().equals("camera")) {
                            Log.d(TAG,"第一个属性：" + xmlParser.getAttributeName(0)+": "
                                    + xmlParser.getAttributeValue(null,"id"));
                            if (xmlParser.getAttributeValue(null,"id").equals("1")){
                                event = xmlParser.next();   //将当前解析器光标往下一步移
                                if(xmlParser.getName().equals("ip")){
                                    event = xmlParser.next();   //将当前解析器光标往下一步移
                                    IP = xmlParser.getText();
                                    Log.e(TAG, "logXmlData: IP:"+IP);
                                    event = xmlParser.next();   //将当前解析器光标往下一步移
                                }
                                event = xmlParser.next();   //将当前解析器光标往下一步移
                                if(xmlParser.getName().equals("bin")){
                                    event = xmlParser.next();   //将当前解析器光标往下一步移
                                    bin = xmlParser.getText();
                                    Log.e(TAG, "logXmlData: bin:"+bin);
                                }
                            }
                        }
                        break;
                    case XmlPullParser.TEXT:
                        Log.d(TAG,"Text:" + xmlParser.getText());
                        break;
                    case XmlPullParser.END_TAG:
                        break;
                    default:
                        break;
                }
                event = xmlParser.next();   //将当前解析器光标往下一步移
            }
        } catch (XmlPullParserException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }



    final private Handler handler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what){
                case 0:
                    pd.dismiss();
                    Toast.makeText(BMap.this," 相机连接成功！",Toast.LENGTH_SHORT).show();
                    break;
                case 1:
                    pd.dismiss();
                    AlertDialog.Builder builder = new AlertDialog.Builder(BMap.this);  //先得到构造器
                    builder.setTitle("错误提示"); //设置标题
                    builder.setMessage("网络连接错误，请检查网络连接！"); //设置内容
                    //builder.setIcon(R.mipmap.ic_launcher);//设置图标，图片id即可
                    builder.create();
                    builder.create().show();
                    break;
                case 2:
                    pd.dismiss();
                    connectManager.closeRTSP();

                    builder = new AlertDialog.Builder(BMap.this);  //先得到构造器
                    builder.setTitle("超時提示"); //设置标题
                    builder.setMessage("网络连接超时，请重试！"); //设置内容
                    //builder.setIcon(R.mipmap.ic_launcher);//设置图标，图片id即可
                    builder.create();
                    builder.create().show();
                    break;
            }
        }
    };
    private ProgressDialog pd;
    private void playPano(String IP, final String serialNo) {
        Log.d(TAG, "playPano: IP:"+IP);
        Log.d(TAG, "playPano: serialNo:"+serialNo);
        pd = ProgressDialog.show(this,"loading...","please wait...");

        final String head = "rtsp://admin:123456@";
//        final String IP = IPAddr.getText().toString();
//        final String IP = "192.168.20.178";
        final int[] port = {12341};
        final String end = "/mpeg4";
        int fishPort = 554;
        String fishEnd = "/profile2";
        final String[] rtspAddr =new String[Define.cameraCount];

        for (int cid=0;cid<Define.cameraCount;cid++){
            rtspAddr[cid] = head+IP+":"+ port[0] +end;
            Log.d(TAG, "port: "+port[0]);
            port[0]++;
            if (Define.cameraCount == 1){
                rtspAddr[cid] = head+IP+":"+ fishPort +fishEnd;
            }
        }

        final Thread connThread = new Thread(new Runnable() {
            @Override
            public void run() {
//                android.os.Process.setThreadPriority (android.os.Process.THREAD_PRIORITY_LOWEST);
//                android.os.Process.setThreadPriority (android.os.Process.THREAD_PRIORITY_BACKGROUND);
//                Thread.currentThread().setPriority(4);
//                Log.e(TAG, "run:getPriority(): "+Thread.currentThread().getPriority());
                Init(Define.cameraCount);
                connectManager = new ConnectManager();
                for (int cid=0;cid<Define.cameraCount;cid++){
                    final int finalHandle = cid;
                    new Thread(new Runnable(){
                        @Override
                        public void run() {
//                            android.os.Process.setThreadPriority (android.os.Process.THREAD_PRIORITY_BACKGROUND);
//                            Thread.currentThread().setPriority(6);
//                            Log.d(TAG, "run: getPriority(): "+Thread.currentThread().getPriority()+";cid:"+finalHandle);
                            int ret = connectManager.InitRTSP(rtspAddr[finalHandle],finalHandle);
                            Log.d(TAG, "run: cid:"+finalHandle+";ret-->"+ret);
                            if (ret == 0){
                                connectManager.flagsDict.put(finalHandle,"Succeed");
                                Log.d(TAG,"模组"+ finalHandle +"-->RTSPConnect()成功!");
                            }else {
                                connectManager.flagsDict.put(finalHandle,"Failed");
                                Log.e(TAG,"模组"+ finalHandle +"-->RTSPConnect()失败!");
                                connectManager.connErrCnt++;
                            }
                        }
                    }).start();
                }

                int sleepCount = 0;
                while (true){
                    Log.d(TAG, "onCreate: true循环，连接相机中...");
                    Log.d(TAG, "RTSPConnect(): flags.size()-->:"+connectManager.flagsDict.size());
                    if (connectManager.flagsDict.size() == Define.cameraCount){
                        if (connectManager.connErrCnt != Define.cameraCount){
                            Intent intent = new Intent(BMap.this,PlayOnline.class);
                            Bundle bundle = new Bundle();
//                            bundle.putSerializable("serialNo",serialNo.getText().toString());
//                            bundle.putSerializable("serialNo","E10010078");
                            bundle.putSerializable("serialNo",serialNo);
                            intent.putExtras(bundle);
                            startActivity(intent);
                            handler.sendEmptyMessage(0);
                        }else {
                            handler.sendEmptyMessage(1);
                        }
                        break;
                    }
                    try {
                        sleepCount++;
                        Log.d(TAG, "onCreate: sleepCount:"+sleepCount);
                        if (sleepCount>=5){
                            handler.sendEmptyMessage(2);
                            break;
                        }
//                        Thread.yield();
                        Thread.sleep(2000);//线程休眠2s
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        });
//        connThread.setPriority(4);
        connThread.start();
    }

    @Override
    public void onSensorChanged(SensorEvent sensorEvent) {
        double x = sensorEvent.values[SensorManager.DATA_X];
        if (Math.abs(x - lastX) > 1.0) {
            mCurrentDirection = (int) x;
//            Log.d(TAG, "onSensorChanged:方向："+mCurrentDirection);
            locData = new MyLocationData.Builder()
                    .accuracy(mCurrentAccracy)
                    // 此处设置开发者获取到的方向信息，顺时针0-360
                    .direction(mCurrentDirection)
                    .latitude(mCurrentLat)
                    .longitude(mCurrentLon)
                    .build();
            mBaiduMap.setMyLocationData(locData);
        }
        lastX = x;
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int i) {

    }

    /**
     * 定位SDK监听函数
     */
    public class MyLocationListenner implements BDLocationListener {

        @Override
        public void onReceiveLocation(BDLocation location) {
            Log.e("log", "onReceiveLocation: ");
            // map view 销毁后不在处理新接收的位置
            if (location == null || mMapView == null) {
                return;
            }
            mCurrentLat = location.getLatitude();
            mCurrentLon = location.getLongitude();
            mCurrentAccracy = location.getRadius();
            Log.d("log", "onReceiveLocation:纬度:"+mCurrentLat+",经度:"+mCurrentLon+",精度:"+mCurrentAccracy);
            locData = new MyLocationData.Builder()
                    .latitude(mCurrentLat)
                    .longitude(mCurrentLon)
                    .accuracy(mCurrentAccracy)
                    // 此处设置开发者获取到的方向信息，顺时针0-360
                    .direction(mCurrentDirection)
                    .build();
            mBaiduMap.setMyLocationData(locData);
            if (isFirstLoc) {
                isFirstLoc = false;
                LatLng ll = new LatLng(location.getLatitude(),location.getLongitude());
//                LatLng llnb = new LatLng(29.925738,121.637416);
                MapStatus.Builder builder = new MapStatus.Builder();
                builder.target(ll).zoom(18.0f);
                mBaiduMap.animateMapStatus(MapStatusUpdateFactory.newMapStatus(builder.build()));
            }


            if (location.getLocType() == BDLocation.TypeGpsLocation) {
                // GPS类型定位结果
                location.getSpeed();
                // 速度 单位：km/h，注意：网络定位结果是没有速度的
                location.getSatelliteNumber();
                // 卫星数目，gps定位成功最少需要4颗卫星
                location.getAltitude();
                //海拔高度 单位：米
                location.getGpsAccuracyStatus();
                // gps质量判断
            }
            else if (location.getLocType() == BDLocation.TypeNetWorkLocation) {
                // 网络类型定位结果
                if (location.hasAltitude()) {
                    //如果有海拔高度
                    location.getAltitude();//单位：米//
                }
                location.getOperators();//运营商信息
            } else if (location.getLocType() == BDLocation.TypeOffLineLocation) {// 离线定位结果
                // 离线定位成功，离线定位结果也是有效的;
                Log.d("log", "onReceiveLocation: 离线定位成功");
            } else if (location.getLocType() == BDLocation.TypeServerError) {
                //服务端网络定位失败，可以反馈IMEI号和大体定位时间到loc-bugs@baidu.com;
            } else if (location.getLocType() == BDLocation.TypeNetWorkException) {
                //网络不同导致定位失败，请检查网络是否通畅;
            } else if (location.getLocType() == BDLocation.TypeCriteriaException) {
                //无法获取有效定位依据导致定位失败，一般是由于手机的原因，处于飞行模式下一般会造成这种结果，可以试着重启手机;
            }
        }

//        public void onReceivePoi(BDLocation poiLocation) {
//        }
    }

    @Override
    protected void onPause() {
        mMapView.onPause();
        super.onPause();
    }

    @Override
    protected void onResume() {
        mMapView.onResume();
        super.onResume();
        //为系统的方向传感器注册监听器
        mSensorManager.registerListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_ORIENTATION),
                SensorManager.SENSOR_DELAY_UI);
    }

    @Override
    protected void onStop() {
        //取消注册传感器监听
        mSensorManager.unregisterListener(this);
        super.onStop();
    }

    @Override
    protected void onDestroy() {
        // 退出时销毁定位
        mLocClient.stop();
        // 关闭定位图层
        mBaiduMap.setMyLocationEnabled(false);
        mMapView.onDestroy();
        mMapView = null;
        super.onDestroy();
    }

    public static boolean isWiFiActive(Context context) {
        WifiManager mWifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
        WifiInfo wifiInfo = mWifiManager.getConnectionInfo();
        int ipAddress = wifiInfo == null ? 0 : wifiInfo.getIpAddress();
        String ip = intToIp(ipAddress);
        Log.e(TAG, "isWiFiActive: ip-->" + ip );
        if (mWifiManager.isWifiEnabled() && ipAddress != 0) {
            System.out.println("**** WIFI is on");
            return true;
        } else {
            System.out.println("**** WIFI is off");
            return false;
        }
    }
    public static String intToIp(int ipInt) {
        StringBuilder sb = new StringBuilder();
        sb.append(ipInt & 0xFF).append(".");
        sb.append((ipInt >> 8) & 0xFF).append(".");
        sb.append((ipInt >> 16) & 0xFF).append(".");
        sb.append((ipInt >> 24) & 0xFF);
        return sb.toString();
    }

    public static boolean isNetworkActive(Context context) {
        String ip;
        ConnectivityManager conMann = (ConnectivityManager)
                context.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo mobileNetworkInfo = conMann.getNetworkInfo(ConnectivityManager.TYPE_MOBILE);
        NetworkInfo wifiNetworkInfo = conMann.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
        if (mobileNetworkInfo.isConnected()) {
            ip = getLocalIpAddress();
            System.out.println("本地ip-----"+ip);
            return true;
        }else if(wifiNetworkInfo.isConnected()) {
            WifiManager wifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
            WifiInfo wifiInfo = wifiManager.getConnectionInfo();
            int ipAddress = wifiInfo.getIpAddress();
            ip = intToIp(ipAddress);
            System.out.println("wifi_ip地址为------"+ip);
            return true;
        }
        return false;
    }
    private static String getLocalIpAddress() {
        try {
            String ipv4;
            ArrayList<NetworkInterface> nilist = Collections.list(NetworkInterface.getNetworkInterfaces());
            for (NetworkInterface ni: nilist)
            {
                ArrayList<InetAddress>  ialist = Collections.list(ni.getInetAddresses());
                for (InetAddress address: ialist){
                    if (!address.isLoopbackAddress())
                    {
                        ipv4=address.getHostAddress();
                        return ipv4;
                    }
                }
            }
        } catch (SocketException ex) {
            Log.e("localip", ex.toString());
        }
        return null;
    }
}

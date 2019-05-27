package com.panoeye.peplayer.peonline;

import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;

import com.panoeye.peplayer.R;

import panoeye.pelibrary.Define;

public class FishEyeCfgActivity extends AppCompatActivity {
    String input1,input2,head,end,str_ip,str_port;
    EditText RTSPaddress1,RTSPaddress2,xRatio,yRatio,xCircle,yCircle,ip,port;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_fisheye_cfg);

        head = "rtsp://admin:123456@";
        end = "/mpeg4";
        RTSPaddress1 = (EditText)findViewById(R.id.RTSPAddress2);
        RTSPaddress1.setText("192.168.111.11:12341");
        RTSPaddress2 = (EditText)findViewById(R.id.RTSPAddress1);
//        RTSPaddress2.setText("192.168.20.13:554");
        ip = (EditText)findViewById(R.id.ip);
        ip.setText("192.168.111.199");
        port = (EditText)findViewById(R.id.port);
        port.setText("5000");

        xRatio = (EditText)findViewById(R.id.xRatio);
        xRatio.setText("2.1");

        yRatio = (EditText)findViewById(R.id.yRatio);
        yRatio.setText("1.5");

        xCircle = (EditText)findViewById(R.id.xCircle);
        xCircle.setText("0.4921");
        Define.xCircle = Float.parseFloat(xCircle.getText().toString());

        yCircle = (EditText)findViewById(R.id.yCircle);
        yCircle.setText("0.4927");
        Define.yCircle = Float.parseFloat(yCircle.getText().toString());
    }
    public void start(View btn){
        Define.xRatio = Float.parseFloat(xRatio.getText().toString());
        Define.yRatio = Float.parseFloat(yRatio.getText().toString());

        input1 = RTSPaddress1.getText().toString();
        if (input1.equals("") ==false){
            input1 = head + input1 + end;
        }
        input2 = RTSPaddress2.getText().toString();
        if (input2.equals("") == false){
            input2 = head + input2 + end;
        }


        if (input1.equals("")&& input2.equals("")){
            new AlertDialog.Builder(this)
                    .setTitle("Error")
                    .setMessage("RTSP Addr is null")
                    .setPositiveButton("OK!",null)
                    .setCancelable(false)
                    .show();
        }else {
//            Intent intent = new Intent(this,FishEyeDrawActivity.class);
//            Bundle bundle = new Bundle();
//            bundle.putSerializable("RTSPAddr1",input1);
//            bundle.putSerializable("RTSPAddr2",input2);
//
//            str_ip = ip.getText().toString();
//            if (str_ip.equals("")==false){
//                bundle.putSerializable("ip",str_ip);
//            }
//            str_port = port.getText().toString();
//            if (str_port.equals("")==false){
//                bundle.putSerializable("port",str_port);
//            }
//
//            intent.putExtras(bundle);
//            startActivity(intent);
        }

    }
}

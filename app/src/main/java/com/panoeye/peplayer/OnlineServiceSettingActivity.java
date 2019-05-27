package com.panoeye.peplayer;

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.TextView;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;


/**
 * Created by tir on 2016/10/5.
 */
public class OnlineServiceSettingActivity extends AppCompatActivity {

    TextView ipField, portField;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_onlineservicesetting);
        ipField = findViewById(R.id.ipField);
        portField = findViewById(R.id.portField);
        SharedPreferences sPerfs= PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
        ipField.setText(sPerfs.getString("IP", ""));
        portField.setText(sPerfs.getString("PORT", ""));

//        ipField.setText("192.168.20.99");
//        portField.setText("5555");
//        next(null);
    }

    public void back(View btn) {
        this.finish();
    }

    public void next(View btn) {
        int port = Integer.parseInt(portField.getText().toString());
        Intent intent = new Intent(this, OnlineUserSettingActivity.class);
        Bundle bundle = new Bundle();
        bundle.putSerializable("IP", ipField.getText().toString());
        bundle.putSerializable("PORT", port);
        intent.putExtras(bundle);
        SharedPreferences sPerfs= PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
        SharedPreferences.Editor editor = sPerfs.edit();
        editor.putString("IP", ipField.getText().toString());
        editor.putString("PORT", portField.getText().toString());
        editor.commit();
        startActivity(intent);
    }
}

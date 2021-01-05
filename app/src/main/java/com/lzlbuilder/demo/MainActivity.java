package com.lzlbuilder.demo;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

import com.lzlbuilder.cdemo.TestNative;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Log.e("lzl_test",new TestNative().test()+"ddd");
    }

}

package com.lzlbuilder.cdemo;

/**
 * Native 测试类
 */
public class TestNative {

    static {
        System.loadLibrary("native-lib");
    }

    public native String test();

}

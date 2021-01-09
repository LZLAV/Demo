//
// Created by liuzl15 on 2021/1/5.
//
#include "./jni_common/jni_wrapper.h"
/*
 * 函数的第二个参数是指对象，倘若定义为 static 方法，则需要表明是类对象 jclass
 * C++ 风格，后缀为 cpp
 *
 */
extern "C" JNIEXPORT jstring JNICALL Java_com_lzlbuilder_cdemo_TestNative_test(JNIEnv *env, jobject obj)
{
    char ca[]="Hello JNI From C++";
    //C++ 风格 JNI 调用
    return env->NewStringUTF(ca);
}

/**
 * C 语言风格,不建议使用，Android Studio 会提示错误
 * 错误使用 C++ 和 C ，会提示 member reference base type 'JNIEnv' (aka 'const struct JNINativeInterface *') is not a structure or union 错误
 */
//JNIEXPORT jstring JNICALL Java_com_lzlbuilder_cdemo_TestNative_test(JNIEnv *env, jobject obj)
//{
//    char ca[]="Hello JNI From C";
//    return (*env)->NewStringUTF(env,ca);
//}

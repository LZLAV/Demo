//
// Created by liuzl15 on 2021/1/5.
//
#include <jni.h>
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

/*
 * Jni 反射调用 Java 方法
 */



/*
 * JNI 使用 DVM表映射的方式关联方法
 */




/*
 * JNI 多线程中，如何保证对象的一致性
 *  在JNI_OnLoad中，缓存JavaVM*。 那是跨线程唯一持久且有效的。
 *      设置一些本机线程后，立即调用AttachCurrentThread并获取 JNIEnv*，该命令仅对该单个线程有效。
 *      使用JavaVM*和JNIEnv*，查找您的jclass es，jobject s和jmethodID s。 这些仅对您连接的单线程仍然有效。
 *      将jclass es和jobject s转换为全局引用，以便它在线程之间持久存在。 jmethodID不需要全球化，它们不是工作对象。
 *      在任何其他本机线程上，您再次需要调用AttachCurrentThread以获得该线程的有效JNIEnv*。
 *      当您不再需要创建的全局引用时，不要忘记删除它们(最晚在JNI_OnUnload中)
 */
// java虚拟机的JavaVM指针是整个jvm公用
JavaVM *gs_jvm;
jobject gl_object;
void  setEnev(JNIEnv *env, jobject obj)
{
    //全局有效的变量
    int retGvm=env->GetJavaVM(&gs_jvm);
    //设置为全局变量
    gl_object=env->NewGlobalRef(obj);
}
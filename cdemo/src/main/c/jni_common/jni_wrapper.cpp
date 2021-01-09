//
// Created by liuzl15 on 2021/1/6.
//

#include <malloc.h>
#include <string.h>
#include "jni_wrapper.h"

/**
 * JNI基础知识
 *  -多线程
 *  -JNIEnv 与 JavaVM
 *  -引用
 *  -异常
 *  -Log
 *  -类
 *  -反射
 *  -本地方法
 *  -字符串处理
 *  -数组处理
 *  -NIO
 */


/**
 * 多线程相关
 *      同步问题
 *          JNI 提供了监视器机制，与 Java 中的 synchronized 相同，用来对临界区进行保护性访问
 *          jint MonitorEnter(JNIEnv *env, jobject obj);    //进入一个临界区
 *              obj：用来充当锁的Java对象
 *
 *          jint MonitorExit(JNIEnv *env, jobject obj);     //退出临界区
 *
 *              if ((*env)->MonitorEnter(env, obj) != JNI_OK) {
 *                  //进入临界区成功
 *              }
 *
 *              //临界区
 *              if ((*env)->MonitorExit(env, obj) != JNI_OK) {
 *                  //退出临界区成功
 *              };
 */
static jmethodID THREAD_WAIT;
static jmethodID THREAD_NOTIFY;
static jmethodID THREAD_NOTYFY_ALL;
void initThread(JNIEnv *env, jobject lock) {
    jclass cls = env->GetObjectClass(lock);
    THREAD_WAIT = env->GetMethodID(cls, "wait", "(J)V");
    THREAD_NOTIFY = env->GetMethodID(cls, "notify", "(V)V");
    THREAD_NOTYFY_ALL = env->GetMethodID(cls, "notifyAll", "(V)V");
}
void wait(JNIEnv *env, jobject lock, jlong timeout) {
    env->CallVoidMethod(lock, THREAD_WAIT, timeout);
}
void notify(JNIEnv *env, jobject lock) {
    env->CallVoidMethod(lock, THREAD_NOTIFY);
}
void notifyAll(JNIEnv *env, jobject lock) {
    env->CallVoidMethod(lock, THREAD_NOTYFY_ALL);
}

/**
 * JavaVM 与 JNIEnv
 *
 *      JNIEnv
 *          JNIEnv类型是一个指向全部JNI方法的指针。该指针只在创建它的线程有效，不能跨线程传递。
 *
 *      JavaVM
 *          JavaVM是虚拟机在JNI中的表示，一个JVM中只有一个JavaVM对象，这个对象是线程共享的。
 *
 *          jint GetJavaVM(JNIEnv *env, JavaVM **vm);   //通过 JNIEnv 可以获取一个 Java 虚拟机对象
 *          jint JNI_CreateJavaVM(JavaVM **p_vm, void **p_env, void *vm_args);  //调用JNI_CreateJavaVM创建JVM的线程被称为主线程。理论上来说，此方法不允许用户调用。
 *
 *      JNIEnv指针仅在创建它的线程有效。如果我们需要在其他线程访问JVM，那么必须先调用AttachCurrentThread将当前线程与JVM进行关联，然后才能获得JNIEnv对象。
 *          jint AttachCurrentThread(JavaVM* vm , JNIEnv** env , JavaVMAttachArgs* args);   //链接到虚拟机，当前线程与 JVM 进行关联
 *          jint DetachCurrentThread(JavaVM* vm);   //解除与虚拟机的连接
 *              struct JavaVMAttachArgs {
 *                  jint        version;    // must be >= JNI_VERSION_1_2
 *                  const char* name;       // NULL or name of thread as modified UTF-8 str
 *                  jobject     group;      // global ref of a ThreadGroup object, or NULL
 *              };
 *
 *         jint DestroyJavaVM(JavaVM* vm);      //卸载当前使用的虚拟机
 *         jint JNI_GetDefaultJavaVMInitArgs(void *vm_args);    //获取默认虚拟机初始化参数
 *         jint JNI_GetCreatedJavaVMs(JavaVM **vmBuf, jsize bufLen, jsize *nVMs);       //获取到已经被创建的Java虚拟机对象
 *              vmBuf：用来保存Java虚拟机的缓冲区
 *
 *     动态加载本地方法
 *          负责Java方法和本地C函数的链接
 *          JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved);      //加载本地方法
 *              //JNI_OnLoad方法在每一个.so库中只能存在一个
 *          JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved);        //卸载本地方法
 *
 *
 */

extern "C"
JNIEXPORT jstring JNICALL getName(JNIEnv *env, jobject thiz, int number) {
    printf("number is %d\n",number);
    return env->NewStringUTF("hello world");
}
static const char *CLASS_NAME = "com/github/cccxm/NativeLib";//类名
static JNINativeMethod method = {//本地方法描述
        "getName",//Java方法名
        "(I)Ljava/lang/String;",//方法签名
        (void *) getName //绑定本地函数
};
static bool
bindNative(JNIEnv *env) {
    jclass clazz;
//    clazz = env->FindClass(CLASS_NAME);
    if (clazz == NULL) {
        return false;
    }
    return env->RegisterNatives(clazz, &method, 1) == 0;
}
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    jint result = -1;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return result;
    }
    bool res = bindNative(env);
    printf("bind result is %s\n",res?"ok":"error");
    // 返回jni的版本
    return JNI_VERSION_1_6;
}

static bool
unBindNative(JNIEnv *env) {
    jclass clazz;
//    clazz = env->FindClass(CLASS_NAME);
    if (clazz == NULL) {
        return false;
    }
    return env->UnregisterNatives(clazz) == 0;
}
JNIEXPORT void JNICALL
JNI_OnUnload(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    jint result = -1;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return;
    }
    bool res = unBindNative(env);
    printf("unbind result is %s\n", res ? "ok" : "error");
}


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
void setEnev(JNIEnv *env, jobject obj) {
    //全局有效的变量
    int retGvm = env->GetJavaVM(&gs_jvm);
    //设置为全局变量，不会被GC
    //gl_object = env->NewGlobalRef(obj);
}

JNIEnv* getEnvForCurrentThread(JavaVM *jvm) {
    JNIEnv *env;

    if (jvm == NULL) {
        return NULL;
    } else if (jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        // The current thread isn't attached to a Java thread, this could happen when
        // mistakenly calling JniHelpers functions directly from other C++ code.
        return NULL;
    }

    return env;
}

JNIEnv* attachCurrentThreadToJVM(const char* thread_name) {
    JNIEnv *env;
    JavaVMAttachArgs args;
    int result = -1;

    args.version = JNI_VERSION_1_6;
    args.name = const_cast<char*>(thread_name);
    args.group = NULL;

#ifdef ANDROID
    // 调用AttachCurrentThread将当前线程与JVM进行关联，然后才能获得JNIEnv对象
    result = gs_jvm->AttachCurrentThread(&env, &args);
#else
    result = sJavaVm->AttachCurrentThread((void**)(&env), &args);
#endif
    return env;
}

JNIEnv* attachCurrentThreadAsDaemonToJVM(const char* thread_name) {
    JNIEnv *env;
    JavaVMAttachArgs args;
    int result = -1;

    args.version = JAVA_VERSION;
    args.name = const_cast<char*>(thread_name);
    args.group = NULL;

#ifdef ANDROID
    //新创建的线程设置为守护线程
    result = gs_jvm->AttachCurrentThreadAsDaemon(&env, &args);
#else
    result = sJavaVm->AttachCurrentThreadAsDaemon((void**)(&env), &args);
#endif
    if (result != JNI_OK) {
//        JavaExceptionUtils::throwRuntimeException(env, "Could not attach daemon thread %s to JVM", thread_name);
        return NULL;
    }

    return env;
}

void detatchCurrentThreadFromJVM() {
    //取消当前线程与 JVM 的关联
    gs_jvm->DetachCurrentThread();
}


/**
 *  引用
 *
 *  jobjectRefType GetObjectRefType(JNIEnv* env, jobject obj);      //获取一个对象的引用类型
 *      typedef enum jobjectRefType {
 *          JNIInvalidRefType = 0,//无效引用
 *          JNILocalRefType = 1,//局部引用
 *          JNIGlobalRefType = 2,//全局引用
 *          JNIWeakGlobalRefType = 3//弱全局引用
 *     } jobjectRefType;
 *
 *  局部引用，被JVM分配管理，并占用JVM的资源。native 方法返回后自动回收
 *      env->NewLocalRef(obj) 、 FindClass、NewObject、GetObjectClass和NewCharArray 等
 *      env->DeleteLocalRef(obj);
 *      局部引用管理
 *          EnsureLocalCapacity
 *          PushLocalFrame、PopLocalFrame
 *
 *  全局引用，不受 JVM 统一管理
 *      env->NewGlobalRef(&obj);
 *      env->DeleteGlobalRef(obj);
 *  弱全局引用
 *      env->NewWeakGlobalRef(&obj);
 *      env->DeleteWeakGlobalRef(obj);
 *      // 判断该弱全局引用是否还存在
 *      env->IsSameObject(obj, NULL)
 */

/**
 * JNI 抛出 Java 异常
 *      jint Throw(JNIEnv *env, jthrowable obj);    //抛出一个 Throwable 对象异常
 *      jint ThrowNew(JNIEnv *env, jclass clazz,const char *message);   //抛出一个新异常对象
 *      jthrowable ExceptionOccurred(JNIEnv *env);  //获取 JVM 抛出的异常
 *      void ExceptionDescribe(JNIEnv *env);    //输出错误日志
 *      void ExceptionClear(JNIEnv *env);   //清空当前产生的全部异常信息
 *      void FatalError(JNIEnv *env, const char *msg);  //产生一个严重错误
 *      jboolean ExceptionCheck(JNIEnv *env);  //检查是否存在异常信息
 */
typedef enum {
    JavaOutOfMemoryError = 1,
    JavaIOException,
    JavaRuntimeException,
    JavaIndexOutOfBoundsException,
    JavaArithmeticException,
    JavaIllegalArgumentException,
    JavaNullPointerException,
    JavaDirectorPureVirtual,
    JavaUnknownError
} JavaExceptionCodes;

typedef struct {
    JavaExceptionCodes code;
    const char *java_exception;
} JavaExceptions_t;

static void JavaThrowException(JNIEnv *jenv, JavaExceptionCodes code, const char *msg) {
    jclass excep;
    static const JavaExceptions_t java_exceptions[] = {
            {JavaOutOfMemoryError,          "java/lang/OutOfMemoryError"},
            {JavaIOException,               "java/io/IOException"},
            {JavaRuntimeException,          "java/lang/RuntimeException"},
            {JavaIndexOutOfBoundsException, "java/lang/IndexOutOfBoundsException"},
            {JavaArithmeticException,       "java/lang/ArithmeticException"},
            {JavaIllegalArgumentException,  "java/lang/IllegalArgumentException"},
            {JavaNullPointerException,      "java/lang/NullPointerException"},
            {JavaDirectorPureVirtual,       "java/lang/RuntimeException"},
            {JavaUnknownError,              "java/lang/UnknownError"},
            {(JavaExceptionCodes) 0,        "java/lang/UnknownError"}
    };
    const JavaExceptions_t *except_ptr = java_exceptions;

    while (except_ptr->code != code && except_ptr->code)
        except_ptr++;

    //清理 JVM 中的异常
    jenv->ExceptionClear();
    excep = jenv->FindClass(except_ptr->java_exception);
    if (excep)
        //抛出Java 异常
        jenv->ThrowNew(excep, msg);
}


/**
 * Android Log
 */
#if ENABLE_LOGGING
#if ANDROID
#include <android/log.h>
#define LOGGING_TAG "JniHelpers"
#if ENABLE_LOGGING_DEBUG
#define LOG_DEBUG(...) __android_log_print(ANDROID_LOG_INFO, LOGGING_TAG, __VA_ARGS__)
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, LOGGING_TAG, __VA_ARGS__)
#else
#define LOG_DEBUG(...)
#define LOG_INFO(...)
#endif
#define LOG_WARN(...) __android_log_print(ANDROID_LOG_WARN, LOGGING_TAG, __VA_ARGS__)
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, LOGGING_TAG, __VA_ARGS__)
#else
#if ENABLE_LOGGING_DEBUG
#define LOG_DEBUG(...) fprintf(stderr, "DEBUG: "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n")
#define LOG_INFO(...) fprintf(stderr, "INFO: "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n")
#else
#define LOG_DEBUG(...)
#define LOG_INFO(...)
#endif
#define LOG_WARN(...) fprintf(stderr, "WARN: "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n")
#define LOG_ERROR(...) fprintf(stderr, "ERROR: "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n")
#endif
#else
#define LOG_DEBUG(...)
#define LOG_INFO(...)
#define LOG_WARN(...)
#define LOG_ERROR(...)
#endif


/**
 * Java 类
 *  API
 *      class
 *
 *          jclass DefineClass(JNIEnv *env, const char *name, jobject loader,const jbyte *buf, jsize bufLen);   //通过类定义来加载一个类
 *          jclass FindClass(const char* name);     //通过名字直接找类
 *          jclass GetObjectClass(jobject obj);     //通过对象获取类
 *          jcalss GetSuperclass(jclass sub)    //获取父类
 *          jboolean IsAssignableFrom(jclass sub, jclass sup);  //判断这两种类型是否可以互相转换
 *          jboolean IsInstanceOf(jobject obj, jclass clazz);   //判断一个对象是否是一个类的实例
 *
 *      object
 *          jobject NewObject(JNIEnv *env, jclass clazz,jmethodID methodID, ...);
 *          jobject NewObjectA(JNIEnv *env, jclass clazz,jmethodID methodID, const jvalue *args);   //需要传入参数数组
 *          jobject NewObjectV(JNIEnv *env, jclass clazz,jmethodID methodID, va_list args);     //指向变参列表的指针
 *          jboolean IsSameObject(JNIEnv *env, jobject ref1,jobject ref2);      //判断两个引用是否指向同一个对象
 *
 *
 *      method
 *
 *          jmethod GetMethodID(jclass clazz,const char *name,const char *sig);     //获取实例方法ID
 *          jmethod GetStaticMethodID(jclass clazz,const char* name,const char *sig);       //获取静态方法
 *          [NativeType] Call[Type]Method(jobject obj, jmethodID methodID,…);       //调用实例方法
 *          [NativeType] Call[Type]MethodV(jobject obj,jmethodID methodID，va_list args);
 *          [NativeType] Call[Type]MethodA(jobject obj, jmethodID methodID, const jvalue *args);
 *          [NativeType] CallStatic[Type]Method(jclass clazz, jmethodID methodID, …);       //调用静态方法
 *          [NativeType]  CallStatic[Type]MethodV(jclass clazz, jmethodID methodID, va_list args);
 *          [NativeType]  CallStatic[Type]MethodA(jclass clazz, jmethodID methodID, const jvalue *args);
 *          [NativeType] CallNonvirtual[Type]Method(jobject obj, jclass clazz, jmethodID methodID, …);      //调用非虚实例方法
 *          [NativeType]  CallNonvirtual[Type]MethodV(jobject obj, jclass clazz, jmethodID methodID, va_list args);
 *          [NativeType]  CallNonvirtual[Type]MethodA(jobject obj, jclass clazz, jmethodID methodID, const jvalue *args);
 *
 *      field
 *          jfieldID GetFieldID(jclass clazz, const char *name, const char *sig);       //获取属性ID
 *          jfieldID GetStaticFieldID(jclass clazz,const char *name, const char *sig);  //获取静态属性 ID
 *          [NativeType] Get[Type]Field(jobject obj, jfieldID fieldID);     //获取属性值
 *          void Set[Type]Field(jobject obj,jfieldID fieldID, [NativeType] val);    //设置属性值
 *          [NativeType] GetStatic[Type]Field(jclass clazz,jfieldID fieldID);   //获取静态属性值
 *          void SetStatic[Type]Field(jclass clazz, jfieldID fieldID, [NativeType] value);  //设置静态属性值
 */

/**
 * 反射
 *      在仅知道名称的情况下就能操作方法和属性
 *
 *
 *      jfieldID FromReflectedField(jobject field);     //将一个Field对象转换为属性ID
 *      jobject ToReflectedField(jclass cls, jfieldID fieldID, jboolean isStatic);  //反射得到Field对象
 *
 *      jmethodID FromReflectedMethod(jobject method);  //将一个Method对象转换为方法ID
 *      jobject ToReflectedMethod(jclass cls, jmethodID methodID, jboolean isStatic);   //反射得到Method对象
 *
 */


/**
 * 本地方法
 *      [ReturnType]  (*fnPtr)(JNIEnv *env, jobject ObjectOrClass, …);      //本地方法对应的函数
 *      jint RegisterNatives(jclass clazz, const JNINativeMethod *methods, jint nMethods);  //注册一个本地方法
 *          把这些方法声明为"static", 这样这些方法名就不会在设备符号表里占用空间
 *      jint UnregisterNatives(jclass clazz);       //接触本地方法，这个方法会导致本地库的重新加载和链接
 */


/**
 * String 相关
 *
 *      jstring NewString(JNIEnv *env, const jchar *unicodeChars,jsize len);    //生成 Java字符串
 *              unicodeChars ：一个指向Unicode编码的字符数组的指针
 *      jsize GetStringLength(JNIEnv *env, jstring string);     //获取Java 字符串的长度
 *      jstring NewStringUTF(JNIEnv *env, const char *bytes);       //新建UTF-8编码字符串
 *      jsize GetStringUTFLength(JNIEnv *env, jstring string);  //获取UTF-8字符串的长度
 *
 *      //生成副本
 *      const jchar* GetStringChars(jstring string, jboolean *isCopy);      //从Java字符串获取字符数组
 *      const char* GetStringUTFChars(jstring string, jboolean *isCopy );   //获取UTF-8编码的Java字符串的
 *      void ReleaseStringChars(jstring string, const jchar *chars);        //释放从Java字符串中获取的字符数组
 *      void ReleaseStringUTFChars(jstring string, const char *utf);        //释放从UTF-8字符串中获取的字符数组
 *
 *      //传送到字符缓存器
 *      void GetStringRegion(jstring str, jsize start, jsize len, jchar *buf);      //从Java字符串中截取一段字符
 *      void GetStringUTFRegion(jstring str, jsize start, jsize len, char *buf);    //从UTF-8字符串中截取一段字符
 *
 *      //直接字符串指针，两函数之间不能存在任何让线程阻塞的操作
 *      const jchar* GetStringCritical(jstring string, jboolean *isCopy);
 *      void ReleaseStringCritical(jstring string,const jchar *carray);
 */


/*
	java String --> C char *
*/
static char* jstringTostring(JNIEnv* env, jstring jstr)
{
    char* rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("utf-8");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr= (jbyteArray)env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0)
    {
        rtn = (char*)malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = '\0';
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    return rtn;
}

/**
 * C字符串转java字符串
 */
jstring Str2Jstring(JNIEnv *env, const char *pStr) {
    int strLen = strlen(pStr);
    jclass jstrObj = env->FindClass("java/lang/String");
    jmethodID methodId = env ->GetMethodID(jstrObj, "<init>", "([BLjava/lang/String;)V");
    jbyteArray byteArray = env ->NewByteArray(strLen);
    jstring encode = env ->NewStringUTF("utf-8");
    env ->SetByteArrayRegion(byteArray, 0, strLen, (jbyte *) pStr);
    return (jstring) env ->NewObject(jstrObj, methodId, byteArray, encode);
}


/**
 * Array相关
 *
 *      //获取数组的长度
 *      jsize GetArrayLength(j[Type]array array);       //Type用基本类型替换
 *
 *  用于对象数组的函数
 *      //创建指定长度的数组
 *      jobjectArray NewObjectArray(jsize length, jclass elementClass, jobject initialElement );
 *      // 获得数值中指定 index 的元素
 *      jobject GetObjectArrayElement(jobjectArray array, jsize Index);
 *      // 设置数组中指定 index 的元素
 *      void SetObjectArrayElement(jobjectArray array, jsize index,jobject value);
 *
 *  用于基本类型数组的函数
 *      //创建指定长度的数组
 *      [ArrayType] New[Type]Array(jsize length);
 *      //获得数组元素，返回一个指向本机类型数组的指针
 *      [NativeType] *Get[Type]ArrayElements([ArrayType] array, jboolean *isCopy);
 *      //释放数组指针，指向本机类型数组的指针
 *      void Release[Type]ArrayElements([ArrayType] array，[NativeType] *elems, jint mode);
 *
 *          mode:
 *              0   :   复制内容，回收本机数组的存储
 *              JNI_COMMIT  复制内容，不回收本机数组的存储
 *              JNI_ABORT   不复制内容，回收本机数组的存储
 *
 *      //截取数组
 *      void Get[Type]ArrayRegion([ArrayType] array, jsize start, jsize len, [NativeType] *buf);
 *      //范围设置数组
 *      void Set[Type]ArrayRegion([ArrayType] array, jsize start, jsize len, [NativeType] *buf);
 *
 *      //操作基本数据类型数组的直接指针，在函数对之间要保证不能进行任何可能会导致线程阻塞的操作。由于GC 的运行会打断线程，所以在此期间任何调用 GC 的线程都会被阻塞
 *      //锁的作用，有锁则 isCopy 置为 NULL 或 JNI_FALSE;无锁则置为 JNI_TRUE
 *      void *GetPrimitiveArrayCritical([ArrayType] array, jboolean *isCopy);
 *      //释放返回的数组指针
 *      void ReleasePrimitiveArrayCritical([ArrayType] array, void *carray, jint mode);
 *          mode:
 *              0   :   复制内容，释放本机数组的存储
 *              JNI_COMMIT  复制内容，不释放本机数组的存储
 *              JNI_ABORT   不复制内容，释放本机数组的存储
 *
 *
 */



/**
 * NIO
 *      NIO 是直接地址访问，绕过JVM直接操作内存的方式极大地提高了程序的运行效率
 *
 *      jobject NewDirectByteBuffer(JNIEnv* env, void* address, jlong capacity);    //将一个内存区域作为直接内存缓冲区
 *          return : ByteBuffer 对象的局部引用，当发生异常时返回 NULL
 *      void* GetDirectBufferAddress(JNIEnv* env, jobject buf);     //获取直接缓存区地址
 *          buf：java.nio.ByteBuffer对象
 *          return：直接缓冲区的地址指针，发生异常时返回NULL
 *      jlong GetDirectBufferCapacity(JNIEnv* env, jobject buf);    //获取直接缓冲区容量
 *          buf：java.nio.ByteBuffer对象
 *          return：缓冲区容量，发生异常时返回-1
 */


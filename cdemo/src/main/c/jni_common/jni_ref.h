//
// Created by liuzl15 on 2021/1/6.
//

#ifndef DEMO_JNI_REF_H
#define DEMO_JNI_REF_H

#include "jni_wrapper.h"

/**
 * Jni 引用相关内容
 *  -局部引用
 *      通过 NewLocalRef 和各种 JNI 接口创建（FindClass、NewObject、GetObjectClass和NewCharArray等）。
 *      会阻止 GC 回收所引用的对象，不在本地函数中跨函数使用，不能跨线程使用。
 *      函数返回后局部引用所引用的对象会被JVM 自动释放，或调用 DeleteLocalRef 释放。
 *      -管理局部引用
 *          任何实现 JNI 规范的 JVM，必须确保每个本地函数至少可以创建 16 个局部引用（可以理解为虚拟机默认支持创建 16 个局部引用）
 *              EnsureLocalCapacity
 *                  该函数的作用确保在当前线程中创建指定数量的局部引用，如果创建成功则返回 0，否则创建失败，并抛出 OutOfMemoryError 异常。EnsureLocalCapacity 这个函数是 1.2 以上版本才提供的
 *                  为了向下兼容，在编译的时候，如果申请创建的局部引用超过了本地引用的最大容量，在运行时 JVM 会调用 FatalError 函数使程序强制退出。在开发过程当中，可以为 JVM 添加-verbose:jni参数，在编译的时如果发现本地代码在试图申请过多的引用时，会打印警告信息提示我们要注意。
 *
 *              PushLocalFrame、PopLocalFrame
 *                  PushLocalFrame 为当前函数中需要用到的局部引用创建了一个引用堆栈，（如果之前调用 PushLocalFrame 已经创建了 Frame，在当前的本地引用栈中仍然是有效的)每遍历一次调用(*env)->GetObjectArrayElement(env, arr, i);返回一个局部引用时，JVM 会自动将该引用压入当前局部引用栈中。
 *                  PopLocalFrame 负责销毁栈中所有的引用。
 *                      在调用 PopLocalFrame 销毁当前 frame 中的所有引用前，如果第二个参数 result 不为空，会由 result 生成一个新的局部引用，再把这个新生成的局部引用存储在上一个 frame 中。
 *  -全局引用
 *      调用 NewGlobalRef 基于局部引用创建，会阻 GC 回收所引用的对象。
 *      可以跨方法、跨线程使用。
 *      JVM 不会自动释放，必须调用 DeleteGlobalRef 手动释放。
 *          env->NewGlobalRef(&obj);
 *          env->DeleteGlobalRef(obj);
 *  -弱全局引用
 *      调用 NewWeakGlobalRef 基于局部引用或全局引用创建，不会阻止 GC 回收所引用的对象，可以跨方法、跨线程使用。
 *      引用不会自动释放，在 JVM 认为应该回收它的时候（比如内存紧张的时候）进行回收而被释放。或调用DeleteWeakGlobalRef 手动释放。
 *      JVM 仍会回收弱引用所指向的对象，但弱引用本身在引用表中所占的内存永远也不会被回收
 *          env->NewWeakGlobalRef(&obj);
 *          env->DeleteWeakGlobalRef(obj);
 *
 *  总结：
 *      局部引用
 *          env->NewLocalRef(obj) 、 FindClass、NewObject、GetObjectClass和NewCharArray 等
 *          env->DeleteLocalRef(obj);
 *          局部引用管理
 *              EnsureLocalCapacity
 *              PushLocalFrame、PopLocalFrame
 *
 *      全局引用
 *          env->NewGlobalRef(&obj);
 *          env->DeleteGlobalRef(obj);
 *      弱全局引用
 *          env->NewWeakGlobalRef(&obj);
 *          env->DeleteWeakGlobalRef(obj);
 *          // 判断该弱全局引用是否还存在
 *          env->IsSameObject(obj, NULL)

 */

#endif //DEMO_JNI_REF_H

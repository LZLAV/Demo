//
// Created by liuzl15 on 2021/1/6.
// JNI 相关功能封装
//

#ifndef DEMO_JNI_WRAPPER_H
#define DEMO_JNI_WRAPPER_H

#include <jni.h>
#define JAVA_VERSION JNI_VERSION_1_6


/**
 * 类型签名
 */
/** @brief Java int primitive */
#define kTypeInt "I"
/** @brief Java short primitive */
#define kTypeShort "S"
/** @brief Java long primitive */
#define kTypeLong "J"
/** @brief Java float primitive */
#define kTypeFloat "F"
/** @brief Java double primitive */
#define kTypeDouble "D"
/** @brief Java bool primitive */
#define kTypeBool "Z"
/** @brief Java byte primitive */
#define kTypeByte "B"
/** @brief Java char primitive */
#define kTypeChar "C"
/** @brief Java void */
#define kTypeVoid "V"
/** @brief Java String class */
#define kTypeString "java/lang/String"
/** @brief Java base Object class */
#define kTypeObject "java/lang/Object"

/**
 * @brief Java class builder macro
 *
 * It is not necessary to call this macro with a quoted string. For example:
 *
 * kTypeJavaClass(Object)
 *
 * Will produce the string "java/lang/Object".
 */
#define kTypeJavaClass(x) "java/lang/" #x

#define kTypeArray(x) "[" x

#define kTypeObjectArray(x) "[L" x ";"

#endif //DEMO_JNI_WRAPPER_H

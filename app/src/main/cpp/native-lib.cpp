#include <jni.h>
#include <string>
#include <android/log.h>


// log标签

#define  TAG    "Lucky"

// 定义info信息

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)

// 定义debug信息

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

// 定义error信息

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

extern "C" JNIEXPORT jstring JNICALL
Java_com_lucky_jnidemo_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}


extern "C"
JNIEXPORT void JNICALL
Java_com_lucky_jnidemo_MainActivity_changeStringName(JNIEnv *env, jobject thiz) {

    //获取Class方式一
    jclass mainActivityCls = env->FindClass("com/lucky/jnidemo/MainActivity");

    //获取Class方式二
    //jclass mainActivityCls = env->GetObjectClass(thiz);

    // jfieldID GetFieldID(jclass clazz, const char* name, const char* sig)
    jfieldID nameFid = env->GetFieldID(mainActivityCls, "mName", "Ljava/lang/String;"); //签名

    // void SetObjectField(jobject obj, jfieldID fieldID, jobject value)
    jstring value = env->NewStringUTF("Update Lucky");

    //void SetObjectField(jobject obj, jfieldID fieldID, jobject value)
    env->SetObjectField(thiz, nameFid, value); //修改值
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lucky_jnidemo_MainActivity_00024Companion_changeIntAge(JNIEnv *env, jobject thiz) {

    // 这里要注意用FindClass方式获取,不然拿到的jcalss会是 "com/lucky/jnidemo/MainActivity$Companion"
    jclass mainActivityCls = env->FindClass("com/lucky/jnidemo/MainActivity");

    //jfieldID GetStaticFieldID(jclass clazz, const char* name, const char* sig)
    jfieldID ageFid = env->GetStaticFieldID(mainActivityCls, "mAge", "I");
    if (ageFid == NULL) {
        LOGI("%s", "No field  mAge");
        return;
    }
    // jint 背后就是int，所以可以直接用，  但是String 必须用 jstring
    int age = env->GetStaticIntField(mainActivityCls, ageFid); // 获取之前的age
    //  void SetStaticIntField(jclass clazz, jfieldID fieldID, jint value)
    env->SetStaticIntField(mainActivityCls, ageFid, age + 10); //修改值+10

}

extern "C"
JNIEXPORT void JNICALL
Java_com_lucky_jnidemo_MainActivity_changeDoubleHeight(JNIEnv *env, jobject thiz) {

    jclass mainActivityCls = env->GetObjectClass(thiz);

    // jfieldID GetFieldID(jclass clazz, const char* name, const char* sig)
    jfieldID numberFid = env->GetFieldID(mainActivityCls, "mHeight", "D");

    // void SetDoubleField(jobject obj, jfieldID fieldID, jdouble value)
    env->SetDoubleField(thiz, numberFid, 178.55);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_lucky_jnidemo_MainActivity_callKotlinMethod(JNIEnv *env, jobject thiz) {

    jclass mainActivitCls = env->GetObjectClass(thiz);

    // jmethodID   (*GetMethodID)(JNIEnv*, jclass, const char*, const char*);
    jmethodID addMid = env->GetMethodID(mainActivitCls, "add", "(II)I"); //kotlin层 fun add(number1: Int, number2: Int): Int

    // jint CallIntMethod(jobject obj, jmethodID methodID, ...)
    int result = env->CallIntMethod(thiz, addMid, 10, 10); //调用方法
    LOGD("result:%d\n", result);

    // ++++++++++++++++++++++ C调用 fun showString(str: String, num: Int): String 函数
    jmethodID showStringMid = env->GetMethodID(mainActivitCls, "showString", "(Ljava/lang/String;I)Ljava/lang/String;");

    // jobject     (*CallObjectMethod)(jobject, jmethodID, ...);
    jstring value = env->NewStringUTF("C语言调用Kotlin");
    jstring resultStrJ = (jstring) env->CallObjectMethod(thiz, showStringMid, value, 10086);
    const char *resultStr = env->GetStringUTFChars(resultStrJ, NULL);
    LOGD("result2:%s\n", resultStr);
}
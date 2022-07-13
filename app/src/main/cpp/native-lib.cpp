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

    //获取Class方式二
    jclass mainActivityCls = env->GetObjectClass(thiz);
    jclass companionClazz  = env->FindClass("com/lucky/jnidemo/MainActivity$Companion");
    if (companionClazz == NULL) {
        LOGI("%s", "method companionClazz not found");
        return;
    }

    //获取Companion成员ID（注意 参数1:OperationActivityClazz ）
    jfieldID OperationActivity$CompanionID = env->GetStaticFieldID(companionClazz, "Companion","c");

    //获取Companion成员（注意 参数1:OperationActivityClazz ）
    jobject Companion = env->GetStaticObjectField(companionClazz, OperationActivity$CompanionID);

    //  jfieldID GetStaticFieldID(jclass clazz, const char* name, const char* sig)
    jfieldID ageFid = env->GetStaticFieldID(mainActivityCls, "mAge", "I");

    // jint 背后就是int，所以可以直接用，  但是String 必须用 jstring
    int age = env->GetStaticIntField(mainActivityCls, ageFid); // 获取之前的age

    //  void SetStaticIntField(jclass clazz, jfieldID fieldID, jint value)
    env->SetStaticIntField(mainActivityCls, ageFid, age+10); //修改值

}
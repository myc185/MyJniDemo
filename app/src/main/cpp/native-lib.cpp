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

extern "C"
JNIEXPORT void JNICALL
Java_com_lucky_jnidemo_MainActivity_testArrayAction(JNIEnv *env, jobject thiz, jint count, jstring text_info, jintArray ints, jobjectArray strs) {

    //++++++++++++++++++++++++++ 操作Int数组
    LOGD("count:%d\n", count);
    const char *_textInfoStr = env->GetStringUTFChars(text_info, NULL);
    LOGD("text_info:%s\n", _textInfoStr);
    env->ReleaseStringUTFChars(text_info, _textInfoStr); //释放

    int intsLen = env->GetArrayLength(ints); //获取数组个数

    for (int i = 0; i < intsLen; ++i) {
        jint *_ints = env->GetIntArrayElements(ints, NULL);
        *(_ints + i) = (i + 1000001); //修改数组的值
        LOGD("C++ _ints item:%d\n", *(_ints + i));
        // JNI_OK 0 == 代表 先用操纵杆刷新到JVM，JVM会更新上层代码。再释放C++层数组
        // JNI_COMMIT 1 == 代表 用操纵杆刷新到JVM，JVM会更新上层代码
        // JNI_ABORT 2 == 代表 释放C++层数组
        env->ReleaseIntArrayElements(ints, _ints, JNI_OK); //在数组循环中，一定要记得释放
    }

    //++++++++++++++++++++++++++ 操作String数组
    int strsLen = env->GetArrayLength(strs);
    for (int i = 0; i < strsLen; ++i) {
        jobject item = env->GetObjectArrayElement(strs, i); //获取每一个值
        jstring itemStr = (jstring) item;
        const char *itemStr_1 = env->GetStringUTFChars(itemStr, NULL);
        LOGI("C++ 修改前itemStr_:%s\n", itemStr_1);
        env->ReleaseStringUTFChars(itemStr, itemStr_1);

        jstring value = env->NewStringUTF("AAAAAAA");
        env->SetObjectArrayElement(strs, i, value); //修改数组值
        jobject item2 = env->GetObjectArrayElement(strs, i);
        jstring itemStr2 = (jstring) item2;
        const char *itemStr_2 = env->GetStringUTFChars(itemStr2, NULL);
        LOGI("C++ 修改后itemStr_2:%s\n", itemStr_2);
        env->ReleaseStringUTFChars(itemStr2, itemStr_2);

    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_lucky_jnidemo_MainActivity_putObject(JNIEnv *env, jobject thiz, jobject person, jstring str) {

    const char *_str = env->GetStringUTFChars(str, NULL);
    LOGD("_str:%s\n", _str);
    env->ReleaseStringUTFChars(str, _str);
    jclass mStudentClass = env->GetObjectClass(person);
    // toString
    jmethodID toStringMethod = env->GetMethodID(mStudentClass, "toString", "()Ljava/lang/String;");
    jstring results = (jstring) env->CallObjectMethod(person, toStringMethod);
    const char *result = env->GetStringUTFChars(results, NULL);
    LOGD("C++ toString:%s\n", result);
    env->ReleaseStringUTFChars(results, result);

    // setName
    jmethodID setNameMethod = env->GetMethodID(mStudentClass, "setName", "(Ljava/lang/String;)V");
    jstring nameS = env->NewStringUTF("李四");
    env->CallVoidMethod(person, setNameMethod, nameS);

    // getName
    jmethodID getNameMethod = env->GetMethodID(mStudentClass, "getName", "()Ljava/lang/String;");
    jstring nameS2 = (jstring) env->CallObjectMethod(person, getNameMethod);
    const char *nameS3 = env->GetStringUTFChars(nameS2, NULL);
    LOGD("C++ getName:%s\n", nameS3);
    env->ReleaseStringUTFChars(nameS2, nameS3);

    // setAge
    jmethodID setAgeMethod = env->GetMethodID(mStudentClass, "setAge", "(I)V");
    env->CallVoidMethod(person, setAgeMethod, 99);

    // getAge
    jmethodID getAgeMethod = env->GetMethodID(mStudentClass, "getAge", "()I");
    int age = env->CallIntMethod(person, getAgeMethod);
    LOGD("C++ getAge:%d\n", age);

    env->DeleteLocalRef(mStudentClass);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_lucky_jnidemo_MainActivity_insertObject(JNIEnv *env, jobject thiz) {

    // @Student对象
    jclass studentClass = env->FindClass("com/lucky/jnidemo/Student");
    jobject student = env->AllocObject(studentClass);

    // setName
    jmethodID setNameMethod = env->GetMethodID(studentClass, "setName", "(Ljava/lang/String;)V");
    jstring value1 = env->NewStringUTF("张三");
    env->CallVoidMethod(student, setNameMethod, value1);

    // setAge
    jmethodID setAgeMethod = env->GetMethodID(studentClass, "setAge", "(I)V");
    env->CallVoidMethod(student, setAgeMethod, 99);

    // @Person对象
    jclass personClass = env->FindClass("com/lucky/jnidemo/Person");
    jobject person = env->AllocObject(personClass); // C++ 分配一个对象出来，不会调用此对象的构造函数
    // env->NewObject();  // C++ 实例化一个对象出来，会调用此对象的构造函数，相当于： new XXX();

    // void CallVoidMethod(person obj, jmethodID methodID, ...)
    jmethodID setStudent = env->GetMethodID(personClass, "setStudent", "(Lcom/lucky/jnidemo/Student;)V");
    env->CallVoidMethod(person, setStudent, student);

    // static void putStudent
    jmethodID putStudent = env->GetStaticMethodID(personClass, "putStudent", "(Lcom/lucky/jnidemo/Student;)V");
    env->CallStaticVoidMethod(personClass, putStudent, student);

    // TODO 释放工作
    // 释放方式一
    env->DeleteLocalRef(personClass);
    env->DeleteLocalRef(student);
    env->DeleteLocalRef(value1);
    env->DeleteLocalRef(personClass);
    env->DeleteLocalRef(person);
    // 释放方式二
    /*env->GetStringUTFChars()
    env->ReleaseStringUTFChars()*/

    // 释放方式三
    /*new StudentCPP对象
    delete StudentCPP对象*/
}
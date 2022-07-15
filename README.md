# Android NDK 开发之JNI常规操作



## 1. JNI函数详解



java中native方法在C++代码中一般如下：

```c++
extern "C" JNIEXPORT jstring JNICALL
Java_com_lucky_jnidemo_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
```



#### 1. `extern "C"` 说明

表示下面的代码，都采用C的编译方式。

之所以用C的编译方式，是因为 JNIEnv 是C语言代码写的，避免一些函数重载等C语言中不支持的



查看`JNIEnv`源码，进入到`jni.h`，可以看到：

```c++
#if defined(__cplusplus)
typedef _JNIEnv JNIEnv; // 如果是C++代码，用这个宏定义
typedef _JavaVM JavaVM;
#else
typedef const struct JNINativeInterface* JNIEnv;  //如果是C代码，用这个宏定义
typedef const struct JNIInvokeInterface* JavaVM;
#endif
```

继续跟踪 `_JNIEnv`，可以看到：

```c++
struct _JNIEnv {
    /* do not rename this; it does not seem to be entirely opaque */
    const struct JNINativeInterface* functions;
```

因此，env的调用方式如下：

```c++
// C++的情况如下： 
Java_com_lucky_jnidemo_MainActivity_stringFromJNI(JNIEnv * env, ...) 
{
   env->NewStringUTF();  //详细看源码中的结构体定义
}

C的情况如下： 
Java_com_lucky_jnidemo_MainActivity_stringFromJNI(JNIEnv * env, ...) 
{ 
  (*env)->NewStringUTF(); //二级指针
}
```



#### 2. `JNIEXPORT` 宏定义说明

针对Linux平台：该声明的作用是保证在本动态库中声明的方法 , 能够在其他项目中可以被调用 ;



#### 3. `JNICALL` 宏定义说明

此宏定义为空，用来表示函数的调用规范。

#### 4. `jobject`  与 `jclass`

```tex
jobject // java传递下来的对象，就是本项目中 MainActivity 对象 
jclass // java传递下来的 class 对象，就是本项目中的 MainActivity class
```



## 2. JNI中函数操作



### 1. 在C层中修改Java/Kotlin层String变量（引用类型遍历） 

Kotlin代码：

```kotlin
//即将要修改的代码熟悉
private var mName: String = "Lucky" 

//native 修改String name
external fun changeStringName()

//点击按钮进行修改
binding.btn1.setOnClickListener {
    Log.i("MainActivity", "修改前：$mName")
    changeStringName()
    Log.i("MainActivity", "修改后：$mName")
}
```

C层中的代码

```c++
extern "C"
JNIEXPORT void JNICALL
Java_com_lucky_jnidemo_MainActivity_changeStringName(JNIEnv *env, jobject thiz) {

    //获取Class方式一
    jclass mainActivityCls = env->FindClass("com/lucky/jnidemo/MainActivity");

    //获取Class方式二
    //jclass mainActivityCls = env->GetObjectClass(thiz);

    // jfieldID GetFieldID(jclass clazz, const char* name, const char* sig)
    jfieldID nameFid = env->GetFieldID(mainActivityCls, "mName", "Ljava/lang/String;");

    // void SetObjectField(jobject obj, jfieldID fieldID, jobject value)
    jstring value = env->NewStringUTF("Update Lucky");

    //void SetObjectField(jobject obj, jfieldID fieldID, jobject value)
    env->SetObjectField(thiz, nameFid, value); //修改值
}
```



打印结果：

```tex
com.lucky.jnidemo I/MainActivity: 修改前：Lucky
com.lucky.jnidemo I/MainActivity: 修改后：Update Lucky
```



### 2. 在C层中修改Kotlin层静态Int变量

通过apk查看编译后的MainActivity 类，可以看到静态mAege是在MainActivity 中

![image-20220714112416060](http://qnpic.luckymo.cn/image-20220714112416060.png)

因此，这里跟修改MainActivity中的变量一样的操作

Kotlin代码：

```kotlin
companion object {
    private var mAge: Int = 666
    init {
        System.loadLibrary("jnidemo")
    }
    external fun changeIntAge() //修改Int age
}


binding.btn2.setOnClickListener {
    Log.i("MainActivity", "修改前：${mAge}")
    changeIntAge()
    Log.i("MainActivity", "修改后：${mAge}")
}
```

C层中的代码

```c++
extern "C"
JNIEXPORT void JNICALL
Java_com_lucky_jnidemo_MainActivity_00024Companion_changeIntAge(JNIEnv *env, jobject thiz) {

    // 这里要注意用FindClass方式获取,不然拿到的jcalss会是 "com/lucky/jnidemo/MainActivity$Companion"
    jclass mainActivityCls = env->FindClass("com/lucky/jnidemo/MainActivity");

    //jfieldID GetStaticFieldID(jclass clazz, const char* name, const char* sig)
    jfieldID ageFid = env->GetStaticFieldID(mainActivityCls, "mAge", "I");
    if (ageFid == NULL) {
        LOGI("%s", "no field  mAge ");
        return;
    }
    // jint 背后就是int，所以可以直接用，  但是String 必须用 jstring
    int age = env->GetStaticIntField(mainActivityCls, ageFid); // 获取之前的age
    //  void SetStaticIntField(jclass clazz, jfieldID fieldID, jint value)
    env->SetStaticIntField(mainActivityCls, ageFid, age + 10); //修改值+10

}
```

打印结果：

```tex
com.lucky.jnidemo I/MainActivity: 修改前：666
com.lucky.jnidemo I/MainActivity: 修改后：676
```



### 3. 在C层中修改Kotlin层double变量

kotlin层代码：

```kotlin
private var mHeight: Double = 175.0

binding.btn3.setOnClickListener {
    Log.i("MainActivity", "修改前：${mHeight}")
    changeDoubleHeight()
    Log.i("MainActivity", "修改后：${mHeight}")
}

external fun changeDoubleHeight() //修改Double height
```

C层中的代码

```c++
extern "C"
JNIEXPORT void JNICALL
Java_com_lucky_jnidemo_MainActivity_changeDoubleHeight(JNIEnv *env, jobject thiz) {

    jclass mainActivityCls = env->GetObjectClass(thiz);

    // jfieldID GetFieldID(jclass clazz, const char* name, const char* sig)
    jfieldID numberFid  = env->GetFieldID(mainActivityCls, "mHeight", "D");

    // void SetDoubleField(jobject obj, jfieldID fieldID, jdouble value)
    env->SetDoubleField(thiz, numberFid, 178.55);

}
```

打印结果：

```tex
com.lucky.jnidemo I/MainActivity: 修改前：175.0
com.lucky.jnidemo I/MainActivity: 修改后：178.55
```



### 4. 在C层中调用Kotlin层方法

kotlin层代码：

```kotlin
/***
 * "(II)I"
 * 被C层调用的方法
 */
fun add(number1: Int, number2: Int): Int {
    return number1 + number2
}

/***
 * 被C调用的方法 (Ljava/lang/String;I) Ljava/lang/String;
 */
fun showString(str: String, num: Int): String {
    Log.i("MainActivity", "Value from C++ str: $str  num: $num")
    return "【$str】"
}

external fun callKotlinMethod() //在C层中调用Kotlin代码

binding.btn4.setOnClickListener {
    callKotlinMethod() //点击按钮
}
```

C层中的代码

```c++
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
```

打印结果：

```tex
//C中的打印
com.lucky.jnidemo D/Lucky: result:20
com.lucky.jnidemo D/Lucky: result2:【C语言调用Kotlin】

//Kotlin中的打印
com.lucky.jnidemo I/MainActivity: Value from C++ str: C语言调用Kotlin  num: 10086
```



### 5. 在C层中修改Kotlin层数组

kotlin层代码：

```kotlin
binding.btn5.setOnClickListener {
    val ints = intArrayOf(1, 2, 3, 4, 5, 6)
    val strs = arrayOf("李小龙", "李连杰", "李元霸")
    testArrayAction(99, "你好", ints, strs)

    for (anInt in ints) {
        Log.d("MainActivity", "(Kotlin)修改后: ints:$anInt")
    }

    for (str in strs) {
        Log.e("MainActivity", "(Kotlin)修改后: strs:$str")
    }
}

//在C层中修改数组
external fun testArrayAction(count: Int, textInfo: String, ints: IntArray, strs: Array<String>) 
```

C层中的代码

```c++
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
```



打印日志：

```tex
com.lucky.jnidemo D/Lucky: count:99
com.lucky.jnidemo D/Lucky: text_info:你好
com.lucky.jnidemo D/Lucky: C++ _ints item:1000001
com.lucky.jnidemo D/Lucky: C++ _ints item:1000002
com.lucky.jnidemo D/Lucky: C++ _ints item:1000003
com.lucky.jnidemo D/Lucky: C++ _ints item:1000004
com.lucky.jnidemo D/Lucky: C++ _ints item:1000005
com.lucky.jnidemo D/Lucky: C++ _ints item:1000006
com.lucky.jnidemo I/Lucky: C++ 修改前itemStr_:李小龙
com.lucky.jnidemo I/Lucky: C++ 修改后itemStr_2:AAAAAAA
com.lucky.jnidemo I/Lucky: C++ 修改前itemStr_:李连杰
com.lucky.jnidemo I/Lucky: C++ 修改后itemStr_2:AAAAAAA
com.lucky.jnidemo I/Lucky: C++ 修改前itemStr_:李元霸
com.lucky.jnidemo I/Lucky: C++ 修改后itemStr_2:AAAAAAA
com.lucky.jnidemo D/MainActivity: (Kotlin)修改后: ints:1000001
com.lucky.jnidemo D/MainActivity: (Kotlin)修改后: ints:1000002
com.lucky.jnidemo D/MainActivity: (Kotlin)修改后: ints:1000003
com.lucky.jnidemo D/MainActivity: (Kotlin)修改后: ints:1000004
com.lucky.jnidemo D/MainActivity: (Kotlin)修改后: ints:1000005
com.lucky.jnidemo D/MainActivity: (Kotlin)修改后: ints:1000006
com.lucky.jnidemo E/MainActivity: (Kotlin)修改后: strs:AAAAAAA
com.lucky.jnidemo E/MainActivity: (Kotlin)修改后: strs:AAAAAAA
```



### 6. 在C层中修改Kotlin层类

修改实体类，我们这里用Java类，这样修改int 变量不需要kotlin中这么麻烦，需要转换Integer

kotlin层代码：

Person类

```java
public class Person {

    private String name;
    private int age;

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public int getAge() {
        return age;
    }

    public void setAge(int age) {
        this.age = age;
    }

    @Override
    public String toString() {
        return "Person{" +
                "name='" + name + '\'' +
                ", age=" + age +
                '}';
    }
}
```

```kotlin
binding.btn6.setOnClickListener {
    val person = Person()
    person.name = "张三"
    person.age = 18
    putObject(person, "Kotlin文本")
    Log.i("MainActivity", "(Kotlin)修改后, person:$person")
}

// 只玩Student对象里面的成员
external fun putObject(person: Person, str: String) // 传递引用类型，传递对象
```

C层中的代码

```c++
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
```



### 7.  在C层中修改Kotlin层类



Student类

```java
public class Student {
    private String name;
    private int age;

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public int getAge() {
        return age;
    }

    public void setAge(int age) {
        this.age = age;
    }

    @Override
    public String toString() {
        return "Student{" +
                "name='" + name + '\'' +
                ", age=" + age +
                '}';
    }
}
```

Person 类

```java
public class Person {

    private String name;
    private int age;
    private static Student student;

    public static void putStudent(Student student) {
        Person.student = student;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public int getAge() {
        return age;
    }

    public void setAge(int age) {
        this.age = age;
    }

    public Student getStudent() {
        return student;
    }

    public void setStudent(Student student) {
        Person.student = student;
    }

    @Override
    public String toString() {
        return "Person{" +
                "name='" + name + '\'' +
                ", age=" + age +
                ", student=" + student +
                '}';
    }
}
```



kotlin层中的代码

```kotlin
binding.btn7.setOnClickListener {
    insertObject()
}
external fun insertObject()
```



C层中的代码

```c++
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
```



[GitHub源码:MyJniDemo](https://github.com/myc185/MyJniDemo)

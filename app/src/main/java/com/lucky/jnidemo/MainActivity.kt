package com.lucky.jnidemo

import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.lucky.jnidemo.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    private var mName: String = "Lucky"

    private var mHeight: Double = 175.0


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        binding.sampleText.text = stringFromJNI()
        handleClick()
    }

    private fun handleClick() {
        binding.btn1.setOnClickListener {
            Log.i("MainActivity", "修改前：$mName")
            changeStringName()
            Log.i("MainActivity", "修改后：$mName")
        }

        binding.btn2.setOnClickListener {
            Log.i("MainActivity", "修改前：${mAge}")
            changeIntAge()
            Log.i("MainActivity", "修改后：${mAge}")
        }

        binding.btn3.setOnClickListener {
            Log.i("MainActivity", "修改前：${mHeight}")
            changeDoubleHeight()
            Log.i("MainActivity", "修改后：${mHeight}")
        }

        binding.btn4.setOnClickListener {
            callKotlinMethod()
        }

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

        binding.btn6.setOnClickListener {
            val person = Person()
            person.name = "张三"
            person.age = 18
            putObject(person, "Kotlin文本")
            Log.i("MainActivity", "(Kotlin)修改后, person:$person")

        }
    }

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


    external fun stringFromJNI(): String

    external fun changeStringName() //修改String name

    external fun changeDoubleHeight() //修改Double height

    external fun callKotlinMethod() //在C层中调用Kotlin代码

    external fun testArrayAction(count: Int, textInfo: String, ints: IntArray, strs: Array<String>) //在C层中修改数组

    // 只玩Student对象里面的成员
    external fun putObject(person: Person, str: String) // 传递引用类型，传递对象


    companion object {

        private var mAge: Int = 666


        init {
            System.loadLibrary("jnidemo")
        }

        external fun changeIntAge() //修改Int age
    }
}
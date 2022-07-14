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


    companion object {

        private var mAge: Int = 666


        init {
            System.loadLibrary("jnidemo")
        }

        external fun changeIntAge() //修改Int age
    }
}
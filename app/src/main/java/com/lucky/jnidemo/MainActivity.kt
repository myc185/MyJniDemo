package com.lucky.jnidemo

import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.lucky.jnidemo.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    private var mName: String = "Lucky"


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
    }


    external fun stringFromJNI(): String

    external fun changeStringName() //修改String name




    companion object {

        private var mAge: Int = 666


        init {
            System.loadLibrary("jnidemo")
        }

        external fun changeIntAge() //修改Int age
    }
}
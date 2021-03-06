package com.lucky.jnidemo;

/**
 * @Author: Lucky Mo
 * @CreateDate: 2022/7/14 15:59
 * @Description:
 */
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

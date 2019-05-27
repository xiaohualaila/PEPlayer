package panoeye.pelibrary;

import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

/**
 * Created by tir on 2016/12/13.
 */
//锁封装类
 class LockWrap<T>{
    Lock _lock = new ReentrantLock();//ReentrantLock 重入锁
    private T _obj;//模板类对象
     //获取T类对象
     T get(){
        return _obj;
    }
    //设置T类对象
     void set(T obj){
        _obj = obj;
    }
    //T类对象上锁
     void lock(){
        _lock.lock();
    }
    //T类对象上锁：只有当它在调用时是空闲的，才获得锁
     boolean tryLock(){
        return _lock.tryLock();
    }
    //T类对象解锁
     void unlock(){
        _lock.unlock();
    }
    //构造函数
     LockWrap(T obj){
        _obj = obj;
    }
}

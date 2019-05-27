package panoeye.pelibrary;


import java.util.Map;

/**
 * Created by tir on 2016/12/14.
 */
//锁+Map封装类
 public class MapThrowWrap<K, V> {

    private static final String TAG = "MapThrowWrap";
    LockWrap<Map<K, V>> _obj;//锁封装类对象

     public V get(K key) throws PEError {
        V value = _obj.get().get(key);
        if (value == null) {
            throw PEError.gen(PEErrorType.notFound, "textureDict中还未存在对应key:"+key+"的PEVideoTexture类对象");
        }
        return value;
    }

     void put(K key, V value) {
        _obj.get().put(key, value);
    }
    //构造函数
    MapThrowWrap(LockWrap<Map<K, V>> obj) {
        _obj = obj;
    }

}

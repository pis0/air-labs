package com.assukar.windows.store.utils {
public class StaticUtils {
    public static function getProp(obj:Object, key:String):* {
        var prop:* = obj[key];
        if (prop != null) return prop;
        throw new Error("unexpected key: " + key);
    }
}
}

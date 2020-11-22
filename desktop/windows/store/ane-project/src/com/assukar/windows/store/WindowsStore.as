package com.assukar.windows.store {
import com.tuarua.fre.ANEError;

import flash.events.EventDispatcher;

public class WindowsStore extends EventDispatcher {
    private static var windowsStore:WindowsStore;

    public function WindowsStore() {
        if (WindowsStoreContext.instance) {
            var theRet:* = WindowsStoreContext.instance.call("init");
            if (theRet is ANEError) throw theRet as ANEError;
        }
        windowsStore = this;
    }

    public static function get storeInstance():WindowsStore {
        if (!windowsStore) new WindowsStore();
        return windowsStore;
    }

    public function getProducts(jsonRequestBody:String):void {
        WindowsStoreContext.validate();
        var theRet:* = WindowsStoreContext.instance.call("getProducts", jsonRequestBody);
        if (theRet is ANEError) throw theRet as ANEError;
    }

    public static function dispose():void {
        if (WindowsStoreContext.instance) {
            WindowsStoreContext.dispose();
        }
    }


}
}
package com.assukar.windows.store.product {
import com.assukar.windows.store.utils.StaticUtils;

public class WindowsProduct implements IWindowsProduct {
    private var dataObj:Object;

    function WindowsProduct(rawData:String) {
        dataObj = JSON.parse(rawData);
    }

    public function get title():String {
        return StaticUtils.getProp(dataObj, "Title") as String;
    }

    public function get price():String {
        return StaticUtils.getProp(dataObj, "Price") as String;
    }

    public function get productKind():String {
        return StaticUtils.getProp(dataObj, "ProductKind") as String;
    }

    public function get storeId():String {
        return StaticUtils.getProp(dataObj, "StoreId") as String;
    }

    public function get inCollection():Boolean {
        return StaticUtils.getProp(dataObj, "InCollection") as Boolean;
    }

    public function toString():String {
        return "[WindowsProduct(title:" + title
                + ", productKind:" + productKind
                + ", price" + price
                + ", inCollection:" + inCollection
                + ", storeId:" + storeId + ")]";
    }
}
}

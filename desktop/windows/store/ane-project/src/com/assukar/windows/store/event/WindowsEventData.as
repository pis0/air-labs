package com.assukar.windows.store.event {
import com.assukar.windows.store.utils.StaticUtils;

public class WindowsEventData implements IWindowsEventData {
    private var dataObj:Object;

    function WindowsEventData(rawData:String) {
        dataObj = JSON.parse(rawData);
    }

    public function get action():String {
        return StaticUtils.getProp(dataObj, "Action") as String;
    }

    public function get success():Boolean {
        return StaticUtils.getProp(dataObj, "Success") as Boolean;
    }

    public function get data():String {
        return StaticUtils.getProp(dataObj, "Data") as String;
    }
}
}

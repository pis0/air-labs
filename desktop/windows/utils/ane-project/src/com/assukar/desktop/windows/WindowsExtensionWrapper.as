package com.assukar.desktop.windows {

import com.assukar.desktop.windows.hardwareinfo.HardwareInfo;
import com.assukar.desktop.windows.hardwareinfo.IHardwareInfo;

import flash.events.EventDispatcher;
import flash.events.IEventDispatcher;
import flash.events.StatusEvent;
import flash.external.ExtensionContext;

public class WindowsExtensionWrapper extends EventDispatcher {
    public function WindowsExtensionWrapper(_target:IEventDispatcher = null) {
        super(_target);

        // Create a new extension context and listen for status events from it:
        m_extContext = ExtensionContext.createExtensionContext(EXTENSION_ID, null);
        m_extContext.addEventListener(StatusEvent.STATUS, onStatusEvent);
    }

    private function onStatusEvent(_event:StatusEvent):void {
        // Here you can add functionality to deal with the various events
        // that you may need to dispatch from native code.
        // For the moment let's just log what comes in for now:
        trace("Event code: " + _event.code + ", event level: " + _event.level);
    }

    // For this tutorial we'll need one ExtensionContext instance
    // and we'll need to keep it while our ANE is in use:
    private var m_extContext:ExtensionContext = null;

    // The extension ID needs to be a unique string:
    private static const EXTENSION_ID:String = "com.assukar.desktop.windows.WindowsExtension";

    public function getHardwareInfo():IHardwareInfo {
        var rawData:String = m_extContext.call("as_getHardwareInfo") as String;
        return new HardwareInfo(rawData);
    }

    public function listProducts():String {
        return m_extContext.call("as_listProducts") as String;
    }


//    public function passString(_message:String):String {
//        // Calls the ASPassAString function (exposed as "as_passAString" to ActionScript)
//        // that we defined in dllMain.cpp:
//        // passes a string to it and expects a strnig back.
//        return m_extContext.call("as_passAString", _message) as String;
//    }
}
}
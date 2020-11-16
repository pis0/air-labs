package com.assukar.desktop.windows {

import flash.events.EventDispatcher;
import flash.events.IEventDispatcher;
import flash.events.StatusEvent;
import flash.external.ExtensionContext;

public class Store extends EventDispatcher {

    private static const EXTENSION_ID:String = "com.assukar.desktop.windows.Store";

    private var m_extContext:ExtensionContext = null;

    public function Store(target:IEventDispatcher = null) {
        super(target);
        m_extContext = ExtensionContext.createExtensionContext(EXTENSION_ID, null);
        m_extContext.addEventListener(StatusEvent.STATUS, onStatusEvent);
    }

    private function onStatusEvent(e:StatusEvent):void {
        trace("Event code: " + e.code + ", event level: " + e.level);
    }

    public function createStoreContext():void {
        m_extContext.call("as_createStoreContext");
    }

    public function listProducts():void {
        m_extContext.call("as_listProducts");
    }
}
}
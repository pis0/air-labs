package com.assukar.windows.store {
import flash.events.Event;

public class WindowsStoreEvent extends Event {
    public var data:*;
    public function WindowsStoreEvent(type:String, data:* = null, bubbles:Boolean = false, cancelable:Boolean = false) {
        super(type, bubbles, cancelable);
        this.data = data;
    }
    public override function clone():Event {
        return new WindowsStoreEvent(type, this.data, bubbles, cancelable);
    }

    public override function toString():String {
        return formatToString("WindowsStoreEvent", "data", "type", "bubbles", "cancelable");
    }
}
}

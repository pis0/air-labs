package com.assukar.desktop.windows {
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
}
}
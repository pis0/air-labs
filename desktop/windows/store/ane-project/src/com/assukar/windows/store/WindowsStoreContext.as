package com.assukar.windows.store {
import flash.events.StatusEvent;
import flash.external.ExtensionContext;

public class WindowsStoreContext {
    private static var initiated:Boolean = false;
    private static var context:ExtensionContext;
    internal static const NAME:String = "WindowsStore";
    internal static const TRACE:String = "LOG";
    internal static const INIT_ERROR_MESSAGE:String = NAME + " not initialised... use .instance";

    public function WindowsStoreContext() {
    }

    public static function get instance():ExtensionContext {
        if (context == null) {
            try {
                context = ExtensionContext.createExtensionContext("com.assukar.windows.store." + NAME, null);
                context.addEventListener(StatusEvent.STATUS, onStatus);
                initiated = true;
            } catch (e:Error) {
                trace("[" + NAME + "] ANE Not loaded properly. Future calls will fail.");
            }
        }
        return context;
    }

    private static function onStatus(e:StatusEvent):void {
        switch (e.level) {
            case "TRACE":
                trace("[" + NAME + "]", e.code);
                break;
            case "WINDOWS_STORE_EVENT":
                WindowsStore.storeInstance.dispatchEvent(new WindowsStoreEvent("WINDOWS_STORE_EVENT", e.code));
                break;
        }
    }

    public static function validate():void {
        if (!initiated) throw new Error(INIT_ERROR_MESSAGE);
    }

    public static function dispose():void {
        if (!context) {
            return;
        }
        trace("[" + NAME + "] Unloading ANE...");
        context.removeEventListener(StatusEvent.STATUS, onStatus);
        context.dispose();
        context = null;
        initiated = false;
    }

}
}

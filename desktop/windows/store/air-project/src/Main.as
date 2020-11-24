package {

import com.assukar.windows.store.Actions;
import com.assukar.windows.store.WindowsStore;
import com.assukar.windows.store.WindowsStoreEvent;
import com.assukar.windows.store.event.WindowsEventData;
import com.assukar.windows.store.product.WindowsProduct;
import com.tuarua.FreSharp;

import flash.desktop.NativeApplication;
import flash.display.Sprite;
import flash.events.Event;

public class Main extends Sprite {
    private var freSharpANE:FreSharp = new FreSharp();
    private static var windowsStore:WindowsStore;

    private function onExiting(event:Event):void {
        WindowsStore.dispose();
    }

    public function Main() {
        NativeApplication.nativeApplication.addEventListener(Event.EXITING, onExiting);
        windowsStore = WindowsStore.storeInstance;
        windowsStore.addEventListener("WINDOWS_STORE_EVENT", onWindowsStoreEvent);

        // get productss
        windowsStore.getProducts(JSON.stringify({
            productKinds: [
                "UnmanagedConsumable"
            ],
            storeIds: [
                "9NRKGGCVC12Q",
                "9MT44Q75C27T"
            ]
        }));

        // purchases
//        windowsStore.purchase(
////    "9NRKGGCVC12Q"
//    "9MT44Q75C27T"
//        );

//        // consume
//        windowsStore.consume(
////                "9NRKGGCVC12Q"
//                "9MT44Q75C27T"
//        );

    }

    private static function onWindowsStoreEvent(e:WindowsStoreEvent):void {
        trace("onWindowsStoreEvent - raw data: " + e.data);

        var windowsEventData:WindowsEventData = new WindowsEventData(e.data);
        if (!windowsEventData.success) {
            trace("Error: " + windowsEventData.data);
            return;
        }

        switch (windowsEventData.action) {
            case Actions.GET_PRODUCTS:
                var getProductsRawData:Array = JSON.parse(windowsEventData.data) as Array;
                var getProductsData:Array = [];
                for each(var obj:Object in getProductsRawData) {
                    getProductsData.push(new WindowsProduct(JSON.stringify(obj)));
                }
                trace("getProductsData:", getProductsData);
                break;

            case Actions.PURCHASE:
                trace("purchaseRawData:", windowsEventData.data);
                break;

            case Actions.CONSUME:
                trace("consumeRawData:", windowsEventData.data);
                break;
        }

    }


}
}

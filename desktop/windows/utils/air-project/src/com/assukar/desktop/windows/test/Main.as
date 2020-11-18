package com.assukar.desktop.windows.test {

import com.assukar.desktop.windows.WindowsExtensionWrapper;
import com.assukar.desktop.windows.hardwareinfo.IHardwareInfo;

import flash.display.Sprite;
import flash.text.TextField;

public class Main extends Sprite {

    private var m_ext:WindowsExtensionWrapper = null;

    public function Main() {

        var message:String = "Hello, World";

        var textField:TextField = new TextField();
        textField.appendText(message);
        textField.appendText('\n');
        m_ext = new WindowsExtensionWrapper();

        var hInfo:IHardwareInfo = m_ext.getHardwareInfo();
        trace(hInfo.uuid );
        trace(hInfo.machineGuid );
        trace(hInfo.diskDrivePNPDeviceId );
        trace(hInfo.hardwareProfileGuid );
        trace(hInfo.volumeSerialNumber );
        trace(hInfo.processorId );

        addChild(textField)


        try {
            m_ext.listProducts();
        }
        catch (err:Error) {
            trace(err.message);
        }


    }
}
}

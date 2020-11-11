package com.assukar.desktop.windows.test {

import com.assukar.desktop.windows.WindowsExtensionWrapper;

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

        var rawHInfo:String = m_ext.getHardwareInfo();
        trace('HERE 0', rawHInfo);

        var temp:String = "{\"uuid\":\"0ECE59BA-85A6-5C18-A960-2CF05D39F4E8\",\"hardwareProfileGuid\":\"0c449f40-00b0-11eb-87fe-806e6f6e6963\",\"diskDrivePNPDeviceId\":\"SCSI\DISK&VEN_NVME&PROD_FORCE_MP510\5&2F840D64&0&000000\",\"machineGuid\":\"4e10e4de-10b7-4a37-8567-ddec2977396d\",\"processorId\":\"BFEBFBFF000A0655\",\"volumeSerialNumber\":\"3085F18A\"}";

        var hInfo:Object = JSON.parse(
                // temp
                rawHInfo
        );
        trace('HERE 1', hInfo);

        //textField.appendText(m_ext.passString(message));

        addChild(textField)
    }
}
}

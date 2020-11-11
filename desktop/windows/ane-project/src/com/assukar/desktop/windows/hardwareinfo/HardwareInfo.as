package com.assukar.desktop.windows.hardwareinfo {

public class HardwareInfo extends Object implements IHardwareInfo {

    private var rawData:String;
    private var data:Object;

    function HardwareInfo(rawData:String) {
        this.rawData = rawData;
        this.data = JSON.parse(rawData);
    }

    public function toString():String {
        return "[HardwareInfo(" +
                rawData.replace(/{/g, "")
                        .replace(/}/g, "")
                + ")]";
    }

    public function get uuid():String {
        return this.data["uuid"];
    }

    public function get hardwareProfileGuid():String {
        return this.data["hardwareProfileGuid"];
    }

    public function get diskDrivePNPDeviceId():String {
        return this.data["diskDrivePNPDeviceId"];
    }

    public function get machineGuid():String {
        return this.data["machineGuid"];
    }

    public function get processorId():String {
        return this.data["processorId"];
    }

    public function get volumeSerialNumber():String {
        return this.data["volumeSerialNumber"];
    }


}
}


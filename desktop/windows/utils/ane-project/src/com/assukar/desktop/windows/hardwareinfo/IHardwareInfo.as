package com.assukar.desktop.windows.hardwareinfo{
public interface IHardwareInfo {
    function get uuid():String;

    function get hardwareProfileGuid():String;

    function get diskDrivePNPDeviceId():String;

    function get machineGuid():String;

    function get processorId():String;

    function get volumeSerialNumber():String;

    function toString():String;
}
}


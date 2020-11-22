package com.assukar.windows.store.event {
public interface IWindowsEventData {
    function get action():String;

    function get success():Boolean;

    function get data():String;
}
}

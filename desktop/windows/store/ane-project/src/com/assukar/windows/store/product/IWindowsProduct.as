package com.assukar.windows.store.product {
public interface IWindowsProduct {
    function get title():String;

    function get price():String;

    function get productKind():String;

    function get storeId():String;

    function get inCollection():String;
}
}

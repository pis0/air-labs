package com.assukar.desktop.windows.test {

import com.assukar.desktop.windows.Store;
import com.assukar.desktop.windows.Utils;

import flash.display.Sprite;
import flash.text.TextField;

public class Main extends Sprite {

    public function Main() {

        var message:String = "Hello, World";

        var textField:TextField = new TextField();
        textField.appendText(message);
        textField.appendText('\n');

        addChild(textField)


        //var m_ext:Store = new Store();
        //m_ext.createStoreContext();

        var m_ext:Store = new Store();
        m_ext.createStoreContext();

    }
}
}

package com.assukar.desktop.windows.test {

import flash.display.Sprite;
import flash.text.TextField;

import com.assukar.desktop.windows.WindowsExtensionWrapper;

public class Main extends Sprite {

    private var m_ext : WindowsExtensionWrapper = null;

    public function Main() {

        var message:String = "Hello, World";

        var textField:TextField = new TextField();
        textField.appendText(message);
        textField.appendText('\n');
        m_ext = new WindowsExtensionWrapper();

        trace('HERE', m_ext.getHardwareInfo());

        //textField.appendText(m_ext.passString(message));

        addChild(textField)
    }
}
}

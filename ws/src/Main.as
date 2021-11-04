package {

import com.hurlant.crypto.tls.TLSConfig;
import com.hurlant.crypto.tls.TLSEngine;
import com.hurlant.crypto.tls.TLSSecurityParameters;
import com.hurlant.crypto.tls.TLSSocket;
import com.hurlant.util.der.PEM;
import com.worlize.websocket.WebSocket;
import com.worlize.websocket.WebSocketConfig;
import com.worlize.websocket.WebSocketErrorEvent;
import com.worlize.websocket.WebSocketEvent;
import com.worlize.websocket.WebSocketMessage;

import flash.display.Sprite;
import flash.events.Event;
import flash.events.IOErrorEvent;
import flash.events.SecurityErrorEvent;
import flash.net.SecureSocket;
import flash.utils.ByteArray;

public class Main extends Sprite {

    [Embed(source = "../assets/cert.pem", mimeType = "application/octet-stream")]
    private static const cert_pem: Class;

    function Main() {

        createWS();

    }

    private var websocket:WebSocket;

    private function createWS():void {

        websocket = new WebSocket(
                "ws://dev-praia1chat.pipastudios.com/ws?port=11101",
                "*" //,
                //"TLS1.2"//["dumb-increment-protocol", "lws-mirror-protocol", "fraggle-protocol"] //["superchat", "boringchat"]
        );

//        var config:WebSocketConfig = new WebSocketConfig();
//        config.assembleFragments = true;
//        websocket.config = config;

        websocket.debug = true;

        websocket.addEventListener(WebSocketEvent.CLOSED, handleWebSocketClosed);
        websocket.addEventListener(WebSocketEvent.OPEN, handleWebSocketOpen);
        websocket.addEventListener(WebSocketEvent.MESSAGE, handleWebSocketMessage);
        websocket.addEventListener(WebSocketErrorEvent.CONNECTION_FAIL, handleConnectionFail);
        websocket.connect();
    }


    private function handleWebSocketOpen(event:WebSocketEvent):void {
        trace("Connected");
        websocket.sendUTF("Hello World!\n");

        var binaryData:ByteArray = new ByteArray();
        binaryData.writeUTF("Hello as Binary Message!");
        websocket.sendBytes(binaryData);
    }

    private function handleWebSocketClosed(event:WebSocketEvent):void {
        trace("Disconnected");
    }

    private function handleConnectionFail(event:WebSocketErrorEvent):void {
        trace("Connection Failure: " + event.text);
    }

    private function handleWebSocketMessage(event:WebSocketEvent):void {
        if (event.message.type === WebSocketMessage.TYPE_UTF8) {
            trace("Got message: " + event.message.utf8Data);
        } else if (event.message.type === WebSocketMessage.TYPE_BINARY) {
            trace("Got binary message of length " + event.message.binaryData.length);

            trace("PLAU: " + event.message.binaryData);

        }
    }
}
}

package {
import flash.events.EventDispatcher;
import flash.events.IEventDispatcher;
import flash.external.ExtensionContext;

public class WindowsExtensionWrapper extends EventDispatcher {


    private var m_extContext : ExtensionContext = null;

    public function WindowsExtensionWrapper(target:IEventDispatcher = null) {
        super(target);
    }
}
}

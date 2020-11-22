#include "WindowsStore.h"
#include "FreSharpBridge.h"

extern "C" {
	CONTEXT_INIT(PSWS) {
		FREBRIDGE_INIT

		static FRENamedFunction extensionFunctions[] = {
			 MAP_FUNCTION(init),
			 MAP_FUNCTION(getProducts)
		};

		SET_FUNCTIONS
	}

	CONTEXT_FIN(PSWS) {
		FreSharpBridge::GetController()->OnFinalize();
	}
	EXTENSION_INIT(PSWS)
	EXTENSION_FIN(PSWS)

}


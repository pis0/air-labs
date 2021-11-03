// dllmain.cpp : Define o ponto de entrada para o aplicativo DLL.
#include "pch.h" 
#include <FlashRuntimeExtensions.h>
#include <assert.h>
#include <sstream>   
#include <iostream>
#include <algorithm>

//#include <winrt/base.h>
//#include <winrt/Windows.Services.Store.h>
//#include <winrt/Windows.Foundation.h>
//using namespace winrt;
//using namespace winrt::impl;
//susing namespace winrt::Windows::Services::Store;


/*TODO to review*/
#include <windows.h>
#include <Windows.Services.Store.h>
#include <wrl.h>
#include <wrl/wrappers/corewrappers.h>
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Foundation::Collections;
using namespace ABI::Windows::Services::Store;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;


#define CheckHr(hr) do { if (FAILED(hr)) __debugbreak(); } while (false)


std::string resultFilter(std::string s)
{
	/*
	std::regex newlines_r("\r+");
	auto result_r = std::regex_replace(s, newlines_r, "");
	std::regex newlines_n("\n+");
	auto result_n = std::regex_replace(result_r, newlines_n, "");
	std::regex linespace(" +");
	auto result_ls = std::regex_replace(result_n, linespace, ""); 
	*/

	s.erase(std::remove_if(
		s.begin(),
		s.end(),
		[](char c) { return std::isspace(c) || std::iscntrl(c); }), 
		s.end() 
	); 

	std::replace(
		s.begin(),
		s.end(),
		'\\', 
		'-'
	); 

	return s;
}

std::string getUUID()
{
	FILE* pipe = _popen("wmic csproduct get UUID | find /v \"UUID\"", "r");
	if (pipe == NULL) return "N/A";

	char buffer[128];
	std::stringstream  stros;
	while (fgets(buffer, sizeof buffer, pipe) != NULL) stros << buffer;
	_pclose(pipe);

	return resultFilter(stros.str());
}

std::string getHardwareProfileGuid()
{
	const int uuidLength = 39;
	char uuid[uuidLength] = { 0 };
	HW_PROFILE_INFO hwProfileInfo;
	if (GetCurrentHwProfile(&hwProfileInfo)) 
	{
		sprintf_s(uuid, uuidLength, "%ws", hwProfileInfo.szHwProfileGuid);
		std::string result(uuid); 

		return resultFilter(result.substr(1, uuidLength - 3));
	}
	return "N/A";	
}

std::string getDiskDrivePNPDeviceId()
{
	FILE* pipe = _popen("wmic diskdrive get Name,PNPDeviceId | find \"PHYSICALDRIVE0\"", "r");    
	if (pipe == NULL) return "N/A";

	char buffer[128];
	std::stringstream stros;
	while (fgets(buffer, sizeof buffer, pipe) != NULL) stros << buffer;
	_pclose(pipe);

	return resultFilter(stros.str().substr(20, stros.str().length() - 20));
}

std::string getMachineGuid()
{
	std::string ret = "N/A";
	char value[64];
	DWORD size = _countof(value);
	DWORD type = REG_SZ;
	HKEY key;
	LONG retKey = ::RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Cryptography", 0, KEY_READ | KEY_WOW64_64KEY, &key);
	LONG retVal = ::RegQueryValueExA(key, "MachineGuid", nullptr, &type, (LPBYTE)value, &size);
	if (retKey == ERROR_SUCCESS && retVal == ERROR_SUCCESS) {
		ret = value;
	}
	::RegCloseKey(key);

	return resultFilter(ret);
}

std::string getVolumeSerialNumber()
{
	const int volumeSerialLength = 9;
	char volumeSerial[volumeSerialLength] = { 0 };
	DWORD volumeSerialNumber = NULL;
	if (GetVolumeInformationA(
		NULL,
		NULL,
		NULL,
		&volumeSerialNumber,
		NULL,
		NULL,
		NULL,
		NULL
	) == 0) 
	{
		return "N/A";
	}

	sprintf_s(volumeSerial, volumeSerialLength, "%08lX", volumeSerialNumber);
	std::string result(volumeSerial);

	return resultFilter(result);
}

std::string getProcessorId()
{
	FILE* pipe = _popen("wmic cpu get ProcessorId | find /v \"ProcessorId\"", "r");
	if (pipe == NULL) return "N/A";

	char buffer[128];
	std::stringstream  stros;
	while (fgets(buffer, sizeof buffer, pipe) != NULL) stros << buffer;
	_pclose(pipe);

	return resultFilter(stros.str());
}


	FREObject ASGetHardwareInfo(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		FREObject retObj = NULL;

		std::stringstream  buffer;
		buffer << "{";
		buffer << "\"uuid\":\"" << getUUID() << "\",";
		buffer << "\"hardwareProfileGuid\":\"" << getHardwareProfileGuid() << "\",";
		buffer << "\"diskDrivePNPDeviceId\":\"" << getDiskDrivePNPDeviceId() << "\",";
		buffer << "\"machineGuid\":\"" << getMachineGuid() << "\",";
		buffer << "\"processorId\":\"" << getProcessorId() << "\",";
		buffer << "\"volumeSerialNumber\":\"" << getVolumeSerialNumber() << "\"";
		buffer << "}";

		std::string bufferString = buffer.str();

		const uint8_t* result = reinterpret_cast<const uint8_t*>(bufferString.c_str());
		FRENewObjectFromUTF8(bufferString.size(), result, &retObj);

		return retObj;
	}

	/*
	void OnPurchaseOperationDone(FREContext ctx, IAsyncOperation<StoreProductResult*>* operation, AsyncStatus status)
	{
		if (status != AsyncStatus::Completed)
		{		
			ComPtr<IAsyncInfo> asyncInfo;
			auto hr = operation->QueryInterface(__uuidof(asyncInfo), &asyncInfo);
			CheckHr(hr);

			HRESULT errorCode;
			hr = asyncInfo->get_ErrorCode(&errorCode);
			CheckHr(hr);

			std::stringstream buffer;
			buffer << "Failed. Error 0x" << std::hex << errorCode << std::endl;
			std::string bufferString = buffer.str();
			const uint8_t* result = reinterpret_cast<const uint8_t*>(bufferString.c_str());

			FREDispatchStatusEventAsync(ctx, (const uint8_t*)"LIST_PRODUCTS_COMPLETE", result);

			return;
		}

		ComPtr<IStoreProductResult> productResult;
		auto hr = operation->GetResults(&productResult);
		CheckHr(hr);

		ComPtr<IStoreProduct> product;
		hr = productResult->get_Product(&product);
		CheckHr(hr);

		
		//HSTRING title; 
		//hr = product->get_Title(&title);
		//CheckHr(hr);

		//HString hstr;
		//hstr.Set(title);
		//std::wstring ws = hstr.GetRawBuffer(nullptr);


		std::stringstream buffer;
		buffer << "Success. product: "; // << std::hex << product << std::endl;

		std::string bufferString = buffer.str();
		const uint8_t* result = reinterpret_cast<const uint8_t*>(bufferString.c_str());


		FREDispatchStatusEventAsync(ctx, (const uint8_t*)"LIST_PRODUCTS_COMPLETE", result);

	}
	*/


	FREObject ASListProducts(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		ComPtr<IStoreContextStatics> storeContextStatics;
		auto hr = RoGetActivationFactory(HStringReference(L"Windows.Services.Store.StoreContext").Get(), __uuidof(storeContextStatics), &storeContextStatics);
		CheckHr(hr);

		ComPtr<IStoreContext> storeContext;
		hr = storeContextStatics->GetDefault(&storeContext);
		CheckHr(hr);


		ComPtr<IAsyncOperation<StoreAppLicense*>> getLicenseOperation;
		hr = storeContext->GetAppLicenseAsync(&getLicenseOperation);
		CheckHr(hr);

		hr = getLicenseOperation->put_Completed(Callback<Implements<RuntimeClassFlags<ClassicCom>, IAsyncOperationCompletedHandler<StoreAppLicense*>, FtmBase>>(
			[ctx](IAsyncOperation<StoreAppLicense*>* operation, AsyncStatus status)
		{
			if (status != AsyncStatus::Completed)
			{
				// It failed for some reason. Find out why.
				ComPtr<IAsyncInfo> asyncInfo;
				auto hr = operation->QueryInterface(__uuidof(asyncInfo), &asyncInfo);
				CheckHr(hr);

				HRESULT errorCode;
				hr = asyncInfo->get_ErrorCode(&errorCode);
				CheckHr(hr);

				FREDispatchStatusEventAsync(ctx, (const uint8_t*)"LIST_PRODUCTS_COMPLETE", (const uint8_t*)"ERROR");
				return S_OK;
			}

			ComPtr<IStoreAppLicense> appLicense;
			auto hr = operation->GetResults(&appLicense);
			CheckHr(hr);

			boolean isActive, isTrial = false;

			hr = appLicense->get_IsActive(&isActive);
			CheckHr(hr);

			if (isActive)
			{
				hr = appLicense->get_IsTrial(&isTrial);
				CheckHr(hr);				
			}

			FREDispatchStatusEventAsync(ctx, (const uint8_t*)"LIST_PRODUCTS_COMPLETE", (const uint8_t*)"TRIAL");
			
			return S_OK;
		}).Get());
		CheckHr(hr);
		
		/*
		com_ptr<IStoreContextStatics> storeContextStatics;
		auto hr = RoGetActivationFactory(HStringReference(L"Windows.Services.Store.StoreContext").Get(), __uuidof(storeContextStatics), &storeContextStatics);
		CheckHr(hr);

		ComPtr<IStoreContext> storeContext;
		hr = storeContextStatics->GetDefault(&storeContext);
		CheckHr(hr);
		*/

		//ComPtr<IInitializeWithWindow> initWindow; 
		//hr = storeContext->QueryInterface(IID_PPV_ARGS(&initWindow));
		//hr = initWindow->Initialize(hwdlg);
		

		//Vector<HSTRING>* productKinds;		
		
		//wrapped_vector<HSTRING>* productKinds = {};
		//productKinds->Append(HStringReference(L"consumable").Get());
		//wrapped_vector<HSTRING>* storeIds = {};
		//storeIds->Append(HStringReference(L"9P6HBF77303B").Get());

		//ComPtr<IAsyncOperation<StoreProductQueryResult*>> storeProductsOperation;
		//hr = storeContext->GetStoreProductsAsync((__FIIterable_1_HSTRING_t*)productKinds, (__FIIterable_1_HSTRING_t*)storeIds, &storeProductsOperation);
		//CheckHr(hr);
		

		/*
		auto onCompletedCallback = Callback<Implements<RuntimeClassFlags<ClassicCom>, IAsyncOperationCompletedHandler<StoreProductQueryResult*>, FtmBase>>(
			[ctx](IAsyncOperation<StoreProductQueryResult*>* operation, AsyncStatus status)
			{
				//OnPurchaseOperationDone(ctx, operation, status);				
				FREDispatchStatusEventAsync(ctx, (const uint8_t*)"LIST_PRODUCTS_COMPLETE", (const uint8_t*)"SUCCESS");
				return S_OK;
			});
		hr = storeProductsOperation->put_Completed(onCompletedCallback.Get());
		CheckHr(hr);
		*/
		 
		FREObject retObj = NULL;
		std::stringstream  buffer;
		buffer << "{}" << std::endl;
		std::string bufferString = buffer.str();
		const uint8_t* result = reinterpret_cast<const uint8_t*>(bufferString.c_str());
		FRENewObjectFromUTF8(bufferString.size(), result, &retObj);		


		//FREDispatchStatusEventAsync(ctx, (const uint8_t*)"LIST_PRODUCTS_COMPLETE", (const uint8_t*)"SUCCESS");

		return retObj;
	}


/*
FREObject ASPassAString(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{

	// What this function does: reads a string passed in from ActionScript,
	// outputs it to the console, then sends it back to ActionScript.

	// This enumerator helps keep track of the parameters
	// we expect from ActionScript and their order. 
	// Not technically necessary, but a good habit:
	// saves you havhavnig to remember which parameter you should access as argv[ 3 ].
	enum
	{
		ARG_STRING_ARGUMENT = 0,
		ARG_COUNT
	};

	// Another good habit, though not a requirement:
	// ARG_COUNT will have the value of the number of arguments you expect.
	// The assertion will fire (in a debug build) to tell you
	// if you mistakenly passed the wrong number of arguments
	// from ActionScritpt.
	assert(ARG_COUNT == argc);

	// Read the ActionScript String object, packed here as a FREObject,
	// into a character array:
	uint32_t strLength = 0;
	const uint8_t* nativeCharArray = NULL;
	FREResult status = FREGetObjectAsUTF8(argv[ARG_STRING_ARGUMENT], &strLength, &nativeCharArray);

	FREObject retObj = NULL;

	if ((FRE_OK == status) && (0 < strLength) && (NULL != nativeCharArray))
	{
		// Read the characters into a c string...
		std::string nativeString((const char*)nativeCharArray);
		// ...and output it into the console to see what we received:
		std::stringstream  stros;
		stros << "This is the string we received from ActionScript: ";
		stros << nativeString;
		// Now let's put the characters back into a FREObject...
		FRENewObjectFromUTF8(strLength, nativeCharArray, &retObj);
	}

	// ... and send them back to ActionScript:
	return retObj;
}
*/

void PSWUcontextFinalizer(FREContext ctx)
{
	return;
}

void PSWUcontextInitializer(
	void* extData,
	const uint8_t* ctxType,
	FREContext ctx,
	uint32_t* numFunctionsToSet,
	const FRENamedFunction** functionsToSet)
{
	// Create mapping between function names and pointers in an array of FRENamedFunction.
	// These are the functions that you will call from ActionScript -
	// effectively the interface of your native library.
	// Each member of the array contains the following information:
	// { function name as it will be called from ActionScript,
	//   any data that should be passed to the function,
	//   a pointer to the implementation of the function in the native library }
	static FRENamedFunction extensionFunctions[] =
	{
		{ (const uint8_t*)"as_getHardwareInfo",NULL, &ASGetHardwareInfo },
		{ (const uint8_t*)"as_listProducts",NULL, &ASListProducts },
	};

	// Tell AIR how many functions there are in the array:
	*numFunctionsToSet = sizeof(extensionFunctions) / sizeof(FRENamedFunction);

	// Set the output parameter to point to the array we filled in:
	*functionsToSet = extensionFunctions;
}

extern "C"
{
	__declspec(dllexport) void PSWUExtensionInitializer(void** extData, FREContextInitializer* ctxInitializer, FREContextFinalizer* ctxFinalizer)
	{
		*ctxInitializer = &PSWUcontextInitializer; // The name of function that will intialize the extension context
		*ctxFinalizer = &PSWUcontextFinalizer; // The name of function that will finalize the extension context
	}
 
	__declspec(dllexport) void PSWUExtensionFinalizer(void* extData)
	{
		return;
	}
}


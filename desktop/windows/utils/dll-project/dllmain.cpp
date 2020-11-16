// dllmain.cpp : Define o ponto de entrada para o aplicativo DLL.
#include "pch.h" 
#include <FlashRuntimeExtensions.h>
#include <assert.h>
#include <sstream>   
#include <iostream>
#include <algorithm>



std::string filter(std::string s)
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

	return filter(stros.str());
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

		return filter(result.substr(1, uuidLength - 3)); 
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

	return filter(stros.str().substr(20, stros.str().length() - 20));
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

	return filter(ret);
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

	return filter(result);
}

std::string getProcessorId()
{
	FILE* pipe = _popen("wmic cpu get ProcessorId | find /v \"ProcessorId\"", "r");
	if (pipe == NULL) return "N/A";

	char buffer[128];
	std::stringstream  stros;
	while (fgets(buffer, sizeof buffer, pipe) != NULL) stros << buffer;
	_pclose(pipe);

	return filter(stros.str());
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

void contextFinalizer(FREContext ctx)
{
	return;
}

void contextInitializer(
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
	};

	// Tell AIR how many functions there are in the array:
	*numFunctionsToSet = sizeof(extensionFunctions) / sizeof(FRENamedFunction);

	// Set the output parameter to point to the array we filled in:
	*functionsToSet = extensionFunctions;
}

extern "C"
{
	__declspec(dllexport) void ExtensionInitializer(void** extData, FREContextInitializer* ctxInitializer, FREContextFinalizer* ctxFinalizer)
	{
		*ctxInitializer = &contextInitializer; // The name of function that will intialize the extension context
		*ctxFinalizer = &contextFinalizer; // The name of function that will finalize the extension context
	}
 
	__declspec(dllexport) void ExtensionFinalizer(void* extData)
	{
		return;
	}
}


// dllmain.cpp : Define o ponto de entrada para o aplicativo DLL.
#include "pch.h" 
#include <FlashRuntimeExtensions.h>
#include <string>
#include <assert.h>
#include <sstream> 
#include <intrin.h>

void cpuid(uint32_t op, uint32_t& eax, uint32_t& ebx, uint32_t& ecx, uint32_t& edx)
{
	int regs[4];
	__cpuid(regs, op);
	eax = regs[0];
	ebx = regs[1];
	ecx = regs[2];
	edx = regs[3];
}
enum CpuidFeatures
{
	PSN = 1 << 18, 
};

// TODO to fix
uint32_t cpuid_features()
{
	uint32_t eax, ebx, ecx, edx;
	cpuid(PSN, eax, ebx, ecx, edx);

	// byte swap
	int first = ((eax >> 24) & 0xff) | ((eax << 8) & 0xff0000) | ((eax >> 8) & 0xff00) | ((eax << 24) & 0xff000000);
	int last = ((edx >> 24) & 0xff) | ((edx << 8) & 0xff0000) | ((edx >> 8) & 0xff00) | ((edx << 24) & 0xff000000);

	std::stringstream buffer;

	scanf_s("%08X%08X", first, last);

	return edx;
}
std::string getProcessorSerialNumber()
{
	std::stringstream buffer;
	buffer << cpuid_features() << std::endl;
	return buffer.str();
}


std::string getVolumeSerialNumber()
{
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

	std::string stringVolumeSerialNumber = std::to_string(volumeSerialNumber);
	return stringVolumeSerialNumber;
}

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
		/*
		// Read the characters into a c string... 
		std::string nativeString((const char*)nativeCharArray);
		// ...and output it into the console to see what we received:
		std::stringstream  stros;
		stros << "This is the string we received from ActionScript: ";
		stros << nativeString;
		// Now let's put the characters back into a FREObject...
		FRENewObjectFromUTF8(strLength, nativeCharArray, &retObj);
		*/

		// TODO to review 
		std::string machineGuid = getProcessorSerialNumber(); // getVolumeSerialNumber(); 
		const uint8_t* result = reinterpret_cast<const uint8_t*>(machineGuid.c_str()); 
		FRENewObjectFromUTF8(machineGuid.size(), result, &retObj);

	}

	// ... and send them back to ActionScript:
	return retObj;
}


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
		{ (const uint8_t*)"as_passAString",NULL, &ASPassAString }
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


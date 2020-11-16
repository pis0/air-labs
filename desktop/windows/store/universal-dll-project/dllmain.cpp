#include "pch.h"
#include <collection.h>
#include <ppltasks.h>

#include <FlashRuntimeExtensions.h>
#include <sstream>   
#include <iostream>

using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Services::Store;



StoreContext^ storeContext;
FREObject ASCreateStoreContext(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    storeContext = StoreContext::GetDefault();   

    std::stringstream buffer;
    buffer << "{";
    buffer << "\"success\":" << true << ",";
    buffer << "\"data\":" << "null";
    buffer << "}";

    std::string bufferString = buffer.str();
    const uint8_t* result = reinterpret_cast<const uint8_t*>(bufferString.c_str());

    FREResult res = FREDispatchStatusEventAsync(ctx, (const uint8_t*)"CREATE_STORE_CONTEXT_COMPLETED", result);
   
    FREObject retObj = NULL;
    FRENewObjectFromUTF8(bufferString.size(), result, &retObj);
    return retObj;
}

FREObject ASListProducts(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{	  
	auto productKinds = ref new Vector<String^>({ "Consumable" });
	auto storeIds = ref new Vector<String^>({ "9P6HBF77303B" });
	create_task(storeContext->GetStoreProductsAsync(productKinds, storeIds)).then([ctx](StoreProductQueryResult^ addOns)
    {           
            std::stringstream buffer;
            buffer << "{";

            if (addOns->ExtendedError.Value != S_OK)
            {
                buffer << "\"success\":" << false << ",";
                buffer << "\"data\":" << "{" << "\"error\":\"" << addOns->ExtendedError.ToString()->Data() << "\"}";
            }
            else if (addOns->Products->Size == 0)
            {
                buffer << "\"success\":" << false << ",";
                buffer << "\"data\":" << "{" << "\"error\":\"" << "No configured Consumable Add-Ons found for this Store Product." << "\"}";
            }
            else
            {
                buffer << "\"success\":" << true << ",";
                buffer << "\"data\":" << "[";
                for (IKeyValuePair<String^, StoreProduct^>^ addOn : addOns->Products)
                {
                    StoreProduct^ product = addOn->Value;

                    auto title = product->Title;
                    auto price = product->Price->FormattedPrice;
                    auto inCollection = product->IsInUserCollection;
                    auto productKind = product->ProductKind;
                    auto storeId = product->StoreId;
                   
                    buffer << "{";
                    buffer << "\"title\":\"" << title->Data() << "\",";
                    buffer << "\"price\":\"" << price->Data() << "\",";
                    buffer << "\"productKind\":\"" << productKind->Data() << "\",";
                    buffer << "\"inCollection\":" << inCollection << ",";
                    buffer << "\"storeId\":\"" << storeId->Data() << "\"";
                    buffer << "}";                   

                }
                buffer << "]";
            }

            buffer << "}";

            std::string bufferString = buffer.str();
            const uint8_t* result = reinterpret_cast<const uint8_t*>(bufferString.c_str());

            FREResult res = FREDispatchStatusEventAsync(ctx, (const uint8_t*)"LIST_PRODUCTS_COMPLETED", result);
            //if (res == FRE_INVALID_ARGUMENT) {
            //}
            //else if (res == FRE_OK) {
            //}
            //else {
            //}

	}, task_continuation_context::get_current_winrt_context());


    FREObject retObj = NULL;
    std::stringstream  buffer;
    buffer << "{}";
    std::string bufferString = buffer.str();
    const uint8_t* result = reinterpret_cast<const uint8_t*>(bufferString.c_str());
    FRENewObjectFromUTF8(bufferString.size(), result, &retObj);
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
    static FRENamedFunction extensionFunctions[] =
    {
        { (const uint8_t*)"as_createStoreContext",NULL, &ASCreateStoreContext },
        { (const uint8_t*)"as_listProducts",NULL, &ASListProducts },
    };

    *numFunctionsToSet = sizeof(extensionFunctions) / sizeof(FRENamedFunction);
    *functionsToSet = extensionFunctions;
}

extern "C"
{
    __declspec(dllexport) void ExtensionInitializer(void** extData, FREContextInitializer* ctxInitializer, FREContextFinalizer* ctxFinalizer)
    {
        *ctxInitializer = &contextInitializer; 
        *ctxFinalizer = &contextFinalizer; 
    }

    __declspec(dllexport) void ExtensionFinalizer(void* extData)
    {
        return;
    }
}
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using TuaRua.FreSharp;
using TuaRua.FreSharp.Exceptions;
using FREObject = System.IntPtr;
using FREContext = System.IntPtr;
using Hwnd = System.IntPtr;

// TODO to review
using Windows.Services.Store;
using System.Runtime.InteropServices;


namespace HelloWorldLib {

    [ComImport]
    [Guid("3E68D4BD-7135-4D10-8018-9FB6D9F33FA1")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IInitializeWithWindow
    {
        void Initialize(IntPtr hwnd);
    }

    public class MainController : FreSharpMainController {
        private Hwnd _airWindow;

        // Must have this function. It exposes the methods to our entry C++.
        public string[] GetFunctions() {
            FunctionsDict =
                new Dictionary<string, Func<FREObject, uint, FREObject[], FREObject>> {
                    {"init", InitController}
                    , {"sayHello", SayHello}
                };
            return FunctionsDict.Select(kvp => kvp.Key).ToArray();
        }

       


        private FREObject SayHello(FREContext ctx, uint argc, FREObject[] argv) {
            if (argv[0] == FREObject.Zero) return new FreArgException().RawValue;
            if (argv[1] == FREObject.Zero) return new FreArgException().RawValue;
            if (argv[2] == FREObject.Zero) return new FreArgException().RawValue;

            try {
                var myString = argv[0].AsString();
                var uppercase = argv[1].AsBool();
                var numRepeats = argv[2].AsInt();

                for (var i = 0; i < numRepeats; i++) {
                    Trace(i);
                }

                var ret = myString;
                if (uppercase) {
                    ret = ret.ToUpper();
                }

                DispatchEvent("MY_EVENT", "SayHello");

                // TODO to fix (get products test)
                var storeContext = StoreContext.GetDefault();
                    string[] productKinds = new string[] { "UnmanagedConsumable" };
                    string[] storeIds = new string[] {
                        //"9NRKGGCVC12Q",
                        "9MT44Q75C27T"
                        //"9P6HBF77303B" 
                    };
                    var addOns = storeContext.GetStoreProductsAsync(productKinds, storeIds);
                    addOns.Completed = (a, b) =>
                    {
                        var results = a.GetResults();
                        if (results.ExtendedError != null)
                        {
                            var errorMessage = "GetStoreProductsAsync error: " + results.ExtendedError.Message;
                            DispatchEvent("MY_EVENT", errorMessage);
                            Trace(errorMessage);
                        }
                        else if (results.Products.Count == 0)
                        {
                            var errorMessage = "GetStoreProductsAsync error: No configured products found for this Store Product.";
                            DispatchEvent("MY_EVENT", errorMessage);
                            Trace(errorMessage);
                        }
                        else
                        {
                            foreach (StoreProduct product in results.Products.Values)
                            {
                                var Title = product.Title;
                                var Price = product.Price.FormattedPrice;
                                var InCollection = product.IsInUserCollection;
                                var ProductKind = product.ProductKind;
                                var StoreId = product.StoreId;

                                var errorMessage = $"{Title} ({ProductKind}) {Price}, InUserCollection:{InCollection}, StoreId:{StoreId}";
                                DispatchEvent("MY_EVENT", errorMessage);
                                Trace(errorMessage);


                                // TODO to fix (purchase product test)
                                errorMessage = $"RequestPurchaseAsync - product:{StoreId}";
                                DispatchEvent("MY_EVENT", errorMessage);
                                Trace(errorMessage);

                                var initWindow = (IInitializeWithWindow)(object)storeContext;
                                initWindow.Initialize(Process.GetCurrentProcess().MainWindowHandle); 
                         
                                var purchaseRequest = storeContext.RequestPurchaseAsync(StoreId);                                
                                purchaseRequest.Completed = (x, y) =>
                                {
                                    errorMessage = $"RequestPurchaseAsync - product:{StoreId}";
                                    DispatchEvent("MY_EVENT", errorMessage);
                                    Trace(errorMessage);

                                    var purchaseResults = x.GetResults();
                                    if (purchaseResults.ExtendedError != null)
                                    {
                                        errorMessage = "RequestPurchaseAsync error: " + purchaseResults.ExtendedError.Message;
                                        DispatchEvent("MY_EVENT", errorMessage);
                                        Trace(errorMessage);
                                        return;                                    
                                    }
                                    switch (purchaseResults.Status)
                                    {
                                        case StorePurchaseStatus.AlreadyPurchased:                                            
                                            errorMessage = $"You already bought this AddOn";
                                            DispatchEvent("MY_EVENT", errorMessage);
                                            Trace(errorMessage);
                                            break;

                                        case StorePurchaseStatus.Succeeded:                                            
                                            errorMessage = $"You bought {Title}.";
                                            DispatchEvent("MY_EVENT", errorMessage);
                                            Trace(errorMessage);
                                            break;

                                        case StorePurchaseStatus.NotPurchased:                                            
                                            errorMessage = $"Product was not purchased, it may have been canceled.";
                                            DispatchEvent("MY_EVENT", errorMessage);
                                            Trace(errorMessage);
                                            break;

                                        case StorePurchaseStatus.NetworkError:                                            
                                            errorMessage = $"Product was not purchased due to a network error.";
                                            DispatchEvent("MY_EVENT", errorMessage);
                                            Trace(errorMessage);
                                            break;

                                        case StorePurchaseStatus.ServerError:                                            
                                            errorMessage = $"Product was not purchased due to a server error.";
                                            DispatchEvent("MY_EVENT", errorMessage);
                                            Trace(errorMessage);
                                            break;

                                        default:
                                            errorMessage = "Product was not purchased due to an unknown error.";
                                            DispatchEvent("MY_EVENT", errorMessage);
                                            Trace(errorMessage);
                                            break;
                                    }
                                };
                                

                            }



                            


                        }

                    };




                return ret.ToFREObject();
            }
            catch (Exception e) {
                return new FreException(e).RawValue; //return as3 error and throw in swc
            }
        }

        private FREObject InitController(FREContext ctx, uint argc, FREObject[] argv) {
            // get a reference to the AIR Window HWND
            _airWindow = Process.GetCurrentProcess().MainWindowHandle;

            // Turn on FreSharp logging
            FreSharpLogger.GetInstance().Context = Context;

            return FREObject.Zero;
        }

        public override void OnFinalize() { }
        public override string TAG => "MainController";
    }
}
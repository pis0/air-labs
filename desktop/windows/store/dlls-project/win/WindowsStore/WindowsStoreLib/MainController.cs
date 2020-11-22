using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Web.Script.Serialization; 
using Windows.Services.Store;
using TuaRua.FreSharp;
using TuaRua.FreSharp.Exceptions;

using FREObject = System.IntPtr;
using FREContext = System.IntPtr;
using Hwnd = System.IntPtr;

namespace WindowsStoreLib {

    [ComImport]
    [Guid("3E68D4BD-7135-4D10-8018-9FB6D9F33FA1")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IInitializeWithWindow
    {
        void Initialize(IntPtr hwnd);
    }

    public class Actions
    {
        public static string GET_PRODUCTS = "GET_PRODUCTS";
    }

    public class EventData
    {
        public string Action { get; set; }
        public bool Success { get; set; }
        public string Data { get; set; }
    }

    public class Product
    {
        public string Title { get; set; }
        public string Price { get; set; }        
        public string ProductKind { get; set; }
        public string StoreId { get; set; }
        public bool InCollection { get; set; }

        override public string ToString()
        {
            return $"{Title} ({ProductKind}) {Price}, InUserCollection:{InCollection}, StoreId:{StoreId}";
        }
    }

    public class MainController : FreSharpMainController {
        private Hwnd _airWindow;
        private const string EVENT_TAG = "WINDOWS_STORE_EVENT";

        public string[] GetFunctions() {
            FunctionsDict = new Dictionary<string, Func<FREObject, uint, FREObject[], FREObject>>
            {
                {"init", InitController},
                {"getProducts", GetProducts}
            }; 

            return FunctionsDict.Select(kvp => kvp.Key).ToArray();
        }

        private StoreContext storeContext;
        private void UpdateStoreContext()
        {
            if(storeContext == null) storeContext = StoreContext.GetDefault();
            var initWindow = (IInitializeWithWindow)(object)storeContext;
            initWindow.Initialize(Process.GetCurrentProcess().MainWindowHandle);
        }

        private FREObject GetProducts(FREContext ctx, uint argc, FREObject[] argv) 
        {
            if (argv[0] == FREObject.Zero) return new FreArgException().RawValue;

            try
            {
               
                string rawParamsData = argv[0].AsString();

                var paramsData = new JavaScriptSerializer().Deserialize(
                    rawParamsData,
                    typeof(Dictionary<string, List<string>>)
                ) as Dictionary<string, List<string>>;               

                foreach (KeyValuePair<string, List<string>> entry in paramsData)
                {
                    Trace($"key: {entry.Key}, value: {entry.Value}");
                    foreach (string value in entry.Value)
                    {
                        Trace($"value: {value}");
                    }
                }
                
                List<string> productKinds;
                List<string> storeIds;
                paramsData.TryGetValue("productKinds", out productKinds);
                paramsData.TryGetValue("storeIds", out storeIds);


                Trace($"getting products...");
                UpdateStoreContext();
                var addOns = storeContext.GetStoreProductsAsync(productKinds, storeIds);
                addOns.Completed = (a, b) =>
                {
                    var results = a.GetResults();
                    if (results.ExtendedError != null)
                    {
                        var errorMessage = "GetStoreProductsAsync error: " + results.ExtendedError.Message;                        
                        Trace(errorMessage);
                        DispatchEvent(EVENT_TAG, new JavaScriptSerializer().Serialize(new EventData(){
                            Action = Actions.GET_PRODUCTS,
                            Success = false,
                            Data = errorMessage
                        }));
                    }
                    else if (results.Products.Count == 0)
                    {
                        var errorMessage = "GetStoreProductsAsync error: No configured products found for this Store Product.";                        
                        Trace(errorMessage);
                        DispatchEvent(EVENT_TAG, new JavaScriptSerializer().Serialize(new EventData()
                        {
                            Action = Actions.GET_PRODUCTS,
                            Success = false,
                            Data = errorMessage
                        }));
                    }
                    else
                    {
                        var successData = new List<Product>();
                        foreach (StoreProduct product in results.Products.Values)
                        {
                            var obj = new Product()
                            {
                                Title = product.Title,
                                Price = product.Price.FormattedBasePrice,
                                InCollection = product.IsInUserCollection,
                                ProductKind = product.ProductKind,
                                StoreId = product.StoreId,
                            };

                            var message = obj.ToString();                            
                            Trace(message);
                            successData.Add(obj);
                        }
                        DispatchEvent(EVENT_TAG, new JavaScriptSerializer().Serialize(new EventData()
                        {
                            Action = Actions.GET_PRODUCTS,
                            Success = true,
                            Data = new JavaScriptSerializer().Serialize(successData)
                        }));
                    }

                };


                return new FREObject();
            }
            catch (Exception e)
            {
                return new FreException(e).RawValue;
            }
            


            
            /*
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

            */
        }

        private FREObject InitController(FREContext ctx, uint argc, FREObject[] argv) 
        {           
            _airWindow = Process.GetCurrentProcess().MainWindowHandle;          
            FreSharpLogger.GetInstance().Context = Context;

            return FREObject.Zero;
        }

        public override void OnFinalize() { }
        public override string TAG => "MainController";
    }
}
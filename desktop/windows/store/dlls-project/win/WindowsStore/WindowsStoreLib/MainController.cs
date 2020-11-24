using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Web.Script.Serialization;
using System.Threading.Tasks;
using Windows.Services.Store;
using Windows.System;
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
        public static string CONSUME = "CONSUME";
        public static string PURCHASE = "PURCHASE";        
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
        private StoreContext storeContext;

        private const string EVENT_TAG = "WINDOWS_STORE_EVENT";

        public string[] GetFunctions() {
            FunctionsDict = new Dictionary<string, Func<FREObject, uint, FREObject[], FREObject>>
            {
                {"init", InitController},
                {"getProducts", GetProducts},
                {"purchase", Purchase},
                {"consume", Consume},
            }; 

            return FunctionsDict.Select(kvp => kvp.Key).ToArray();
        }

        
        private void UpdateStoreContext(Action callback)
        {
            Task.Run(() =>
            {
                var users = User.FindAllAsync(UserType.LocalUser, UserAuthenticationStatus.LocallyAuthenticated);
                users.Completed = (a, b) =>
                {
                    var results = a.GetResults();
                    var user = results.FirstOrDefault();

                    if (storeContext == null) storeContext = StoreContext.GetForUser(user);
                    var initWindow = (IInitializeWithWindow)(object)storeContext;
                    initWindow.Initialize(Process.GetCurrentProcess().MainWindowHandle);

                    callback.Invoke();

                };
            });

            //if (storeContext == null) storeContext = StoreContext.GetDefault();
            //var initWindow = (IInitializeWithWindow)(object)storeContext;
            //initWindow.Initialize(Process.GetCurrentProcess().MainWindowHandle);
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

                    UpdateStoreContext(() => {
                        var addOns = storeContext.GetStoreProductsAsync(productKinds, storeIds);
                        addOns.Completed = (a, b) =>
                            {

                                Trace($"GetStoreProductsAsync completed");

                                void GetProductsDispatch(bool success, string message)
                                {
                                    Trace(message);
                                    DispatchEvent(EVENT_TAG, new JavaScriptSerializer().Serialize(new EventData()
                                    {
                                        Action = Actions.GET_PRODUCTS,
                                        Success = success,
                                        Data = message
                                    }));
                                }

                                var results = a.GetResults();
                                if (results.ExtendedError != null)
                                {
                                    GetProductsDispatch(false, $"GetStoreProductsAsync error: {results.ExtendedError.Message}");
                                }
                                else if (results.Products.Count == 0)
                                {
                                    GetProductsDispatch(false, "GetStoreProductsAsync error: No configured products found for this Store Product.");
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
                                    GetProductsDispatch(true, new JavaScriptSerializer().Serialize(successData));
                                }

                            };
                    
                    });

                return new FREObject();
            }
            catch (Exception e)
            {
                return new FreException(e).RawValue;
            }
        }

        private FREObject Purchase(FREContext ctx, uint argc, FREObject[] argv)
        {

            if (argv[0] == FREObject.Zero) return new FreArgException().RawValue;
            
            try
            {

                var productStoreId = argv[0].AsString();

                UpdateStoreContext(() => {                   
                       var purchaseRequest = storeContext.RequestPurchaseAsync(productStoreId);       
                       purchaseRequest.Completed = (a, b) =>
                       {
                           Trace($"RequestPurchaseAsync completed - product:{productStoreId}");


                           void PurchaseDispatch(bool success, string message)
                           {
                               Trace(message);
                               DispatchEvent(EVENT_TAG, new JavaScriptSerializer().Serialize(new EventData()
                               {
                                   Action = Actions.PURCHASE,
                                   Success = success,
                                   Data = message
                               }));
                           }

                           var purchaseResults = a.GetResults();
                           if (purchaseResults.ExtendedError != null)
                           {
                               PurchaseDispatch(false, $"RequestPurchaseAsync error: {purchaseResults.ExtendedError.Message}");
                               return;
                           }                   

                           switch (purchaseResults.Status)
                           {
                               case StorePurchaseStatus.Succeeded:
                                   PurchaseDispatch(true, $"You bought the product {productStoreId}.");
                                   break;
                               case StorePurchaseStatus.AlreadyPurchased:              
                                   PurchaseDispatch(false, $"You already bought this AddOn");
                                   break;    
                               case StorePurchaseStatus.NotPurchased:
                                   PurchaseDispatch(false, $"Product was not purchased, it may have been canceled.");
                                   break;
                               case StorePurchaseStatus.NetworkError:
                                   PurchaseDispatch(false, $"Product was not purchased due to a network error.");
                                   break;
                               case StorePurchaseStatus.ServerError:
                                   PurchaseDispatch(false, $"Product was not purchased due to a server error.");
                                   break;   
                               default:
                                   PurchaseDispatch(false, $"Product was not purchased due to an unknown error.");
                                   break;
                           }

                       };
                });


                return new FREObject();
            }
            catch (Exception e)
            {
                return new FreException(e).RawValue;
            }

        }


        private FREObject Consume(FREContext ctx, uint argc, FREObject[] argv)
        {

            if (argv[0] == FREObject.Zero) return new FreArgException().RawValue;

            try
            {
                var productStoreId = argv[0].AsString();
                var trackingId = Guid.NewGuid();

                Trace($"consume product...");

                UpdateStoreContext(() => {

                        var consume = storeContext.ReportConsumableFulfillmentAsync(productStoreId, Convert.ToUInt32(1), trackingId);
                        consume.Completed = (a, b) =>
                        {
                            Trace($"Consume completed");

                            void ConsumeDispatch(bool success, string message)
                            {
                                Trace(message);
                                DispatchEvent(EVENT_TAG, new JavaScriptSerializer().Serialize(new EventData()
                                {
                                    Action = Actions.CONSUME,
                                    Success = success,
                                    Data = message
                                }));
                            }

                            var results = a.GetResults();
                            if (results.ExtendedError != null)
                            {
                                ConsumeDispatch(false, $"ReportConsumableFulfillmentAsync error: {results.ExtendedError.Message}");
                                return;
                            }

                            switch (results.Status)
                            {
                                case StoreConsumableStatus.Succeeded:
                                    ConsumeDispatch(true, $"Successful fulfillment! Balance Remaining: {results.BalanceRemaining}");
                                    break;
                                case StoreConsumableStatus.InsufficentQuantity:
                                    ConsumeDispatch(false, $"Insufficient Quantity! Balance Remaining: {results.BalanceRemaining}");
                                    break;    
                                case StoreConsumableStatus.NetworkError:
                                    ConsumeDispatch(false, $"Network error fulfilling consumable.");
                                    break;
                                case StoreConsumableStatus.ServerError:
                                    ConsumeDispatch(false, $"Server error fulfilling consumable.");
                                    break;
                                default:
                                    ConsumeDispatch(false, $"Unknown error fulfilling consumable.");
                                    break;                           
                            }
                        };
                });

                return new FREObject();
            }
            catch (Exception e)
            {
                return new FreException(e).RawValue;
            }
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
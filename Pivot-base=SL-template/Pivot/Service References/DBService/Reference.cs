﻿//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:4.0.30319.1
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

// 
// This code was auto-generated by Microsoft.Silverlight.ServiceReference, version 4.0.50826.0
// 
namespace Pivot.DBService {
    
    
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ServiceModel.ServiceContractAttribute(ConfigurationName="DBService.DBService")]
    public interface DBService {
        
        [System.ServiceModel.OperationContractAttribute(AsyncPattern=true, Action="http://tempuri.org/DBService/GetDeflate", ReplyAction="http://tempuri.org/DBService/GetDeflateResponse")]
        System.IAsyncResult BeginGetDeflate(Pivot.DBService.GetDeflateRequest request, System.AsyncCallback callback, object asyncState);
        
        Pivot.DBService.GetDeflateResponse EndGetDeflate(System.IAsyncResult result);
        
        [System.ServiceModel.OperationContractAttribute(AsyncPattern=true, Action="http://tempuri.org/DBService/GetGZip", ReplyAction="http://tempuri.org/DBService/GetGZipResponse")]
        System.IAsyncResult BeginGetGZip(Pivot.DBService.GetGZipRequest request, System.AsyncCallback callback, object asyncState);
        
        Pivot.DBService.GetGZipResponse EndGetGZip(System.IAsyncResult result);
        
        [System.ServiceModel.OperationContractAttribute(AsyncPattern=true, Action="http://tempuri.org/DBService/Get", ReplyAction="http://tempuri.org/DBService/GetResponse")]
        System.IAsyncResult BeginGet(Pivot.DBService.GetRequest request, System.AsyncCallback callback, object asyncState);
        
        Pivot.DBService.GetResponse EndGet(System.IAsyncResult result);
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
    [System.ServiceModel.MessageContractAttribute(IsWrapped=false)]
    public partial class GetDeflateRequest {
        
        [System.ServiceModel.MessageBodyMemberAttribute(Name="GetDeflate", Namespace="http://tempuri.org/", Order=0)]
        public Pivot.DBService.GetDeflateRequestBody Body;
        
        public GetDeflateRequest() {
        }
        
        public GetDeflateRequest(Pivot.DBService.GetDeflateRequestBody Body) {
            this.Body = Body;
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
    [System.Runtime.Serialization.DataContractAttribute(Namespace="http://tempuri.org/")]
    public partial class GetDeflateRequestBody {
        
        [System.Runtime.Serialization.DataMemberAttribute(EmitDefaultValue=false, Order=0)]
        public string query;
        
        public GetDeflateRequestBody() {
        }
        
        public GetDeflateRequestBody(string query) {
            this.query = query;
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
    [System.ServiceModel.MessageContractAttribute(IsWrapped=false)]
    public partial class GetDeflateResponse {
        
        [System.ServiceModel.MessageBodyMemberAttribute(Name="GetDeflateResponse", Namespace="http://tempuri.org/", Order=0)]
        public Pivot.DBService.GetDeflateResponseBody Body;
        
        public GetDeflateResponse() {
        }
        
        public GetDeflateResponse(Pivot.DBService.GetDeflateResponseBody Body) {
            this.Body = Body;
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
    [System.Runtime.Serialization.DataContractAttribute(Namespace="http://tempuri.org/")]
    public partial class GetDeflateResponseBody {
        
        [System.Runtime.Serialization.DataMemberAttribute(EmitDefaultValue=false, Order=0)]
        public string GetDeflateResult;
        
        public GetDeflateResponseBody() {
        }
        
        public GetDeflateResponseBody(string GetDeflateResult) {
            this.GetDeflateResult = GetDeflateResult;
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
    [System.ServiceModel.MessageContractAttribute(IsWrapped=false)]
    public partial class GetGZipRequest {
        
        [System.ServiceModel.MessageBodyMemberAttribute(Name="GetGZip", Namespace="http://tempuri.org/", Order=0)]
        public Pivot.DBService.GetGZipRequestBody Body;
        
        public GetGZipRequest() {
        }
        
        public GetGZipRequest(Pivot.DBService.GetGZipRequestBody Body) {
            this.Body = Body;
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
    [System.Runtime.Serialization.DataContractAttribute(Namespace="http://tempuri.org/")]
    public partial class GetGZipRequestBody {
        
        [System.Runtime.Serialization.DataMemberAttribute(EmitDefaultValue=false, Order=0)]
        public string query;
        
        public GetGZipRequestBody() {
        }
        
        public GetGZipRequestBody(string query) {
            this.query = query;
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
    [System.ServiceModel.MessageContractAttribute(IsWrapped=false)]
    public partial class GetGZipResponse {
        
        [System.ServiceModel.MessageBodyMemberAttribute(Name="GetGZipResponse", Namespace="http://tempuri.org/", Order=0)]
        public Pivot.DBService.GetGZipResponseBody Body;
        
        public GetGZipResponse() {
        }
        
        public GetGZipResponse(Pivot.DBService.GetGZipResponseBody Body) {
            this.Body = Body;
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
    [System.Runtime.Serialization.DataContractAttribute(Namespace="http://tempuri.org/")]
    public partial class GetGZipResponseBody {
        
        [System.Runtime.Serialization.DataMemberAttribute(EmitDefaultValue=false, Order=0)]
        public string GetGZipResult;
        
        public GetGZipResponseBody() {
        }
        
        public GetGZipResponseBody(string GetGZipResult) {
            this.GetGZipResult = GetGZipResult;
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
    [System.ServiceModel.MessageContractAttribute(IsWrapped=false)]
    public partial class GetRequest {
        
        [System.ServiceModel.MessageBodyMemberAttribute(Name="Get", Namespace="http://tempuri.org/", Order=0)]
        public Pivot.DBService.GetRequestBody Body;
        
        public GetRequest() {
        }
        
        public GetRequest(Pivot.DBService.GetRequestBody Body) {
            this.Body = Body;
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
    [System.Runtime.Serialization.DataContractAttribute(Namespace="http://tempuri.org/")]
    public partial class GetRequestBody {
        
        [System.Runtime.Serialization.DataMemberAttribute(EmitDefaultValue=false, Order=0)]
        public string query;
        
        public GetRequestBody() {
        }
        
        public GetRequestBody(string query) {
            this.query = query;
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
    [System.ServiceModel.MessageContractAttribute(IsWrapped=false)]
    public partial class GetResponse {
        
        [System.ServiceModel.MessageBodyMemberAttribute(Name="GetResponse", Namespace="http://tempuri.org/", Order=0)]
        public Pivot.DBService.GetResponseBody Body;
        
        public GetResponse() {
        }
        
        public GetResponse(Pivot.DBService.GetResponseBody Body) {
            this.Body = Body;
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
    [System.Runtime.Serialization.DataContractAttribute(Namespace="http://tempuri.org/")]
    public partial class GetResponseBody {
        
        [System.Runtime.Serialization.DataMemberAttribute(EmitDefaultValue=false, Order=0)]
        public System.Xml.Linq.XElement GetResult;
        
        public GetResponseBody() {
        }
        
        public GetResponseBody(System.Xml.Linq.XElement GetResult) {
            this.GetResult = GetResult;
        }
    }
    
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    public interface DBServiceChannel : Pivot.DBService.DBService, System.ServiceModel.IClientChannel {
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    public partial class GetDeflateCompletedEventArgs : System.ComponentModel.AsyncCompletedEventArgs {
        
        private object[] results;
        
        public GetDeflateCompletedEventArgs(object[] results, System.Exception exception, bool cancelled, object userState) : 
                base(exception, cancelled, userState) {
            this.results = results;
        }
        
        public string Result {
            get {
                base.RaiseExceptionIfNecessary();
                return ((string)(this.results[0]));
            }
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    public partial class GetGZipCompletedEventArgs : System.ComponentModel.AsyncCompletedEventArgs {
        
        private object[] results;
        
        public GetGZipCompletedEventArgs(object[] results, System.Exception exception, bool cancelled, object userState) : 
                base(exception, cancelled, userState) {
            this.results = results;
        }
        
        public string Result {
            get {
                base.RaiseExceptionIfNecessary();
                return ((string)(this.results[0]));
            }
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    public partial class GetCompletedEventArgs : System.ComponentModel.AsyncCompletedEventArgs {
        
        private object[] results;
        
        public GetCompletedEventArgs(object[] results, System.Exception exception, bool cancelled, object userState) : 
                base(exception, cancelled, userState) {
            this.results = results;
        }
        
        public System.Xml.Linq.XElement Result {
            get {
                base.RaiseExceptionIfNecessary();
                return ((System.Xml.Linq.XElement)(this.results[0]));
            }
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    public partial class DBServiceClient : System.ServiceModel.ClientBase<Pivot.DBService.DBService>, Pivot.DBService.DBService {
        
        private BeginOperationDelegate onBeginGetDeflateDelegate;
        
        private EndOperationDelegate onEndGetDeflateDelegate;
        
        private System.Threading.SendOrPostCallback onGetDeflateCompletedDelegate;
        
        private BeginOperationDelegate onBeginGetGZipDelegate;
        
        private EndOperationDelegate onEndGetGZipDelegate;
        
        private System.Threading.SendOrPostCallback onGetGZipCompletedDelegate;
        
        private BeginOperationDelegate onBeginGetDelegate;
        
        private EndOperationDelegate onEndGetDelegate;
        
        private System.Threading.SendOrPostCallback onGetCompletedDelegate;
        
        private BeginOperationDelegate onBeginOpenDelegate;
        
        private EndOperationDelegate onEndOpenDelegate;
        
        private System.Threading.SendOrPostCallback onOpenCompletedDelegate;
        
        private BeginOperationDelegate onBeginCloseDelegate;
        
        private EndOperationDelegate onEndCloseDelegate;
        
        private System.Threading.SendOrPostCallback onCloseCompletedDelegate;
        
        public DBServiceClient() {
        }
        
        public DBServiceClient(string endpointConfigurationName) : 
                base(endpointConfigurationName) {
        }
        
        public DBServiceClient(string endpointConfigurationName, string remoteAddress) : 
                base(endpointConfigurationName, remoteAddress) {
        }
        
        public DBServiceClient(string endpointConfigurationName, System.ServiceModel.EndpointAddress remoteAddress) : 
                base(endpointConfigurationName, remoteAddress) {
        }
        
        public DBServiceClient(System.ServiceModel.Channels.Binding binding, System.ServiceModel.EndpointAddress remoteAddress) : 
                base(binding, remoteAddress) {
        }
        
        public System.Net.CookieContainer CookieContainer {
            get {
                System.ServiceModel.Channels.IHttpCookieContainerManager httpCookieContainerManager = this.InnerChannel.GetProperty<System.ServiceModel.Channels.IHttpCookieContainerManager>();
                if ((httpCookieContainerManager != null)) {
                    return httpCookieContainerManager.CookieContainer;
                }
                else {
                    return null;
                }
            }
            set {
                System.ServiceModel.Channels.IHttpCookieContainerManager httpCookieContainerManager = this.InnerChannel.GetProperty<System.ServiceModel.Channels.IHttpCookieContainerManager>();
                if ((httpCookieContainerManager != null)) {
                    httpCookieContainerManager.CookieContainer = value;
                }
                else {
                    throw new System.InvalidOperationException("Unable to set the CookieContainer. Please make sure the binding contains an HttpC" +
                            "ookieContainerBindingElement.");
                }
            }
        }
        
        public event System.EventHandler<GetDeflateCompletedEventArgs> GetDeflateCompleted;
        
        public event System.EventHandler<GetGZipCompletedEventArgs> GetGZipCompleted;
        
        public event System.EventHandler<GetCompletedEventArgs> GetCompleted;
        
        public event System.EventHandler<System.ComponentModel.AsyncCompletedEventArgs> OpenCompleted;
        
        public event System.EventHandler<System.ComponentModel.AsyncCompletedEventArgs> CloseCompleted;
        
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
        System.IAsyncResult Pivot.DBService.DBService.BeginGetDeflate(Pivot.DBService.GetDeflateRequest request, System.AsyncCallback callback, object asyncState) {
            return base.Channel.BeginGetDeflate(request, callback, asyncState);
        }
        
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
        private System.IAsyncResult BeginGetDeflate(string query, System.AsyncCallback callback, object asyncState) {
            Pivot.DBService.GetDeflateRequest inValue = new Pivot.DBService.GetDeflateRequest();
            inValue.Body = new Pivot.DBService.GetDeflateRequestBody();
            inValue.Body.query = query;
            return ((Pivot.DBService.DBService)(this)).BeginGetDeflate(inValue, callback, asyncState);
        }
        
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
        Pivot.DBService.GetDeflateResponse Pivot.DBService.DBService.EndGetDeflate(System.IAsyncResult result) {
            return base.Channel.EndGetDeflate(result);
        }
        
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
        private string EndGetDeflate(System.IAsyncResult result) {
            Pivot.DBService.GetDeflateResponse retVal = ((Pivot.DBService.DBService)(this)).EndGetDeflate(result);
            return retVal.Body.GetDeflateResult;
        }
        
        private System.IAsyncResult OnBeginGetDeflate(object[] inValues, System.AsyncCallback callback, object asyncState) {
            string query = ((string)(inValues[0]));
            return this.BeginGetDeflate(query, callback, asyncState);
        }
        
        private object[] OnEndGetDeflate(System.IAsyncResult result) {
            string retVal = this.EndGetDeflate(result);
            return new object[] {
                    retVal};
        }
        
        private void OnGetDeflateCompleted(object state) {
            if ((this.GetDeflateCompleted != null)) {
                InvokeAsyncCompletedEventArgs e = ((InvokeAsyncCompletedEventArgs)(state));
                this.GetDeflateCompleted(this, new GetDeflateCompletedEventArgs(e.Results, e.Error, e.Cancelled, e.UserState));
            }
        }
        
        public void GetDeflateAsync(string query) {
            this.GetDeflateAsync(query, null);
        }
        
        public void GetDeflateAsync(string query, object userState) {
            if ((this.onBeginGetDeflateDelegate == null)) {
                this.onBeginGetDeflateDelegate = new BeginOperationDelegate(this.OnBeginGetDeflate);
            }
            if ((this.onEndGetDeflateDelegate == null)) {
                this.onEndGetDeflateDelegate = new EndOperationDelegate(this.OnEndGetDeflate);
            }
            if ((this.onGetDeflateCompletedDelegate == null)) {
                this.onGetDeflateCompletedDelegate = new System.Threading.SendOrPostCallback(this.OnGetDeflateCompleted);
            }
            base.InvokeAsync(this.onBeginGetDeflateDelegate, new object[] {
                        query}, this.onEndGetDeflateDelegate, this.onGetDeflateCompletedDelegate, userState);
        }
        
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
        System.IAsyncResult Pivot.DBService.DBService.BeginGetGZip(Pivot.DBService.GetGZipRequest request, System.AsyncCallback callback, object asyncState) {
            return base.Channel.BeginGetGZip(request, callback, asyncState);
        }
        
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
        private System.IAsyncResult BeginGetGZip(string query, System.AsyncCallback callback, object asyncState) {
            Pivot.DBService.GetGZipRequest inValue = new Pivot.DBService.GetGZipRequest();
            inValue.Body = new Pivot.DBService.GetGZipRequestBody();
            inValue.Body.query = query;
            return ((Pivot.DBService.DBService)(this)).BeginGetGZip(inValue, callback, asyncState);
        }
        
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
        Pivot.DBService.GetGZipResponse Pivot.DBService.DBService.EndGetGZip(System.IAsyncResult result) {
            return base.Channel.EndGetGZip(result);
        }
        
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
        private string EndGetGZip(System.IAsyncResult result) {
            Pivot.DBService.GetGZipResponse retVal = ((Pivot.DBService.DBService)(this)).EndGetGZip(result);
            return retVal.Body.GetGZipResult;
        }
        
        private System.IAsyncResult OnBeginGetGZip(object[] inValues, System.AsyncCallback callback, object asyncState) {
            string query = ((string)(inValues[0]));
            return this.BeginGetGZip(query, callback, asyncState);
        }
        
        private object[] OnEndGetGZip(System.IAsyncResult result) {
            string retVal = this.EndGetGZip(result);
            return new object[] {
                    retVal};
        }
        
        private void OnGetGZipCompleted(object state) {
            if ((this.GetGZipCompleted != null)) {
                InvokeAsyncCompletedEventArgs e = ((InvokeAsyncCompletedEventArgs)(state));
                this.GetGZipCompleted(this, new GetGZipCompletedEventArgs(e.Results, e.Error, e.Cancelled, e.UserState));
            }
        }
        
        public void GetGZipAsync(string query) {
            this.GetGZipAsync(query, null);
        }
        
        public void GetGZipAsync(string query, object userState) {
            if ((this.onBeginGetGZipDelegate == null)) {
                this.onBeginGetGZipDelegate = new BeginOperationDelegate(this.OnBeginGetGZip);
            }
            if ((this.onEndGetGZipDelegate == null)) {
                this.onEndGetGZipDelegate = new EndOperationDelegate(this.OnEndGetGZip);
            }
            if ((this.onGetGZipCompletedDelegate == null)) {
                this.onGetGZipCompletedDelegate = new System.Threading.SendOrPostCallback(this.OnGetGZipCompleted);
            }
            base.InvokeAsync(this.onBeginGetGZipDelegate, new object[] {
                        query}, this.onEndGetGZipDelegate, this.onGetGZipCompletedDelegate, userState);
        }
        
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
        System.IAsyncResult Pivot.DBService.DBService.BeginGet(Pivot.DBService.GetRequest request, System.AsyncCallback callback, object asyncState) {
            return base.Channel.BeginGet(request, callback, asyncState);
        }
        
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
        private System.IAsyncResult BeginGet(string query, System.AsyncCallback callback, object asyncState) {
            Pivot.DBService.GetRequest inValue = new Pivot.DBService.GetRequest();
            inValue.Body = new Pivot.DBService.GetRequestBody();
            inValue.Body.query = query;
            return ((Pivot.DBService.DBService)(this)).BeginGet(inValue, callback, asyncState);
        }
        
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
        Pivot.DBService.GetResponse Pivot.DBService.DBService.EndGet(System.IAsyncResult result) {
            return base.Channel.EndGet(result);
        }
        
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
        private System.Xml.Linq.XElement EndGet(System.IAsyncResult result) {
            Pivot.DBService.GetResponse retVal = ((Pivot.DBService.DBService)(this)).EndGet(result);
            return retVal.Body.GetResult;
        }
        
        private System.IAsyncResult OnBeginGet(object[] inValues, System.AsyncCallback callback, object asyncState) {
            string query = ((string)(inValues[0]));
            return this.BeginGet(query, callback, asyncState);
        }
        
        private object[] OnEndGet(System.IAsyncResult result) {
            System.Xml.Linq.XElement retVal = this.EndGet(result);
            return new object[] {
                    retVal};
        }
        
        private void OnGetCompleted(object state) {
            if ((this.GetCompleted != null)) {
                InvokeAsyncCompletedEventArgs e = ((InvokeAsyncCompletedEventArgs)(state));
                this.GetCompleted(this, new GetCompletedEventArgs(e.Results, e.Error, e.Cancelled, e.UserState));
            }
        }
        
        public void GetAsync(string query) {
            this.GetAsync(query, null);
        }
        
        public void GetAsync(string query, object userState) {
            if ((this.onBeginGetDelegate == null)) {
                this.onBeginGetDelegate = new BeginOperationDelegate(this.OnBeginGet);
            }
            if ((this.onEndGetDelegate == null)) {
                this.onEndGetDelegate = new EndOperationDelegate(this.OnEndGet);
            }
            if ((this.onGetCompletedDelegate == null)) {
                this.onGetCompletedDelegate = new System.Threading.SendOrPostCallback(this.OnGetCompleted);
            }
            base.InvokeAsync(this.onBeginGetDelegate, new object[] {
                        query}, this.onEndGetDelegate, this.onGetCompletedDelegate, userState);
        }
        
        private System.IAsyncResult OnBeginOpen(object[] inValues, System.AsyncCallback callback, object asyncState) {
            return ((System.ServiceModel.ICommunicationObject)(this)).BeginOpen(callback, asyncState);
        }
        
        private object[] OnEndOpen(System.IAsyncResult result) {
            ((System.ServiceModel.ICommunicationObject)(this)).EndOpen(result);
            return null;
        }
        
        private void OnOpenCompleted(object state) {
            if ((this.OpenCompleted != null)) {
                InvokeAsyncCompletedEventArgs e = ((InvokeAsyncCompletedEventArgs)(state));
                this.OpenCompleted(this, new System.ComponentModel.AsyncCompletedEventArgs(e.Error, e.Cancelled, e.UserState));
            }
        }
        
        public void OpenAsync() {
            this.OpenAsync(null);
        }
        
        public void OpenAsync(object userState) {
            if ((this.onBeginOpenDelegate == null)) {
                this.onBeginOpenDelegate = new BeginOperationDelegate(this.OnBeginOpen);
            }
            if ((this.onEndOpenDelegate == null)) {
                this.onEndOpenDelegate = new EndOperationDelegate(this.OnEndOpen);
            }
            if ((this.onOpenCompletedDelegate == null)) {
                this.onOpenCompletedDelegate = new System.Threading.SendOrPostCallback(this.OnOpenCompleted);
            }
            base.InvokeAsync(this.onBeginOpenDelegate, null, this.onEndOpenDelegate, this.onOpenCompletedDelegate, userState);
        }
        
        private System.IAsyncResult OnBeginClose(object[] inValues, System.AsyncCallback callback, object asyncState) {
            return ((System.ServiceModel.ICommunicationObject)(this)).BeginClose(callback, asyncState);
        }
        
        private object[] OnEndClose(System.IAsyncResult result) {
            ((System.ServiceModel.ICommunicationObject)(this)).EndClose(result);
            return null;
        }
        
        private void OnCloseCompleted(object state) {
            if ((this.CloseCompleted != null)) {
                InvokeAsyncCompletedEventArgs e = ((InvokeAsyncCompletedEventArgs)(state));
                this.CloseCompleted(this, new System.ComponentModel.AsyncCompletedEventArgs(e.Error, e.Cancelled, e.UserState));
            }
        }
        
        public void CloseAsync() {
            this.CloseAsync(null);
        }
        
        public void CloseAsync(object userState) {
            if ((this.onBeginCloseDelegate == null)) {
                this.onBeginCloseDelegate = new BeginOperationDelegate(this.OnBeginClose);
            }
            if ((this.onEndCloseDelegate == null)) {
                this.onEndCloseDelegate = new EndOperationDelegate(this.OnEndClose);
            }
            if ((this.onCloseCompletedDelegate == null)) {
                this.onCloseCompletedDelegate = new System.Threading.SendOrPostCallback(this.OnCloseCompleted);
            }
            base.InvokeAsync(this.onBeginCloseDelegate, null, this.onEndCloseDelegate, this.onCloseCompletedDelegate, userState);
        }
        
        protected override Pivot.DBService.DBService CreateChannel() {
            return new DBServiceClientChannel(this);
        }
        
        private class DBServiceClientChannel : ChannelBase<Pivot.DBService.DBService>, Pivot.DBService.DBService {
            
            public DBServiceClientChannel(System.ServiceModel.ClientBase<Pivot.DBService.DBService> client) : 
                    base(client) {
            }
            
            public System.IAsyncResult BeginGetDeflate(Pivot.DBService.GetDeflateRequest request, System.AsyncCallback callback, object asyncState) {
                object[] _args = new object[1];
                _args[0] = request;
                System.IAsyncResult _result = base.BeginInvoke("GetDeflate", _args, callback, asyncState);
                return _result;
            }
            
            public Pivot.DBService.GetDeflateResponse EndGetDeflate(System.IAsyncResult result) {
                object[] _args = new object[0];
                Pivot.DBService.GetDeflateResponse _result = ((Pivot.DBService.GetDeflateResponse)(base.EndInvoke("GetDeflate", _args, result)));
                return _result;
            }
            
            public System.IAsyncResult BeginGetGZip(Pivot.DBService.GetGZipRequest request, System.AsyncCallback callback, object asyncState) {
                object[] _args = new object[1];
                _args[0] = request;
                System.IAsyncResult _result = base.BeginInvoke("GetGZip", _args, callback, asyncState);
                return _result;
            }
            
            public Pivot.DBService.GetGZipResponse EndGetGZip(System.IAsyncResult result) {
                object[] _args = new object[0];
                Pivot.DBService.GetGZipResponse _result = ((Pivot.DBService.GetGZipResponse)(base.EndInvoke("GetGZip", _args, result)));
                return _result;
            }
            
            public System.IAsyncResult BeginGet(Pivot.DBService.GetRequest request, System.AsyncCallback callback, object asyncState) {
                object[] _args = new object[1];
                _args[0] = request;
                System.IAsyncResult _result = base.BeginInvoke("Get", _args, callback, asyncState);
                return _result;
            }
            
            public Pivot.DBService.GetResponse EndGet(System.IAsyncResult result) {
                object[] _args = new object[0];
                Pivot.DBService.GetResponse _result = ((Pivot.DBService.GetResponse)(base.EndInvoke("Get", _args, result)));
                return _result;
            }
        }
    }
}
﻿//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:2.0.50727.3053
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

// 
// This code was auto-generated by Microsoft.Silverlight.ServiceReference, version 2.0.5.0
// 
namespace PicaPages.DBService {
    
    
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")]
    [System.ServiceModel.ServiceContractAttribute(Namespace="urn:SerializationTest", ConfigurationName="DBService.IDBService")]
    public interface IDBService {
        
        [System.ServiceModel.OperationContractAttribute(AsyncPattern=true, Action="urn:SerializationTest/IDBService/GetDataSet", ReplyAction="urn:SerializationTest/IDBService/GetDataSetResponse")]
        System.IAsyncResult BeginGetDataSet(PicaPages.DBService.GetDataSetRequest request, System.AsyncCallback callback, object asyncState);
        
        PicaPages.DBService.GetDataSetResponse EndGetDataSet(System.IAsyncResult result);
        
        [System.ServiceModel.OperationContractAttribute(AsyncPattern=true, Action="urn:SerializationTest/IDBService/GetOleDbDataSet", ReplyAction="urn:SerializationTest/IDBService/GetOleDbDataSetResponse")]
        System.IAsyncResult BeginGetOleDbDataSet(PicaPages.DBService.GetOleDbDataSetRequest request, System.AsyncCallback callback, object asyncState);
        
        PicaPages.DBService.GetOleDbDataSetResponse EndGetOleDbDataSet(System.IAsyncResult result);
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")]
    [System.ServiceModel.MessageContractAttribute(IsWrapped=false)]
    public partial class GetDataSetRequest {
        
        [System.ServiceModel.MessageBodyMemberAttribute(Name="GetDataSet", Namespace="urn:SerializationTest", Order=0)]
        public PicaPages.DBService.GetDataSetRequestBody Body;
        
        public GetDataSetRequest() {
        }
        
        public GetDataSetRequest(PicaPages.DBService.GetDataSetRequestBody Body) {
            this.Body = Body;
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")]
    [System.Runtime.Serialization.DataContractAttribute(Namespace="urn:SerializationTest")]
    public partial class GetDataSetRequestBody {
        
        [System.Runtime.Serialization.DataMemberAttribute(EmitDefaultValue=false, Order=0)]
        public string query;
        
        public GetDataSetRequestBody() {
        }
        
        public GetDataSetRequestBody(string query) {
            this.query = query;
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")]
    [System.ServiceModel.MessageContractAttribute(IsWrapped=false)]
    public partial class GetDataSetResponse {
        
        [System.ServiceModel.MessageBodyMemberAttribute(Name="GetDataSetResponse", Namespace="urn:SerializationTest", Order=0)]
        public PicaPages.DBService.GetDataSetResponseBody Body;
        
        public GetDataSetResponse() {
        }
        
        public GetDataSetResponse(PicaPages.DBService.GetDataSetResponseBody Body) {
            this.Body = Body;
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")]
    [System.Runtime.Serialization.DataContractAttribute(Namespace="urn:SerializationTest")]
    public partial class GetDataSetResponseBody {
        
        [System.Runtime.Serialization.DataMemberAttribute(EmitDefaultValue=false, Order=0)]
        public System.Xml.Linq.XElement GetDataSetResult;
        
        public GetDataSetResponseBody() {
        }
        
        public GetDataSetResponseBody(System.Xml.Linq.XElement GetDataSetResult) {
            this.GetDataSetResult = GetDataSetResult;
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")]
    [System.ServiceModel.MessageContractAttribute(IsWrapped=false)]
    public partial class GetOleDbDataSetRequest {
        
        [System.ServiceModel.MessageBodyMemberAttribute(Name="GetOleDbDataSet", Namespace="urn:SerializationTest", Order=0)]
        public PicaPages.DBService.GetOleDbDataSetRequestBody Body;
        
        public GetOleDbDataSetRequest() {
        }
        
        public GetOleDbDataSetRequest(PicaPages.DBService.GetOleDbDataSetRequestBody Body) {
            this.Body = Body;
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")]
    [System.Runtime.Serialization.DataContractAttribute(Namespace="urn:SerializationTest")]
    public partial class GetOleDbDataSetRequestBody {
        
        [System.Runtime.Serialization.DataMemberAttribute(EmitDefaultValue=false, Order=0)]
        public string query;
        
        public GetOleDbDataSetRequestBody() {
        }
        
        public GetOleDbDataSetRequestBody(string query) {
            this.query = query;
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")]
    [System.ServiceModel.MessageContractAttribute(IsWrapped=false)]
    public partial class GetOleDbDataSetResponse {
        
        [System.ServiceModel.MessageBodyMemberAttribute(Name="GetOleDbDataSetResponse", Namespace="urn:SerializationTest", Order=0)]
        public PicaPages.DBService.GetOleDbDataSetResponseBody Body;
        
        public GetOleDbDataSetResponse() {
        }
        
        public GetOleDbDataSetResponse(PicaPages.DBService.GetOleDbDataSetResponseBody Body) {
            this.Body = Body;
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")]
    [System.Runtime.Serialization.DataContractAttribute(Namespace="urn:SerializationTest")]
    public partial class GetOleDbDataSetResponseBody {
        
        [System.Runtime.Serialization.DataMemberAttribute(EmitDefaultValue=false, Order=0)]
        public System.Xml.Linq.XElement GetOleDbDataSetResult;
        
        public GetOleDbDataSetResponseBody() {
        }
        
        public GetOleDbDataSetResponseBody(System.Xml.Linq.XElement GetOleDbDataSetResult) {
            this.GetOleDbDataSetResult = GetOleDbDataSetResult;
        }
    }
    
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")]
    public interface IDBServiceChannel : PicaPages.DBService.IDBService, System.ServiceModel.IClientChannel {
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")]
    public partial class GetDataSetCompletedEventArgs : System.ComponentModel.AsyncCompletedEventArgs {
        
        private object[] results;
        
        public GetDataSetCompletedEventArgs(object[] results, System.Exception exception, bool cancelled, object userState) : 
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
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")]
    public partial class GetOleDbDataSetCompletedEventArgs : System.ComponentModel.AsyncCompletedEventArgs {
        
        private object[] results;
        
        public GetOleDbDataSetCompletedEventArgs(object[] results, System.Exception exception, bool cancelled, object userState) : 
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
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")]
    public partial class DBServiceClient : System.ServiceModel.ClientBase<PicaPages.DBService.IDBService>, PicaPages.DBService.IDBService {
        
        private BeginOperationDelegate onBeginGetDataSetDelegate;
        
        private EndOperationDelegate onEndGetDataSetDelegate;
        
        private System.Threading.SendOrPostCallback onGetDataSetCompletedDelegate;
        
        private BeginOperationDelegate onBeginGetOleDbDataSetDelegate;
        
        private EndOperationDelegate onEndGetOleDbDataSetDelegate;
        
        private System.Threading.SendOrPostCallback onGetOleDbDataSetCompletedDelegate;
        
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
        
        public event System.EventHandler<GetDataSetCompletedEventArgs> GetDataSetCompleted;
        
        public event System.EventHandler<GetOleDbDataSetCompletedEventArgs> GetOleDbDataSetCompleted;
        
        public event System.EventHandler<System.ComponentModel.AsyncCompletedEventArgs> OpenCompleted;
        
        public event System.EventHandler<System.ComponentModel.AsyncCompletedEventArgs> CloseCompleted;
        
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
        System.IAsyncResult PicaPages.DBService.IDBService.BeginGetDataSet(PicaPages.DBService.GetDataSetRequest request, System.AsyncCallback callback, object asyncState) {
            return base.Channel.BeginGetDataSet(request, callback, asyncState);
        }
        
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
        private System.IAsyncResult BeginGetDataSet(string query, System.AsyncCallback callback, object asyncState) {
            PicaPages.DBService.GetDataSetRequest inValue = new PicaPages.DBService.GetDataSetRequest();
            inValue.Body = new PicaPages.DBService.GetDataSetRequestBody();
            inValue.Body.query = query;
            return ((PicaPages.DBService.IDBService)(this)).BeginGetDataSet(inValue, callback, asyncState);
        }
        
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
        PicaPages.DBService.GetDataSetResponse PicaPages.DBService.IDBService.EndGetDataSet(System.IAsyncResult result) {
            return base.Channel.EndGetDataSet(result);
        }
        
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
        private System.Xml.Linq.XElement EndGetDataSet(System.IAsyncResult result) {
            PicaPages.DBService.GetDataSetResponse retVal = ((PicaPages.DBService.IDBService)(this)).EndGetDataSet(result);
            return retVal.Body.GetDataSetResult;
        }
        
        private System.IAsyncResult OnBeginGetDataSet(object[] inValues, System.AsyncCallback callback, object asyncState) {
            string query = ((string)(inValues[0]));
            return this.BeginGetDataSet(query, callback, asyncState);
        }
        
        private object[] OnEndGetDataSet(System.IAsyncResult result) {
            System.Xml.Linq.XElement retVal = this.EndGetDataSet(result);
            return new object[] {
                    retVal};
        }
        
        private void OnGetDataSetCompleted(object state) {
            if ((this.GetDataSetCompleted != null)) {
                InvokeAsyncCompletedEventArgs e = ((InvokeAsyncCompletedEventArgs)(state));
                this.GetDataSetCompleted(this, new GetDataSetCompletedEventArgs(e.Results, e.Error, e.Cancelled, e.UserState));
            }
        }
        
        public void GetDataSetAsync(string query) {
            this.GetDataSetAsync(query, null);
        }
        
        public void GetDataSetAsync(string query, object userState) {
            if ((this.onBeginGetDataSetDelegate == null)) {
                this.onBeginGetDataSetDelegate = new BeginOperationDelegate(this.OnBeginGetDataSet);
            }
            if ((this.onEndGetDataSetDelegate == null)) {
                this.onEndGetDataSetDelegate = new EndOperationDelegate(this.OnEndGetDataSet);
            }
            if ((this.onGetDataSetCompletedDelegate == null)) {
                this.onGetDataSetCompletedDelegate = new System.Threading.SendOrPostCallback(this.OnGetDataSetCompleted);
            }
            base.InvokeAsync(this.onBeginGetDataSetDelegate, new object[] {
                        query}, this.onEndGetDataSetDelegate, this.onGetDataSetCompletedDelegate, userState);
        }
        
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
        System.IAsyncResult PicaPages.DBService.IDBService.BeginGetOleDbDataSet(PicaPages.DBService.GetOleDbDataSetRequest request, System.AsyncCallback callback, object asyncState) {
            return base.Channel.BeginGetOleDbDataSet(request, callback, asyncState);
        }
        
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
        private System.IAsyncResult BeginGetOleDbDataSet(string query, System.AsyncCallback callback, object asyncState) {
            PicaPages.DBService.GetOleDbDataSetRequest inValue = new PicaPages.DBService.GetOleDbDataSetRequest();
            inValue.Body = new PicaPages.DBService.GetOleDbDataSetRequestBody();
            inValue.Body.query = query;
            return ((PicaPages.DBService.IDBService)(this)).BeginGetOleDbDataSet(inValue, callback, asyncState);
        }
        
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
        PicaPages.DBService.GetOleDbDataSetResponse PicaPages.DBService.IDBService.EndGetOleDbDataSet(System.IAsyncResult result) {
            return base.Channel.EndGetOleDbDataSet(result);
        }
        
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
        private System.Xml.Linq.XElement EndGetOleDbDataSet(System.IAsyncResult result) {
            PicaPages.DBService.GetOleDbDataSetResponse retVal = ((PicaPages.DBService.IDBService)(this)).EndGetOleDbDataSet(result);
            return retVal.Body.GetOleDbDataSetResult;
        }
        
        private System.IAsyncResult OnBeginGetOleDbDataSet(object[] inValues, System.AsyncCallback callback, object asyncState) {
            string query = ((string)(inValues[0]));
            return this.BeginGetOleDbDataSet(query, callback, asyncState);
        }
        
        private object[] OnEndGetOleDbDataSet(System.IAsyncResult result) {
            System.Xml.Linq.XElement retVal = this.EndGetOleDbDataSet(result);
            return new object[] {
                    retVal};
        }
        
        private void OnGetOleDbDataSetCompleted(object state) {
            if ((this.GetOleDbDataSetCompleted != null)) {
                InvokeAsyncCompletedEventArgs e = ((InvokeAsyncCompletedEventArgs)(state));
                this.GetOleDbDataSetCompleted(this, new GetOleDbDataSetCompletedEventArgs(e.Results, e.Error, e.Cancelled, e.UserState));
            }
        }
        
        public void GetOleDbDataSetAsync(string query) {
            this.GetOleDbDataSetAsync(query, null);
        }
        
        public void GetOleDbDataSetAsync(string query, object userState) {
            if ((this.onBeginGetOleDbDataSetDelegate == null)) {
                this.onBeginGetOleDbDataSetDelegate = new BeginOperationDelegate(this.OnBeginGetOleDbDataSet);
            }
            if ((this.onEndGetOleDbDataSetDelegate == null)) {
                this.onEndGetOleDbDataSetDelegate = new EndOperationDelegate(this.OnEndGetOleDbDataSet);
            }
            if ((this.onGetOleDbDataSetCompletedDelegate == null)) {
                this.onGetOleDbDataSetCompletedDelegate = new System.Threading.SendOrPostCallback(this.OnGetOleDbDataSetCompleted);
            }
            base.InvokeAsync(this.onBeginGetOleDbDataSetDelegate, new object[] {
                        query}, this.onEndGetOleDbDataSetDelegate, this.onGetOleDbDataSetCompletedDelegate, userState);
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
        
        protected override PicaPages.DBService.IDBService CreateChannel() {
            return new DBServiceClientChannel(this);
        }
        
        private class DBServiceClientChannel : ChannelBase<PicaPages.DBService.IDBService>, PicaPages.DBService.IDBService {
            
            public DBServiceClientChannel(System.ServiceModel.ClientBase<PicaPages.DBService.IDBService> client) : 
                    base(client) {
            }
            
            public System.IAsyncResult BeginGetDataSet(PicaPages.DBService.GetDataSetRequest request, System.AsyncCallback callback, object asyncState) {
                object[] _args = new object[1];
                _args[0] = request;
                System.IAsyncResult _result = base.BeginInvoke("GetDataSet", _args, callback, asyncState);
                return _result;
            }
            
            public PicaPages.DBService.GetDataSetResponse EndGetDataSet(System.IAsyncResult result) {
                object[] _args = new object[0];
                PicaPages.DBService.GetDataSetResponse _result = ((PicaPages.DBService.GetDataSetResponse)(base.EndInvoke("GetDataSet", _args, result)));
                return _result;
            }
            
            public System.IAsyncResult BeginGetOleDbDataSet(PicaPages.DBService.GetOleDbDataSetRequest request, System.AsyncCallback callback, object asyncState) {
                object[] _args = new object[1];
                _args[0] = request;
                System.IAsyncResult _result = base.BeginInvoke("GetOleDbDataSet", _args, callback, asyncState);
                return _result;
            }
            
            public PicaPages.DBService.GetOleDbDataSetResponse EndGetOleDbDataSet(System.IAsyncResult result) {
                object[] _args = new object[0];
                PicaPages.DBService.GetOleDbDataSetResponse _result = ((PicaPages.DBService.GetOleDbDataSetResponse)(base.EndInvoke("GetOleDbDataSet", _args, result)));
                return _result;
            }
        }
    }
}
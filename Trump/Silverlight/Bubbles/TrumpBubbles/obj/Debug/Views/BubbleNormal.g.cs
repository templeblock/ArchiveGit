﻿#pragma checksum "C:\My Projects\Trump\ttnweb_trunk\Silverlight\Bubbles\TrumpBubbles\Views\BubbleNormal.xaml" "{406ea660-64cf-4c82-b6f0-42d48172a799}" "29E3516CD21EB3FB824AC997F4AB0D92"
//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:2.0.50727.4927
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

using System;
using System.Windows;
using System.Windows.Automation;
using System.Windows.Automation.Peers;
using System.Windows.Automation.Provider;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Markup;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Media.Imaging;
using System.Windows.Resources;
using System.Windows.Shapes;
using System.Windows.Threading;


namespace TrumpBubbles {
    
    
    public partial class BubbleNormal : System.Windows.Controls.UserControl {
        
        internal System.Windows.Controls.Grid x_RootLayout;
        
        internal System.Windows.VisualStateGroup CommonStates;
        
        internal System.Windows.VisualState Shown;
        
        internal System.Windows.VisualState NotShown;
        
        internal System.Windows.Controls.StackPanel x_Header;
        
        internal System.Windows.Controls.Image x_Icon;
        
        internal System.Windows.Controls.TextBlock x_OpenText;
        
        internal System.Windows.Controls.Image x_OpenImage;
        
        internal System.Windows.Controls.TextBlock x_Title;
        
        internal System.Windows.Controls.ContentControl x_ItemContent;
        
        internal System.Windows.Controls.StackPanel x_CallToActionSP;
        
        internal System.Windows.Controls.Image x_CallToActionBtn;
        
        internal System.Windows.Controls.TextBlock x_CallToActionText;
        
        private bool _contentLoaded;
        
        /// <summary>
        /// InitializeComponent
        /// </summary>
        [System.Diagnostics.DebuggerNonUserCodeAttribute()]
        public void InitializeComponent() {
            if (_contentLoaded) {
                return;
            }
            _contentLoaded = true;
            System.Windows.Application.LoadComponent(this, new System.Uri("/TrumpBubbles;component/Views/BubbleNormal.xaml", System.UriKind.Relative));
            this.x_RootLayout = ((System.Windows.Controls.Grid)(this.FindName("x_RootLayout")));
            this.CommonStates = ((System.Windows.VisualStateGroup)(this.FindName("CommonStates")));
            this.Shown = ((System.Windows.VisualState)(this.FindName("Shown")));
            this.NotShown = ((System.Windows.VisualState)(this.FindName("NotShown")));
            this.x_Header = ((System.Windows.Controls.StackPanel)(this.FindName("x_Header")));
            this.x_Icon = ((System.Windows.Controls.Image)(this.FindName("x_Icon")));
            this.x_OpenText = ((System.Windows.Controls.TextBlock)(this.FindName("x_OpenText")));
            this.x_OpenImage = ((System.Windows.Controls.Image)(this.FindName("x_OpenImage")));
            this.x_Title = ((System.Windows.Controls.TextBlock)(this.FindName("x_Title")));
            this.x_ItemContent = ((System.Windows.Controls.ContentControl)(this.FindName("x_ItemContent")));
            this.x_CallToActionSP = ((System.Windows.Controls.StackPanel)(this.FindName("x_CallToActionSP")));
            this.x_CallToActionBtn = ((System.Windows.Controls.Image)(this.FindName("x_CallToActionBtn")));
            this.x_CallToActionText = ((System.Windows.Controls.TextBlock)(this.FindName("x_CallToActionText")));
        }
    }
}

<?xml version="1.0" encoding="UTF-8"?>
<%@ Page Language="C#" EnableViewState="false" AutoEventWireup="true" CodeBehind="Sitemap.aspx.cs" Inherits="Money.Web.Sitemap" %>
<%@ Register TagPrefix="asp" Namespace="System.Web.UI.WebControls" Assembly="System.Web" %> 
<%@ Register TagPrefix="ria" Namespace="System.Web.DomainServices.WebControls" Assembly="System.Web.DomainServices.WebControls" %> 
<%@ OutputCache CacheProfile="SitemapPage" %>
<%--

     Define your DataSource.
     Be sure to update the DomainServiceTypeName below

--%>
<ria:DomainDataSource runat="server"
    ID="ProductsDomainDataSource" 
    DomainServiceTypeName="SilverlightStore.ProductsDomainService" 
    SelectMethod="GetProducts" />
<%--

    Render sitemap XML as per http://www.sitemaps.org specification.

--%>
<asp:Repeater runat="server" DataSourceID="ProductsDomainDataSource">
    <HeaderTemplate>
        <urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">
    </HeaderTemplate>
    <ItemTemplate>
        <url><%--

                 Update the <loc> and related values to expose deep-links 
                 to search engine crawlers.                               
                                                                          
                 In the example below, a collection of product detail     
                 deep-links are generated to ensure crawlers visit each   
                 product using the metadata (<lastmod>, <priority>, etc)  
                 defined from the DataSource.
            
            --%>
            <loc><%= new Uri(this.Request.Url, this.ResolveUrl("Product.aspx")).ToString() %>?Name=<%# HttpUtility.UrlEncode((string)Eval("Name")) %></loc>
            <lastmod><%# Eval("LastModified")%></lastmod>
            <changefreq><%# Eval("ChangeFrequency")%></changefreq>
            <priority><%# Eval("Priority")%></priority>
        </url>
    </ItemTemplate>
    <FooterTemplate>
        </urlset>
    </FooterTemplate>
</asp:Repeater>

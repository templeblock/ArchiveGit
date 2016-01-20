// XMLhttpRequest stuff
var aXmlHttp = new Array();
var aXmlResponse = new Array();
function xmlResult()
{
    for(var i=0;i<aXmlHttp.length;i++)
    {
        if(aXmlHttp[i] && aXmlHttp[i][0] && aXmlHttp[i][0].readyState==4&&aXmlHttp[i][0].responseText)
        {
            //must null out record before calling function in case
            //function invokes another xmlHttpRequest.
            var f = aXmlHttp[i][1];
            var s = aXmlHttp[i][0].responseText;
            aXmlHttp[i][0] = null;
            aXmlHttp[i][1] = null;
            aXmlHttp[i] = null;
            f(s);
        }
    }
}

// u -> url
// f -> callback
function call(u,f)
{
    var idx = aXmlHttp.length;
    for(var i=0; i<idx;i++)
    if (aXmlHttp[i] == null)
    {
        idx = i;
        break;
    }
    aXmlHttp[idx]=new Array(2);
    aXmlHttp[idx][0] = getXMLHTTP();
    aXmlHttp[idx][1] = f;
    if(aXmlHttp[idx])
    {
        aXmlHttp[idx][0].open("GET",u,true);
        aXmlHttp[idx][0].onreadystatechange=xmlResult;
        aXmlHttp[idx][0].send(null);
    }
}

function getXMLHTTP()
{
    var A=null;
    try
    {
        A=new ActiveXObject("Msxml2.XMLHTTP")
    }
    catch(e)
    {
        try
        {
            A=new ActiveXObject("Microsoft.XMLHTTP")
        }
        catch(oc)
        {
            A=null
        }
    }
    if(!A && typeof XMLHttpRequest != "undefined")
    {
        A=new XMLHttpRequest()
    }
    return A
}

function drawNull(s)
{
    eval(s);
    return false;
}
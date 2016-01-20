<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
#define IMAGE

<%@ Page Language="C#" %>
<%@ Import Namespace="System" %>
<%@ Import Namespace="System.Configuration" %>
<%@ Import Namespace="System.Data" %>
<%@ Import Namespace="System.Data.Odbc" %>
<%@ Import Namespace="System.Web" %>
<%@ Import Namespace="System.Web.UI" %>
<%@ Import Namespace="System.Web.UI.WebControls" %>
<%@ Import Namespace="System.Web.UI.HtmlControls" %>
<%@ Import Namespace="System.Web.Security" %>
<%@ Import Namespace="System.Drawing" %>
<%@ Import Namespace="System.Drawing.Drawing2D" %>
<%@ Import Namespace="System.Drawing.Imaging" %>

<html><head>
<script type="text/javascript" runat="server">

//class Tile : Page
//{
	protected void Page_Load(object sender, EventArgs e)
	{
		int documentWidth = 1969;
		int documentHeight = 1585;

		// Left-top is in document coordinates - independant of zoom
		int tileLeft = Convert.ToInt32(Request.QueryString["l"]);
		int tileTop    = Convert.ToInt32(Request.QueryString["t"]);

		// Width-height is in screen pixels - used to clip the returned image
		int tileWidth = Convert.ToInt32(Request.QueryString["w"]);
		int tileHeight = Convert.ToInt32(Request.QueryString["h"]);

		double xZoom   = Convert.ToDouble(Request.QueryString["z"]);
		double yZoom   = xZoom;

		if (tileWidth <= 0)
			tileWidth = documentWidth;

		if (tileHeight <= 0)
			tileHeight = documentHeight;

		// No zoom means scale to fit
		if (xZoom == 0.0)
			xZoom = (double)tileWidth / documentWidth;
		if (yZoom == 0.0)
			yZoom = (double)tileHeight / documentHeight;

		Bitmap objTile = new Bitmap(tileWidth, tileHeight);
		Graphics objGraphic = Graphics.FromImage(objTile);
		ImageFormat imageFormat;
#if !IMAGE
//j		Bitmap oImgSource = new Bitmap(Server.MapPath("images\\sample.jpg"));
		System.Drawing.Image oImgSource = System.Drawing.Image.FromFile(Server.MapPath("images\\sample.jpg"));

		double dx = (double)tileWidth / xZoom;
		double dy = (double)tileHeight / yZoom;
		RectangleF dstRect = new RectangleF(0, 0, tileWidth, tileHeight);
		RectangleF srcRect = new RectangleF(tileLeft, tileTop, (float)dx, (float)dy);

		objGraphic.DrawImage(oImgSource, dstRect, srcRect, GraphicsUnit.Pixel);
		oImgSource.Dispose();

		imageFormat = ImageFormat.Jpeg;
		Response.ContentType = "image/jpeg";
#else
		int barValue1 = 20;
		int barValue2 = 20;
		int barValue3 = 20;

		int barValueMax = Math.Max(barValue1, Math.Max(barValue2, barValue3));
		if (barValueMax == 0)
			barValueMax = 1;

		int maxBarHeight = documentHeight - 10;
		int barHeight1 = (barValue1 * maxBarHeight) / barValueMax;
		int barHeight2 = (barValue2 * maxBarHeight) / barValueMax;
		int barHeight3 = (barValue3 * maxBarHeight) / barValueMax;

		Brush redBrush = new SolidBrush(Color.Red);
		Brush blueBrush = new SolidBrush(Color.Blue);
		Brush greenBrush = new SolidBrush(Color.Green);
		Brush whiteBrush = new SolidBrush(Color.White);
		Pen blackPen = new Pen(Color.Black, 2);

		objGraphic.FillRectangle(whiteBrush, 0, 0, tileWidth, tileHeight);
		Matrix matrix = objGraphic.Transform;
		matrix.Scale((float)xZoom, (float)yZoom);
		matrix.Translate(-tileLeft, -tileTop);
		objGraphic.Transform = matrix;
		objGraphic.DrawLine(blackPen, new Point(5, 5), new Point(5, documentHeight - 5));
		objGraphic.DrawLine(blackPen, new Point(5, documentHeight - 5), new Point(documentWidth - 5, documentHeight - 5));
		objGraphic.FillRectangle(redBrush, (documentWidth * 10) / 200, (documentHeight - 6) - barHeight1, documentWidth / 4, barHeight1);
		objGraphic.FillRectangle(blueBrush, (documentWidth * 70) / 200, (documentHeight - 6) - barHeight2, documentWidth / 4, barHeight2);
		objGraphic.FillRectangle(greenBrush, (documentWidth * 130) / 200, (documentHeight - 6) - barHeight3, documentWidth / 4, barHeight3);

		imageFormat = ImageFormat.Gif;
		Response.ContentType = "image/gif";
#endif

		objTile.Save(Response.OutputStream, imageFormat);
		objTile.Dispose();
	}
//}
</script>
</head></html>

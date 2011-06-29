<%@page language="java" import="
         java.io.*, 
         java.net.*, 
         java.awt.*, 
         java.awt.image.*, 
         javax.swing.*, 
		 com.bepco.rtc.playback.*,
		 com.bepco.rtc.gui.*,
		 com.bepco.io.*,
		 java.util.*,
		 java.util.List,
     com.sun.image.codec.jpeg.*,
     com.atomicobject.gui.ColorLoader" %>

<%!
static String pageName = "brakeforce.jsp";
static String pageTitle = "Drive Curve";

static String placeholder = "/local/httproot/imgstore/DO_NOT_REMOVE";
static String imgFileName = "bfgraph.jpg";

/** Tell the web browser to download: */
//static String webImage = "/imgstore/bfgraph.jpg";
static String webImage = "/imgstore/prototype_bfgraph.jpg";
%>

<%
List errors = new ArrayList();

/*
	// UNCOMMENT ME AND MAKE ME WORK!!!!

ColorLoader.loadColors("com.bepco.rtc.properties.colors");
// Create the Brake Force Panel, add it's components.
URL brakeURL = new URL(
		"ftp",
		InetAddress.getLocalHost().getHostAddress(),
		BEPCommProtocol.FTPD_PORT,
		Controller.BRAKEFORCE_PATH);
		
BrakeForcePanel brakeforcePanel = new BrakeForcePanel(brakeURL);
brakeforcePanel.loadData();
brakeforcePanel.setVisible(true);
brakeforcePanel.setSize(640,480);

//JPanel panel = new JPanel();
//panel.add(brakeforcePanel);
//panel.setVisible(true);
//panel.setSize(640,480);


// Create  an image to paint into
BufferedImage crtImage =
  new BufferedImage(640, 480,BufferedImage.TYPE_INT_RGB);

// Print into the image
Graphics g = crtImage.getGraphics();
brakeforcePanel.print(g);
//panel.print(g);

//JLabel label = new JLabel("Hello World!");
//label.setBackground(Color.blue);
//label.setSize(100,50);
//label.paint(g);

//g.setColor(Color.white);
//g.fillRect(10,40,50,50);



// Get anchorage into the file system
URL holder = getClass().getResource(placeholder);
// Determine destination dir of the JPEG.
File dir = (new File(URLDecoder.decode(holder.getFile()))).getParentFile();
// Write to file.
File outfile = new File(dir, imgFileName);
try {
	FileOutputStream output = new FileOutputStream(outfile);
	JPEGImageEncoder encoder = JPEGCodec.createJPEGEncoder(output);
	encoder.encode(crtImage);
	output.close();
} catch (IOException ex) {
	errors.add("Failed to write image: " + ex.getMessage());
}
*/
%>

<html>
<head>
 <jsp:include page="header.jsp" flush="true">
    <jsp:param name="title" value="<%=pageTitle%>"/>
  </jsp:include> 
 <style>
 
 
 
 </style>
</head>

<body bgcolor="white">
<jsp:include page="navbar.jsp" flush="true">
 <jsp:param name="mode" value="bar"/>
</jsp:include>
<center><h2><%=pageTitle%></h2></center>

<center><img src="<%= webImage %>"></center>


</body>
</html>

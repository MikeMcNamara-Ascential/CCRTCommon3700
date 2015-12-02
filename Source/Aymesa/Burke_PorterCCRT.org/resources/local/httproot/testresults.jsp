<%@page language="java" 
  import="com.bepco.rtc.report.*,
          java.util.*,
          java.net.*,
          java.io.*,
          javax.xml.parsers.*,
          javax.xml.transform.*,
          org.w3c.dom.*,
          org.xml.sax.*,
          com.atomicobject.resource.*,
          com.bepco.rtc.playback.*,
		  com.bepco.rtc.Main,
		  com.bepco.rtc.report.*,
		  com.bepco.data.*
		  " %>
<%@include file="nocache.jsp"%>

<%!
static String pageName="testresults.jsp";
static String pageTitle="Test Results";
static String RESULT_TAG = "TestResultToPrint";
static String RESULTS_PATH = "/home/ftp";
static String REPORT_CONFIG_FILE = "/local/httproot/conf/ipaq.xml";


%>

<%
List errors = new LinkedList();

String resultFile = "";

try {
  // Connect  to broker
  ServletDataBroker broker = ServletDataBroker.getBroker();
  // Get the result file 
  if (broker.containsTag(RESULT_TAG)) {
    resultFile = RESULTS_PATH + broker.getString(RESULT_TAG);
		
	if (resultFile == null) {
	  errors.add("Could not retrieve value for " + RESULT_TAG);
	}
  } else {
    errors.add("Could not find "+RESULT_TAG+" tag");
  }

} catch (IOException ex) {
  errors.add("Creating ServletDataBroker: " + ex.getMessage());
}
 


%>

<html>

<head>
 <jsp:include page="header.jsp" flush="true">
    <jsp:param name="title" value="<%=pageTitle%>"/>
  </jsp:include> 
</head>

<body bgcolor="white">
<jsp:include page="navbar.jsp" flush="true">
 <jsp:param name="mode" value="bar"/>
</jsp:include>
 <%
   doTestReport(resultFile, out, errors);
 %>

<%@include file="errorbox.jsp"%>

<jsp:include page="navbar.jsp" flush="true">
 <jsp:param name="mode" value="bar"/>
</jsp:include>

</body>
</html>

<!-- Test report output function -->
<%!
static void doTestReport(String pathname, JspWriter out, List errors) {
		// FTP url based on this name and load it.
		Document testDocument = null;
		ReportEngine repEngine = null;
		URL resultURL = null;
		
		Document reportPrefs = null;
		try {
			String urlString = "ftp://burke:porter@" +
					InetAddress.getLocalHost().getHostAddress() + ":" +
					com.bepco.io.BEPCommProtocol.FTPD_PORT +
					pathname;
			resultURL = new URL(urlString);

//			resultURL = new URL("ftp",
//					InetAddress.getLocalHost().getHostAddress(),
//					com.bepco.io.BEPCommProtocol.FTPD_PORT,
//					pathname);
			Reader xmlSource = new InputStreamReader(resultURL.openStream());
			// Parse XML 
			testDocument = com.atomicobject.util.XmlUtil.getXMLDocument(
					xmlSource, false);
			
			reportPrefs = ResourceLoader.getResourceLoader(
					Main.class).getXMLDocument(REPORT_CONFIG_FILE, false);
		} catch (MalformedURLException ex) {
			errors.add("URL for " + pathname + " malformed");
			return;
			
		} catch (IOException ex) {
			errors.add("Could not load " + resultURL + ": " + ex.getMessage());
			return;
			
		} catch (SAXException ex) {
			String mesg = "SAX parser error in file ["+pathname+"]";
			if (ex instanceof SAXParseException) {
				mesg += ", line=" + ((SAXParseException)ex).getLineNumber();
			}
			errors.add(mesg);
			return;
			
		} catch (ParserConfigurationException ex) {
			errors.add("XML subsystem error on file ["
					+pathname +"]: " + ex.getMessage());
			return;
		} catch (ResourceException ex) {
			errors.add("Trouble accessing ["
					+pathname +"]: " + ex.getMessage());
		}

		if(testDocument == null) {
			errors.add("Did not get results document ["+pathname+"]via FTP");
			return;
		}

		try {
			// Build the report engine
			repEngine = new ReportEngine(UnitsConverter.getUnitSystem());

			// Generate long report
			Document reportDocument = repEngine.generateReport(
					reportPrefs.getDocumentElement(),
					testDocument);
			
			// Use GUIPrintDriver to generate suitable HTML output
			// and write it to the page
			GUIPrintDriver guiPrinter = new GUIPrintDriver();
			guiPrinter.print(reportDocument, out);
		
				
		} catch (PrintDriverException ex) {
			errors.add(ex.getMessage());
			
		} catch (ConfigurationException ex) {
			errors.add(ex.getMessage());

		} catch (TransformerException ex) {
			errors.add(ex.getMessage());
		}
		
}
%>

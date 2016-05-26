<%@page language="java" import="com.bepco.rtc.report.*,
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
static String pageName="streaming.jsp";
static String pageTitle="Live Data";
%>


<%
List errors = new ArrayList();
// Connect to broker
ServletDataBroker broker = null;
try {
  // Connect  to broker
  broker = ServletDataBroker.getBroker();
 
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



<body>
<jsp:include page="navbar.jsp" flush="true">
 <jsp:param name="mode" value="bar"/>
</jsp:include>
<h2><%=pageTitle%></h2>

<table align="center"  border="1">
<tr><td>LF</td><td>RF</td></tr>
<tr>
	<td><%= broker.getString("LFSpeed") %></td>
    <td><%= broker.getString("RFSpeed") %></td>
</tr>
<tr><td>LR</td><td>RR</td></tr>
<tr>
	<td><%= broker.getString("LRSpeed") %></td>
    <td><%= broker.getString("RRSpeed") %></td>
</tr>
</table>




</body>
</html>
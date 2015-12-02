<%@page language="java"
 import="com.bepco.rtc.playback.ServletDataBroker, java.util.*"%>

<%@include file="nocache.jsp"%>

<%!
static String pageTitle  = "Broker Test";
static String pageName  = "brokertest.jsp";
%>

<%
List errors = new LinkedList();

// Connect to the data broker
ServletDataBroker broker = null;
try {
  broker = ServletDataBroker.getBroker();
} catch (IOException ex) {
  errors.add("Creating ServletDataBroker: " + ex.getMessage());
}

// See if we are to refresh the broker
if ("true".equals(request.getParameter("refresh"))) {
  if (broker != null) {
    try { 
	  broker.reloadModels();
    } catch (IOException ex) {
	  errors.add("Refreshing broker: " + ex.getMessage());
    }
  } else {
    errors.add("Broker reference is null -- cannot refresh it!");
  }
}
%>

<html>
<head>
<jsp:include page="header.jsp" flush="true">
  <jsp:param name="title" value="<%=pageTitle%>"/>
</jsp:include>

</head>

<body>
<jsp:include page="navbar.jsp" flush="true"/>
<h2><%=pageTitle%></h2>
 
<% 
 if (broker != null) { 

  // List supported tags
  out.println("<b>Broker connected.</b>");
  if (request.getParameter("listtags") != null) {
    Iterator it = broker.modelMap.keySet().iterator();
    out.println("<ul>");
    while (it.hasNext()) {
      out.println("<li>" + it.next() + "</li>");  
    }
    out.println("</ul>");
	out.println("[<a href='"+pageName+"'>Hide</a>]");
  } else {
	out.println("[<a href='"+pageName+"?listtags=on'>Expand</a>]");
  }

  // Refresh models 
  out.println("<p>Click here to <a href='brokertest.jsp?listtags=on&refresh=true'>refresh underlying data models</a> in the broker.");
 }
%>

<%@include file="errorbox.jsp"%>

</body>
</html>

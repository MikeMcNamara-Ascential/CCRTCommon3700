<%@page import="java.util.*"%>

<%
// 'pages' contains a html links to the modules.
// 'desc' contains a parallel list of module descriptions.
List pages = new ArrayList();
List desc = new ArrayList();
String topFrame = "PortalMain";

pages.add("<a href='remote.jsp' target='"+topFrame+"'>Remote</a>");
desc.add("Control the playback server via remote control");

pages.add("<a href='testresults.jsp' target='"+topFrame+"'>Test Results</a>");
desc.add("View current test results from the server");

pages.add("<a href='vehiclebuild.jsp' target='"+topFrame+"'>Vehicle Build</a>");
desc.add("View build data for the tested vehicle");

pages.add("<a href='stream_frame.html' target='"+topFrame+"'>Wheel Speeds</a>");
desc.add("View live realtime data as it comes from the server");

pages.add("<a href='brakeforce.jsp' target='"+topFrame+"'>Brake Force</a>");
desc.add("View the brake force data graph from the most recent test.");

//pages.add("<a href='brokertest.jsp' target='_top'>Servlet Broker</a>");
//desc.add("Controls for the pseudo-broker used by these servlets.");
%>

<% if ("toc".equals(request.getParameter("mode"))) { %>

<div align="center">
<table width="300" class="nav" border="0">
  <%
  Iterator it = pages.iterator();
  Iterator descit = desc.iterator();
  while (it.hasNext()) {
    out.println("<tr><td>" + it.next() + "</td><td class='desc'>" 
	  + descit.next() + "</td></tr>");  
  }
  %>
</table>
</div>

<% } else { %>

<div align="center">
<table class="nav" border="0">
<tr>
  <%
  Iterator it = pages.iterator();
  while (it.hasNext()) {
    out.println("<td>" + it.next() + "</td>");  
  }
  %>
</tr>
</table>
</div>

<% } %>

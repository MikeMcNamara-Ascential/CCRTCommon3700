<%@page language="java" %>

<%!
static String pageTitle = "CCRT Remote Access Modules";
static String pageName = "index.jsp";
%>

<%@include file="nocache.jsp"%>

<html>
<head>
 <jsp:include page="header.jsp" flush="true">
    <jsp:param name="title" value="<%=pageTitle%>"/>
  </jsp:include> 
</head>
<body bgcolor="white">
<script>

if (!parent.frames[0]) {
  // Ensure we are appearing inside the frameset.
  location = "playportal.jsp?main=index.jsp";
} else {
  // inside the frame
}
</script>
	<h2><%=pageTitle%></h2>
	
	<jsp:include page="navbar.jsp" flush="true">	
	  <jsp:param name="mode" value="toc"/>
	</jsp:include>

</body>
</html>
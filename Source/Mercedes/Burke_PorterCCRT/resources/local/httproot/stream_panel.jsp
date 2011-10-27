<%@page language="java" import="java.util.*"%>

<%@include file="nocache.jsp"%>

<%!
static String pageName="stream_panel.jsp";
static String pageTitle="Wheel Speeds";
%>


<%
List errors = new ArrayList();
%>

<html>

<head>
 <jsp:include page="header.jsp" flush="true">
    <jsp:param name="title" value="<%=pageTitle%>"/>
  </jsp:include> 
  <script language="javascript">
 
  function acceptSpeeds(lf, lr, rf, rr) {
    
    document.forms[0].LFSpeed.value = lf;
    document.forms[0].LRSpeed.value = lr;
    document.forms[0].RRSpeed.value = rr;
    document.forms[0].RFSpeed.value = rf;
  }
  </script>
</head>



<body bgcolor="white" onload="">
<jsp:include page="navbar.jsp" flush="true">
 <jsp:param name="mode" value="bar"/>
</jsp:include>
<center><h2><%=pageTitle%></h2></center>

<form name="guifields">
<table align="center"  border="0">
<tr>
	<td valign="top"><input size="4" name="LFSpeed"></td>
    <td width="100" rowspan="3"><img src="img/driveAxleConnected.png"></td>
	<td valign="top"><input size="4" name="RFSpeed"></td>
</tr>


<tr>
	<td valign="bottom"><input size="4" name="LRSpeed"></td>
	<td valign="bottom"><input size="4" name="RRSpeed"></td>
</tr>
</table>
</form>

<table align="center" width="300">
 <tr><td align="right"><hr><span class="timestamp"><%= new Date() %></span></td></tr>


</body>
</html>

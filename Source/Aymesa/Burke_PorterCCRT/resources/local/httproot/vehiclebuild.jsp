<%@page language="java" import="
         java.io.*, 
		 com.bepco.rtc.playback.*,
		 java.util.*" %>

<%!
static String pageName = "vehiclebuild.jsp";
static String pageTitle = "Vehicle Build";
%>

<%
List errors = new ArrayList();
Map tags = new HashMap();
tags.put("BodyStyle", "");
tags.put("PerformAbortTest", "");
tags.put("EngineController", "");
tags.put("Transmission", "");
tags.put("SpeedControl", "");
tags.put("BrakeType", "");
tags.put("TransferCase", "");
tags.put("Axle", "");
tags.put("SentryKeyImmobilizer", "");
tags.put("Airbag", "");
tags.put("Cluster", "");
tags.put("RemoteKeylessEntry", "");
tags.put("DaytimeRunningLamps", "");
tags.put("PowerSteering", "");
tags.put("HighSpeedChime", "");
tags.put("TirePressureMonitor", "");

/** Make a connection to the broker */
ServletDataBroker broker = null;
try {
  broker = ServletDataBroker.getBroker();
} catch (IOException ex) {
  errors.add("Connecting to data broker: " + ex.getMessage());
}
%>

<html>
<head>
 <jsp:include page="header.jsp" flush="true">
    <jsp:param name="title" value="<%=pageTitle%>"/>
  </jsp:include> 
 <style>
 
 .vehicleBuildTag-low {
   font: normal 10pt arial;
   background-color: white;
 }
 .vehicleBuildTag-high {
   font: normal 10pt arial;
   background-color: #aaaacc;
 }

 .vehicleBuildValue-low {
   font: normal 10pt "courier new", courier;
   background-color: white;
 }
 .vehicleBuildValue-high {
   font: normal 10pt "courier new", courier;
   background-color: #aaaacc;
 }
 
 </style>
</head>

<body bgcolor="white">

<jsp:include page="navbar.jsp" flush="true">
 <jsp:param name="mode" value="bar"/>
</jsp:include>
<center><h2><%=pageTitle%></h2></center>

<table align="center" width="200" cellspacing="0">
<%
Iterator keys = tags.keySet().iterator();
String key = null;
String highlow = "high";
while (keys.hasNext()) {
	key = (String)keys.next();
%>
	 <tr>
	   <td class='vehicleBuildTag-<%=highlow%>'><%=key%></td>
	   <td class='vehicleBuildValue-<%=highlow%>'><%=broker.getString(key)%></td>
	 </tr>
<% 
    if ("high".equals(highlow)) {
	  highlow = "low";
	} else {
	  highlow = "high";
    }
} 
%>
</table>


</body>
</html>

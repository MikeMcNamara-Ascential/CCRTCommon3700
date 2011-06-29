<%@page language="java" import="java.io.*,
          com.bepco.rtc.playback.*
		  " %>

<%@include file="nocache.jsp"%>



<%
// Set the millisecond delay before reposting.
int repostDelay = 1000 / 10;
String errorMsg = null;

/** Make a connection to the broker */
ServletDataBroker broker = null;
try {
  broker = ServletDataBroker.getBroker();
} catch (IOException ex) {
  errorMsg = "Connecting to data broker: " + ex.getMessage();
}
%>

<html>
<head>
 <script language="Javascript">

 // Send the wheel speeds to the peer frame
 function update() {
   if (!parent.frames[0] || !parent.frames[0].acceptSpeeds) {    
     return;
   }
   // JSP sets these values
   var lfspeed = "<%= broker.getString("LFSpeed")%>";
   var lrspeed = "<%= broker.getString("LRSpeed")%>";
   var rfspeed = "<%= broker.getString("RFSpeed")%>";
   var rrspeed = "<%= broker.getString("RRSpeed")%>";
   parent.frames[0].acceptSpeeds(lfspeed, rfspeed, lrspeed, rrspeed);
   setTimeout("reloadPage()", <%= repostDelay %>);
 }
 
 function reloadPage() {
   location = location;
 }
 
 </script>

</head>

<body onload="javascript:update();">

<% if (errorMsg != null) { %>
  <h2>Connection error!</h2>
  <pre>
    <%= errorMsg %>
  </pre>
<% } %>

</body>
</html>
<%@page language="java" import="
    java.io.*,
    java.util.*,
    com.bepco.rtc.playback.*"
 %>


<%
 String refresh = "1000";
 String playClock = "00:00";

 PlaybackServerDaemon daemon = PlaybackServerDaemon.getDaemon();
 if (daemon != null) {
			int millis = daemon.getSimulationTime();
			int tseconds = millis / 1000;
			int tminutes = (tseconds / 60);
			String rseconds = "" + (tseconds - (tminutes*60));
			if (rseconds.length() < 2) {
				rseconds = "0" + rseconds;
			}
			playClock = "" + tminutes + ":" + rseconds;
 }
%>
 
<html>
 <script>
 var refreshTimeout = <%=refresh%>;
 function init() {
	 if (refreshTimeout > 0) {
		 setTimeout("reloadPage()", refreshTimeout);
	 }
 }
 function reloadPage() {
	 location = location;
 }

 </script>
<body bgcolor="white" onload="javascript:init()">
<form>Clock: <input size="5" name="clock" value="<%=playClock%>">
	[<a href="remote.jsp" target="_top">Hide</a>]</form>
</body>
</html>

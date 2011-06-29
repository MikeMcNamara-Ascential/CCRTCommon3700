<%@page language="java" import="java.util.*" %>

<%
/**
 * remote.jsp
 *
 * Provide web-based remote control of CCRT playback daemon
 * @version $Revision$
 */

%>
<%@include file="nocache.jsp"%>

<%!
/** The name of this page */
static String pageName = "remote.jsp";
static String pageTitle = "CCRT Playback Remote";

/** Relative location of icons */
static String imgDir = "img";

static String PONG_TEXT = "PONG";
// The available actions
static final String PING_CMD = "ping";
static final String PLAY_CMD = "play";
static final String PAUSE_CMD = "pause";
static final String RESET_CMD = "reset";
static final String FF_CMD = "ff";

/** URLs for the different buttons*/
static Map buttons = new HashMap();
static {
	buttons.put("play", pageName + "?action=" + PLAY_CMD);
	buttons.put("pause", pageName + "?action=" + PAUSE_CMD);
	buttons.put("reset", pageName + "?action=" + RESET_CMD);
	buttons.put("ff", pageName + "?action=" + FF_CMD);
}

/** Create the HTML needed for clickable buttons */
static String buildButton(String name) {
	long tstamp = System.currentTimeMillis();
	String ret = "<a href='" + buttons.get(name) + "&time="+tstamp+"'>\n";
	ret += "<img src='" + imgDir + "/web_"+name+"_off.png' name='"+name+"' ";
	ret += " border='0' onMouseDown='press(\""+name+"\");return true;'";
  ret += ">\n";
	ret += "</a>\n</td>\n";
	return ret;
	
}
%>

<%
// Process action 
String action = request.getParameter("action");
com.bepco.rtc.playback.PlaybackServerDaemon daemon =
 com.bepco.rtc.playback.PlaybackServerDaemon.getDaemon();
List errors = new LinkedList();
String playMessage = null;

if (daemon == null) {
	errors.add("Could not establish a link to the playback daemon!");
} else {
	if (PING_CMD.equals(action)) {
		out.println(PONG_TEXT);
		return; // end right here!

	} else if (PLAY_CMD.equals(action)) {
		daemon.play();
		playMessage = "Playing";
		
	} else if (PAUSE_CMD.equals(action)) {
		daemon.pause();
		playMessage = "Pausing";
	
	} else if (RESET_CMD.equals(action)) {
		try {
			daemon.reset();
			playMessage = "Resetting";

		} catch (com.bepco.data.ConfigurationException ex) {
			errors.add("While refreshing playback scripts: " + ex.toString());
		}
	} else if (FF_CMD.equals(action)) {
		daemon.fastForward();
		playMessage = "Playing";
		
	} else if (action == null) {
		// No action specified; nothing special
	} else {
		// Invalid action specified
		errors.add("Unkown action: " + action);
	}
}

%>
 
<html>
 <head>
  <jsp:include page="/header.jsp" flush="true">
    <jsp:param name="title" value="<%=pageTitle%>"/>
  </jsp:include> 


  <script language="Javascript">
  //
  // Change the image for a button when it gets clicked
  //
  function press(name) {
    document[name].src = "<%=imgDir%>/web_"+name+"_on.png";
  }

  </script>

</head>
<body bgcolor="white" name="bob">

<jsp:include page="/navbar.jsp" flush="true">
 <jsp:param name="mode" value="bar"/>
</jsp:include>
  
  <center><h2><%=pageTitle%></h2></center>

<div align="center">

<!-- 
  The button panel
-->	 
<table align="center" width="300" border="0">
<tr><td height="80" align=center valign="middle">
  <table border="0" cellpadding="0" cellspacing="0">
	 <tr>
	   <td height="63" width="53"><%= buildButton("play") %></td>
	   <td height="63" width="44"><%= buildButton("pause") %></td>
	   <td height="63" width="44"><%= buildButton("reset") %></td>
 	   <td height="63" width="53"><%= buildButton("ff") %></td>
    </tr>
  </table>
</td></tr>
</table>

<table align="center" width="300" border="0">
  <tr>
    <% if (playMessage != null) { %>
	  <td><span class="playMessage"><%= playMessage %></span></td>
	<% } %> 
    <td align="right"><hr /><span class="timestamp"><%= new Date() %></span>
	<script>
    if (!parent.frames[0]) {
	  document.writeln("<br><a href='playportal.jsp?main=remote.jsp'>View Clock</a>");
    } else {
	  document.writeln("<br><a href='remote.jsp' target='_top'Hide Clock</a>");
    }
	</script>
	</small></td>
  </tr>
  <tr><td><%@include file="errorbox.jsp"%></td></tr>
</table>
</div>
 




</body>
</html>









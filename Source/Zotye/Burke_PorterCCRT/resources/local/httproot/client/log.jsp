<%@page language="java" import="
     java.io.*, 
		 java.util.*,
		 java.net.*,
		 com.bepco.rtc.playback.*,
		 com.bepco.logging.*" %>

<%-- This JSP is disabled but not removed --%>
  <h1>Log Viewer Disabled</h1>
  <p>Web-based log viewing is currently unavailable pending re-design.
<%
    if (true) {
		// "Hide" this logic flaw from the compiler
		return;
    }
%> 

<%@include file="/nocache.jsp"%>

<%!
static String pageName = "log.jsp";
static String pageTitle = "CCRT Client Log";
static String logFile = "/local/main.log";
%>

<%
//Init form helper variables
Map optSelected = new HashMap();
optSelected.put("first", "");
optSelected.put("last", "");
optSelected.put("10", "");
optSelected.put("20", "");
optSelected.put("50", "");
optSelected.put("100", "");
optSelected.put("ALL", "");

// get desired record count 
String countStr = request.getParameter("count");
int count = -1; 
if (countStr == null) {
	countStr = "20";
}
optSelected.put(countStr, "SELECTED");
try {
	count = Integer.parseInt(countStr);
} catch (NumberFormatException ex) {
	// ok
}

// Get first/last direction
String end = request.getParameter("end");
if (end == null) {
	end = "last";
}
optSelected.put(end, "SELECTED");

%>

<%
List errors = new ArrayList();

// Create a log record stream from the log file
LogRecordStream logStream = null;
try {
  URL logURL = getClass().getResource(logFile);
	logStream = new LogRecordXMLStream(logURL.openStream());
} catch (IOException ex) {
	errors.add("Failed to open " + logFile + ": " + ex.getMessage());
}

// Capture a list of records from the stream
List records = new LinkedList();
LogRecord rec = null;
try {
	rec = logStream.nextRecord();
} catch (LogRecordException ex) {
	errors.add("Broken log record: " + ex.getMessage());
}
while (rec != null) {
	try {
		records.add(rec);
		if ("first".equals(end) && count != -1) {
			if (records.size() >= count) {
				// stop after getting the first  'count' records
				break;
			}
		} else if ("last".equals(end) && count != -1) {
			if (records.size() >= count) {
				// Slide the window of records.
				records.remove(0);
			}
		}
		rec = logStream.nextRecord();
	} catch (LogRecordException ex) {
		errors.add("Broken log record: " + ex.getMessage());
	}
}

%>

<html>
<head>
 <style>
 h2 {
   text-align: center;
   font: bold italic 12pt arial;
 }
 th {
   font: bold 10pt arial;
   background-color: black;
   color: white;
 }
 td {
   font: normal 8pt arial;
 }
 tr.high {
   background-color: #bbbbff;
 }

 .high .level {
   background-color: #ddddff;
   text-align: center;
 }
 .low .level {
   background-color: #eeeeff;
   text-align: center;
 }
 td.levelsevere {
   background-color: #ff8888;
   text-align: center;
 }
 td.levelwarning {
   background-color: #ff6633;
   font-weight: bold;
   text-align: center;
   color: #660000;
 }
 form {
   font: normal 10pt verdana, arial, helvetica;
 }

 td.thread {
   text-align: center;
 }
 </style>
</head>

<body>
<h2><%=pageTitle%></h2>

<form action="<%=pageName%>">
  View only the
  <select name="end">
    <option value="first" <%=(String)optSelected.get("first")%>>first</option>
    <option value="last" <%=(String)optSelected.get("last")%>>last</option>
  </select>
  <select name="count">
    <option value="10" <%=(String)optSelected.get("10")%>>10</option>
    <option value="20" <%=(String)optSelected.get("20")%>>20</option>
    <option value="50" <%=(String)optSelected.get("50")%>>50</option>
    <option value="100" <%=(String)optSelected.get("100")%>>100</option>
    <option value="ALL" <%=(String)optSelected.get("ALL")%>>ALL</option>
  </select>
  records...<input type="submit" value="Reload">
</form>

<div>
<table align="left" cellspacing="0">
<%
List columnNames = new ArrayList();
columnNames.add("Date");
columnNames.add("Thread");
columnNames.add("Level");
columnNames.add("Source");
columnNames.add("Message");
columnNames.add("Exception");

// Print col headers
Iterator cols = columnNames.iterator();
out.println("<tr>");
while (cols.hasNext()) {
	out.println("<th>"+ cols.next() +"</th>");
}
out.println("</tr>");

String highlow = "high";
String levelClass = "level";

Iterator it = records.iterator();
while (it.hasNext()) {
	rec = (LogRecord)it.next();
	out.println("<tr class='"+ highlow+"'>");
	if ("high".equals(highlow)) {
		highlow = "low";
	} else {
		highlow = "high";
	}
	out.println("<td class='date'>"+
			Logger.LOG_DATE_FORMAT.format(new Date(rec.getMillis()))+"</td>");

	out.println("<td class='thread'>"+ rec.getThreadID() +"</td>");

	if (Level.SEVERE.equals(rec.getLevel())) {
	  levelClass = "levelsevere";
    } else if (Level.WARNING.equals(rec.getLevel())) {
	  levelClass = "levelwarning";
    } else {
	  levelClass = "level";
    }
	out.println("<td class='"+levelClass+"'>"
	    + rec.getLevel().toString() + "</td>");

	out.println("<td class='source'>"+rec.getSourceClassName() + "."
			+ rec.getSourceMethodName()+"</td>");
	
	out.println("<td class='message'>"+ makeHtmlSafe(rec.getMessage()) +"</td>");

	if (rec.getThrown() != null) {
		out.println("<td class='exception'>"+rec.getThrown().getMessage()+"</td>");
	}
	
	out.println("</tr>");
}

// Print col headers again at the bottom
cols = columnNames.iterator();
out.println("<tr>");
while (cols.hasNext()) {
	out.println("<th>"+ cols.next() +"</th>");
}
out.println("</tr>");
%>
</table>
</div>
<a name="pagebottom"> </a>

<%@include file="/errorbox.jsp"%>
</body>
</html>

<%!
   /**
	* Escape HTML-unsafe characters inside 'text'
	*/
static String makeHtmlSafe(String text) {
	StringBuffer outbuf = new StringBuffer();
	for (int i = 0; i < text.length(); i++) {
    if (text.charAt(i) == '<') {
			outbuf.append("&lt;");
		}
		else if (text.charAt(i) == '&') {
			outbuf.append("&amp;");
		} else {
			outbuf.append(text.charAt(i));
		}
	}
	return outbuf.toString();
}
%>



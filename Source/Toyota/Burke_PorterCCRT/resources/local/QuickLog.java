import java.io.*;
import java.util.*;
import java.net.*;

import com.bepco.rtc.*;
import com.bepco.rtc.playback.*;
import com.bepco.logging.*;

 
public class QuickLog {

	static String pageTitle = "CCRT Client Log";
	static String logFile = LocalUserProperties.LOCAL_DIRECTORY + "main.log";

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

	public QuickLog() {}

	public static void main(String args[]) throws Exception {
		PrintWriter out = new PrintWriter(new OutputStreamWriter(System.out));
		
		out.write("\r\n\r\n");
                
		// get desired record count 
		int count = -1; 
		String end = "last";
		
		List errors = new ArrayList();
                
		// Create a log record stream from the log file
		LogRecordStream logStream = null;
		try {
			URL logURL = QuickLog.class.getResource(logFile);
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
		
		out.write("\r\n\r\n<html>\r\n<head>\r\n <style>\r\n h2 {\r\n   text-align: center;\r\n   font: bold italic 12pt arial;\r\n }\r\n th {\r\n   font: bold 10pt arial;\r\n   background-color: black;\r\n   color: white;\r\n }\r\n td {\r\n   font: normal 8pt arial;\r\n }\r\n tr.high {\r\n   background-color: #bbbbff;\r\n }\r\n\r\n .high .level {\r\n   background-color: #ddddff;\r\n   text-align: center;\r\n }\r\n .low .level {\r\n   background-color: #eeeeff;\r\n   text-align: center;\r\n }\r\n td.levelsevere {\r\n   background-color: #ff8888;\r\n   text-align: center;\r\n }\r\n td.levelwarning {\r\n   background-color: #ff6633;\r\n   font-weight: bold;\r\n   text-align: center;\r\n   color: #660000;\r\n }\r\n form {\r\n   font: normal 10pt verdana, arial, helvetica;\r\n }\r\n\r\n td.thread {\r\n   text-align: center;\r\n }\r\n </style>\r\n</head>\r\n\r\n<body>\r\n<h2>");
		out.print(pageTitle);
		out.write("</h2>");
		out.write("<div>\r\n<table align=\"left\" cellspacing=\"0\">\r\n");
                
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
		out.write("\r\n</table>\r\n</div>\r\n<a name=\"pagebottom\"> </a>\r\n\r\n");
		if (errors.size() > 0) { 
			out.write("\r\n<hr>\r\n<span class=\"errorTitle\">Error:</span>\r\n<ul class=\"errorList\">\r\n\t ");
			Iterator errorIt = errors.iterator();
			while (errorIt.hasNext()) {
				out.println("<li>" + errorIt.next() + "<li>\n");
			} 
                	 
			out.write("\r\n </ul>\r\n");
		} // End if errors > 0 
		out.write("\r\n");
		out.write("\r\n</body>\r\n</html>\r\n\r\n");
		out.write("\r\n\r\n\r\n");
		out.flush();
		
	}
}

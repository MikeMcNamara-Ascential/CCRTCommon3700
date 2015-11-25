<% if (errors.size() > 0) { %>
<hr>
<span class="errorTitle">Error:</span>
<ul class="errorList">
	 <%
	   Iterator errorIt = errors.iterator();
	   while (errorIt.hasNext()) {
	     out.println("<li>" + errorIt.next() + "<li>\n");
	   } 
	 %>
 </ul>
<% } // End if errors > 0 %>

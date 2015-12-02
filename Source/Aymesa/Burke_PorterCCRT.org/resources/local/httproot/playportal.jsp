<%@page language="java" %>

<%
String mainPage = "index.jsp";
if (request.getParameter("main") != null 
    && !request.getParameter("main").equals("")) {
  mainPage = request.getParameter("main");
}
%>
<html>
<frameset rows="*,25">
  <frame name="PortalMain" src="<%=mainPage%>">
  <frame name="PortalFooter" src="portalfooter.jsp">
</frameset>
</html>
<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"  version="1.0"   >
    <xsl:output method="xml"/>

    <!-- Locate the test results -->
    <xsl:param name="allresults" select="/BepVehicleTestResult/TestResult" />

    <!-- Locate the vehicle build data block -->
    <xsl:param name="vehiclebuild" select="/BepVehicleTestResult/VehicleBuild" />
    
    <!-- Locate the Brake Results block -->
    <xsl:param name="brakeresults" select="/BepVehicleTestResult/BrakeResults" />

    <xsl:template match="/BepVehicleTestResult">
        <html>
            <head>
                <title>
                    <xsl:value-of select="RtReportConfig/ReportLayout/Title"/>
                </title>
            </head>
            <body>
                <xsl:apply-templates select="RtReportConfig/ReportLayout"/>
            </body>
        </html>
    </xsl:template>

    <!-- 
        ReportLayout 
        Navigate the user-defined layout and generate output,
        based on the test result and vehicle build data.
    -->
    <xsl:template match="ReportLayout">
        <!-- Process TestIdBlock, TestResultBlock, BuildDataBlock, Title, etc. -->
        <xsl:apply-templates/>
    </xsl:template>

    <!-- Print the report title -->
    <xsl:template match="ReportLayout/Title">
        <center>
            <h1>
                <xsl:value-of select="."/>
            </h1>
        </center>
    </xsl:template>

    <!-- Horizontal bar separator -->
    <xsl:template match="HorizontalBar">
        <hr />
    </xsl:template>

    <!-- Blank line separator -->
    <xsl:template match="BlankLine">
        <p/>
    </xsl:template>

    <!-- Render the TestIdBlock -->
    <xsl:template match="ReportLayout/TestIdBlock">
        <table width="100%">
            <tr>
                <xsl:for-each select="Column">
                    <td valign="top">
												<xsl:attribute name="width">
											  	<xsl:value-of select="@width"/>
												</xsl:attribute>
                        <xsl:apply-templates mode="testidblock"/>
                    </td>
                </xsl:for-each>
            </tr>
        </table>
    </xsl:template>

    <!-- Render the Brake Results -->
    <xsl:template match="ReportLayout/BrakeResultsBlock">
        <table width="100%">
            <tr>
                <xsl:for-each select="Column">
										<td valign="top">
												<xsl:attribute name="width">
											  	<xsl:value-of select="@width"/>
												</xsl:attribute>
                        <xsl:apply-templates mode="brakeresultsblock"/>
                    </td>
                </xsl:for-each>
            </tr>
        </table>
    </xsl:template>


    <!--
      convert a Timestamp element into a text rendition
        of the time string value in it
    -->
    <xsl:template match="Timestamp">
        <p>
            <xsl:value-of select="@timestring"/>
        </p>
    </xsl:template>
    <xsl:template match="Timestamp" mode="testidblock">
        <p>
            <xsl:value-of select="@timestring"/>
        </p>
    </xsl:template>
    <xsl:template match="Timestamp" mode="builddatablock">
        <p>
            <xsl:value-of select="@timestring"/>
        </p>
    </xsl:template>
    <xsl:template match="Timestamp" mode="testresultsblock">
        <p>
            <xsl:value-of select="@timestring"/>
        </p>
    </xsl:template>

    <!-- Render the Static text tags -->
    <xsl:template match="StaticText" mode="testidblock">
        <p>
            <xsl:value-of select="@label"/>
        </p>
    </xsl:template>

    <xsl:template match="StaticText" mode="builddatablock">
        <p>
            <xsl:value-of select="@label"/>
        </p>
    </xsl:template>

    <xsl:template match="StaticText" mode="testresultsblock">
        <p>
            <xsl:value-of select="@label"/>
        </p>
    </xsl:template>

   <xsl:template match="StaticText" mode="brakeresultsblock">
        <p>
            <xsl:value-of select="@label"/>
        </p>
    </xsl:template>


    <!--
      TestResultDetail 
          Make sure we get included in all the various modes
    -->
    <xsl:template match="TestResultDetail">
        <xsl:call-template name="getTestDetail"/>
    </xsl:template>
    <xsl:template match="TestResultDetail" mode="testidblock">
        <xsl:call-template name="getTestDetail"/>
    </xsl:template>
    <xsl:template match="TestResultDetail" mode="builddatablock">
        <xsl:call-template name="getTestDetail"/>
    </xsl:template>
    <xsl:template match="TestResultDetail" mode="testresultsblock">
        <xsl:call-template name="getTestDetail"/>
    </xsl:template>
    <xsl:template match="BrakeResult" mode="brakeresultsblock">
        <xsl:call-template name="getBrakeDetail"/>
    </xsl:template>
    


    <!-- Now do the real work for TestResultDetail -->
    <xsl:template name="getTestDetail">
        <p>
            <b>
                <xsl:value-of select="@label"/>
                :</b>
            <xsl:value-of select="/BepVehicleTestResult/TestResult/*[name()=current()/@tagname]/*[name()=current()/@childtagname]"/>
        </p>
    </xsl:template>

    <!-- Try to get something into a brake result block -->
    <xsl:template name="getBrakeDetail">
        <p>
            <xsl:value-of select="@label"/>
            <xsl:value-of select="/BepVehicleTestResult/TestResult/*[name()=current()/@tagname]/*[name()=current()/@childtagname]"/>
        </p>
    </xsl:template>



    <!-- Render ServerTag elements within TestIdBlock -->
    <xsl:template match="ServerTag" mode="testidblock">
        <p>
            <b>
                <xsl:value-of select="@label"/>
                :</b>
            <xsl:value-of select="$vehiclebuild/*[name() = current()/@tagname]"/>
        </p>
    </xsl:template>

	<!-- Render the fetched server tag -->
	<xsl:template match="FetchedServerTag" mode="testidblock">
		<p>
			<b><xsl:value-of select="@label"/>: </b>
			<xsl:value-of select="/BepVehicleTestResult/FetchedServerTags/*[name() = current()/@tagname]"/>
		</p>
	</xsl:template>
    
    <!-- Render BuildData block -->
    <xsl:template match="ReportLayout/BuildDataBlock">
        <table width="100%">
            <tr>
                <td>
                    <b>
                        <xsl:value-of select="@label"/>
                    </b>
                </td>
            </tr>
            <tr>
                <xsl:for-each select="Column">
                    <td valign="top">
												<xsl:attribute name="width">
												 	<xsl:value-of select="@width"/>
												</xsl:attribute>
                        <ul>
                            <xsl:apply-templates mode="builddatablock"/>
                        </ul>
                    </td>
                </xsl:for-each>
            </tr>
        </table>
    </xsl:template>

    <!-- Render ServerTag elements within BuildBlock -->
    <xsl:template match="ServerTag" mode="builddatablock">
        <li>
            <xsl:value-of select="@label"/>
            :
            <xsl:value-of select="/BepVehicleTestResult/VehicleBuild/*[name() = current()/@tagname]"/>
        </li>
    </xsl:template>

    <!-- 
      Render the TestResultsBlock 
    -->
    <xsl:template match="ReportLayout/TestResultsBlock">
        <table width="100%">
            <tr>
                <xsl:for-each select="Column">
                    <td valign="top">
												<xsl:attribute name="width">
											  	<xsl:value-of select="@width"/>
												</xsl:attribute>
                        <xsl:apply-templates mode="testresultsblock"/>
                    </td>
                </xsl:for-each>
            </tr>
        </table>
    </xsl:template>

    <!-- 
      Given a process name, build a test result table based on
      all test result tags sharing that process name     
    -->
    <xsl:template match="TestProcess" mode="testresultsblock">
        <!-- Find the targeted tag -->
        <xsl:param name="overall"
          select="$allresults/*[name() = current()/@tagname]"/>
        <!-- If the targeted tag is present in the results, the status of the
             process is equal to the 'Result' attribute of the element.
             (Eg, "Pass", "Fail" or "Abort".)
             Otherwise, the status is "Not Tested".
        -->
        <xsl:param name="status">
            <xsl:choose>
                <xsl:when test="string-length($overall/@Result) &gt; 0">
                    <xsl:value-of select="$overall/@Result"/>
                </xsl:when>
                <xsl:when test="$overall and string-length($overall/@Result) &lt; 1"></xsl:when>
                <xsl:otherwise>Not Tested</xsl:otherwise>
            </xsl:choose>
        </xsl:param>
        <!-- Find all failures associated with this process -->
        <!--   <xsl:param name="failures"  -->
        <!-- 	select="$allresults/*[@Process = $overall/@Process][@DoDetail = 'true'][not(@PrimaryElement = 'true')]"/> -->
        <table>
            <xsl:if test="not(@tagname='AllFailures')">
                <tr>
                    <td>
                        <b>
                            <xsl:value-of select="@label"/>:
                        </b>
                        <xsl:value-of select="$status"/>
                    </td>
                </tr>
                <!-- If: - this process has details enabled
                    - and this process has OverallTest failure
                - and there are some actual test step elements that need displaying
                -->
                <xsl:choose>
                    <!-- If it says no detail, do nothing. -->
                    <xsl:when test="@showdetail='none'"></xsl:when>
                    <xsl:when test="@showdetail='all'">
                        <!-- If it said all detail and it passed, match the passes template -->
                        <tr>
                            <td>
                                <ul>
                                    <xsl:choose>
                                        <xsl:when test="$allresults/*[@Process=$overall/@Process]">
                                    <xsl:apply-templates select="$allresults/*[@Process=$overall/@Process]" mode="listStepsWithDetail"/>
                                        </xsl:when>
                                        <xsl:when test="$allresults/*[name() = current()/@tagname]">
                                            <xsl:apply-templates select="$allresults/*[name() = current()/@tagname]" mode="listStepsWithDetail">
                                                
                                            </xsl:apply-templates>
                                        </xsl:when>
                                        <xsl:otherwise></xsl:otherwise>
                                    </xsl:choose>
                                </ul>
                            </td>
                        </tr>
                    </xsl:when>  <!-- end if showdetail='all' -->
                    <xsl:when test="@showdetail='failures'">
                        <tr>
                            <td>
                                <ul>
                                    <xsl:apply-templates select="$allresults/*[@Process=$overall/@Process]" mode="listFailedStepsWithDetail"/>
                                </ul>
                            </td>
                        </tr>
                    </xsl:when>
                </xsl:choose>
            </xsl:if>
            <xsl:if test="@tagname='AllFailures'">
                <tr>
                    <td>
                        <b>
                            <xsl:value-of select="@label"/>:
                        </b>
                    </td>
                </tr>
                <tr>
                    <td>
                        <ul>
                            <xsl:call-template name="listAllFailuresDetails"/>
                        </ul>
                    </td>
                </tr>
            </xsl:if>
        </table>
    </xsl:template>

    <xsl:template name="expandTestStep">
        <xsl:choose>
            <xsl:when test="string-length(@Result) &gt; 0">
        <li>
                    <xsl:value-of select="@Result"/>:
            <xsl:value-of select="@Description"/>
            <xsl:if test="*">
                <ul>
                    <xsl:for-each select="*">
                        <li>
                                    <xsl:value-of select="name()"/>:
                            <xsl:value-of select="."/>
                        </li>
                    </xsl:for-each>
                </ul>
            </xsl:if>
        </li>
            </xsl:when>
            <xsl:otherwise>
                <xsl:if test="*">
                    <xsl:for-each select="*">
                        <li>
                            <xsl:apply-templates select="current()" mode="innerExpand"/>
                        </li>
                    </xsl:for-each>
                </xsl:if>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <xsl:template match="*" mode="innerExpand">
        <xsl:value-of select="name()"/>
        <xsl:choose>
            <xsl:when test="count(./*) &lt; 1 and not(.='')">:
                <xsl:value-of select="."/>
            </xsl:when>
            <xsl:when test="count(./*) &lt; 1">
            </xsl:when>
            <xsl:otherwise>
                <ul>
                    <xsl:call-template name="expandTestStep"/>
                </ul>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <xsl:template match="*" mode="listStepsWithDetail">
        <xsl:if test="not(@PrimaryElement='true')">
            <xsl:call-template name="expandTestStep"/>
        </xsl:if>
    </xsl:template>

    <xsl:template match="*" mode="listFailedStepsWithDetail">
        <xsl:if test="not(@PrimaryElement='true')">
            <xsl:if test="@Result='Fail'">
                <xsl:call-template name="expandTestStep"/>
            </xsl:if>
            <xsl:if test="@Result='Timeout'">
                <xsl:call-template name="expandTestStep"/>
            </xsl:if>
        </xsl:if>
    </xsl:template>

    <xsl:template name="listDTCdetails">
            <!--<xsl:for-each select="/BepVehicleTestResult/TestResult/DTC/child::*">-->
            <xsl:for-each select="$allresults/DTC/child::*">
                <li>
                    <xsl:value-of select="name()"/>
                    <ul>
                    <xsl:for-each select="current()/*">
                            <li>
                            <xsl:value-of select="name()"/>
:
                            <xsl:value-of select="."/>
                            </li>
                        </xsl:for-each>
                    </ul>
                </li>
            </xsl:for-each>
    </xsl:template>

    <xsl:template name="listAllFailuresDetails">
            <xsl:for-each select="$allresults/AllFailures/child::*">
                <li>
                    <xsl:value-of select="name()"/>
                    <xsl:call-template name="expandAllFailures"/>
                </li>
            </xsl:for-each>
    </xsl:template>

    <xsl:template name="expandAllFailures">-
            <xsl:value-of select="@Result"/>
            :
            <xsl:value-of select="@Description"/>
            <xsl:if test="*">
                <ul>
                    <xsl:for-each select="*">
                        <li>
                            <xsl:value-of select="name()"/>
                            -
                            <xsl:value-of select="."/>
                        </li>
                    </xsl:for-each>
                </ul>
            </xsl:if>
    </xsl:template>
</xsl:stylesheet>

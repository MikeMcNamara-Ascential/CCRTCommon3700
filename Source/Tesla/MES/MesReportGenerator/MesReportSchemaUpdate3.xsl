<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xalan="http://xml.apache.org/xalan">
	<xsl:output method="xml" version="1.0" indent="yes" xalan:indent-amount="2"/>
	<xsl:strip-space elements="*"/>

	<xsl:template match="/BepVehicleTestResult">
		<xsl:element name="QualityTest">
			<xsl:element name="QualityTestNumber">
				<xsl:value-of select="//VehicleBuild/VIN"/>
			</xsl:element>
			<xsl:call-template name="BrakeModule"/>
            <xsl:call-template name="AirbagModule"/>
		</xsl:element>
	</xsl:template>




	<xsl:template name="BrakeModule" match="//TestResult">
        <xsl:element name="Module">
            <xsl:call-template name="BrakeModuleInfo">
                <xsl:with-param name="NAME">BrakeControlModule</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="ModuleTest">
                <xsl:with-param name="NAME">BrakeValveFiring</xsl:with-param>
                <xsl:with-param name="NODE">//TestResult/BrakeValveFiring</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="ModuleTest">
                <xsl:with-param name="NAME">BrakeFrontToRearBalanceTest</xsl:with-param>
                <xsl:with-param name="NODE">//TestResult/BrakeFrontToRearBalanceTest</xsl:with-param>
            </xsl:call-template>
        </xsl:element>
	</xsl:template>

    <xsl:template name="AirbagModule" match="//TestResult">
        <xsl:element name="Module">
            <xsl:call-template name="AirbagModuleInfo">
                <xsl:with-param name="NAME">AirbagControlModule</xsl:with-param>
            </xsl:call-template>
            <xsl:element name="ModuleTest"/>
        </xsl:element>
    </xsl:template>



	<xsl:template name="BrakeModuleInfo">
		<xsl:param name="NAME"/>
		<xsl:element name="ModuleName">
			<xsl:value-of select="$NAME"/>
		</xsl:element>
		<xsl:element name="ModulePartNumber">
			<xsl:value-of select="//TestResult/BrakeCheckPartNumber/ModulePartNumber"/>
		</xsl:element>
		<xsl:element name="SerialNumber">
			<xsl:value-of select="//TestResult/BrakeCollectTraceData/SerialNumber"/>
		</xsl:element>
        <xsl:element name="Result">
            <xsl:value-of select="//TestResult/BrakeOverallTest/@Result"/>
        </xsl:element>
	</xsl:template>

    <xsl:template name="AirbagModuleInfo">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleName">
            <xsl:value-of select="$NAME"/>
        </xsl:element>
        <xsl:element name="ModulePartNumber">
            <xsl:value-of select="//TestResult/AirbagCheckPartNumber/ModulePartNumber"/>
        </xsl:element>
        <xsl:element name="SerialNumber">
            <xsl:value-of select="//TestResult/AirbagCollectTraceData/SerialNumber"/>
        </xsl:element>
        <xsl:element name="Result">
            <xsl:value-of select="//TestResult/AirbagOverallTest/@Result"/>
        </xsl:element>
    </xsl:template>


    <xsl:template name="ModuleTest">
        <xsl:param name="NAME"/>
        <xsl:param name="NODE"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:variable name="processNode" select="concat($NODE,'/@Process')"/>
                <xsl:for-each select="$processNode">
                    <xsl:value-of select="current()"/>
                </xsl:for-each>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/NAME/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/NAME/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/NAME/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:value-of select="//TestResult/NAME"/>
            </xsl:element>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/NAME">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="local-name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:value-of select="./@Units"/>
                            </xsl:attribute>
                            <xsl:value-of select="."/>
                        </xsl:element>
                    </xsl:element>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>


</xsl:stylesheet> 
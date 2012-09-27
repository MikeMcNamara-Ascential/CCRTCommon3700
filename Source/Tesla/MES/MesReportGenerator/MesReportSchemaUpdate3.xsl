<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xalan="http://xml.apache.org/xalan">
	<xsl:output method="xml" version="1.0" indent="yes" xalan:indent-amount="2"/>
	<xsl:strip-space elements="*"/>

	<xsl:template match="/BepVehicleTestResult">
		<xsl:element name="QualityTest">
			<xsl:element name="QualityTestNumber">
				<xsl:value-of select="//VehicleBuild/VIN"/>
			</xsl:element>
			<xsl:call-template name="module"/>
		</xsl:element>
	</xsl:template>


	<xsl:template name="module" match="//TestResult">
		<xsl:for-each select="//TestResult/BrakeCheckPartNumber">
			<xsl:call-template name="moduleInfo">
				<xsl:with-param name="NAME">BrakeControlModule</xsl:with-param>
			</xsl:call-template>
		</xsl:for-each>
	</xsl:template>

	<xsl:template name="moduleInfo">
		<xsl:param name="NAME"/>
		<xsl:element name="ModuleName">
			<xsl:value-of select="$NAME"/>
		</xsl:element>
		<xsl:element name="ModulePartNumber">
			<xsl:value-of select="ModulePartNumber"/>
		</xsl:element>
		<xsl:element name="SerialNumber">
			<xsl:value-of select="SerialNumber"/>
		</xsl:element>
	</xsl:template>


</xsl:stylesheet> 
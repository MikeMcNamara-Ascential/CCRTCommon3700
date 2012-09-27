<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xalan="http://xml.apache.org/xalan">
	<xsl:output method="xml" version="1.0" indent="yes" xalan:indent-amount="2"/>
	<xsl:strip-space elements="*"/>

	<xsl:template match="/">
		<xsl:text>&#xa;</xsl:text>
		<xsl:element name="VIN_{BepVehicleTestResult/TestResult/AirbagCheckVIN/ModuleVIN}">
			<xsl:text>&#xa;</xsl:text>
			<xsl:for-each select="BepVehicleTestResult/VehicleBuild">
					<xsl:copy-of select="DriveCurve"/>
			</xsl:for-each>
			<xsl:element name="BrakeConrolModule">
				<xsl:comment>ABS controller Part Number as read over CAN</xsl:comment> 
				<xsl:for-each select="BepVehicleTestResult/TestResult/BrakeCheckPartNumber">
					<xsl:copy-of select="ModulePartNumber"/>
				</xsl:for-each>
				<xsl:comment>Presense of DTCs (if any)</xsl:comment>
				<xsl:element name="DTC">
					<xsl:for-each select="BepVehicleTestResult/TestResult/DTC">
						<xsl:copy-of select="Brake"/>
					</xsl:for-each>
				</xsl:element>
				<xsl:comment>ABS valve test results</xsl:comment>
				<xsl:for-each select="BepVehicleTestResult/TestResult">
						<xsl:copy-of select="BrakeValveFiringTest"/>
						<xsl:comment>Valve release/recovery</xsl:comment>
						<xsl:copy-of select="BrakeDumpAndIsolateAllWheels"/>
						<xsl:copy-of select="BrakeRecoverAllWheels"/>
						<xsl:comment>Valve release/recovery (detail)</xsl:comment>
						<xsl:copy-of select="BrakeLeftFrontReduction"/>
						<xsl:copy-of select="BrakeRightFrontReduction"/>
						<xsl:copy-of select="BrakeLeftRearReduction"/>
						<xsl:copy-of select="BrakeRightRearRecovery"/>
						<xsl:copy-of select="BrakeLeftFrontRecovery"/>
						<xsl:copy-of select="BrakeRightFrontRecovery"/>
						<xsl:copy-of select="BrakeLeftRearRecovery"/>
						<xsl:copy-of select="BrakeRightRearRecovery"/>
						<xsl:comment>Balance</xsl:comment>
						<xsl:copy-of select="BrakeAnalyzeBalance"/>
						<xsl:comment>F/R balance</xsl:comment>
						<xsl:copy-of select="BrakeFrontToRearBalanceTest"/>
						<xsl:comment>L/R balance</xsl:comment>
						<xsl:copy-of select="BrakeFrontBalanceTest"/>
						<xsl:copy-of select="BrakeRearBalanceTest"/>
				</xsl:for-each>	
			</xsl:element>
			<xsl:text>&#xa;</xsl:text>
			<xsl:element name="AirbagControlModule">
				<xsl:for-each select="BepVehicleTestResult/TestResult">
					<xsl:comment>Tesla Airbag Control Module Part Number: 12 ASCII characters for storage in MES</xsl:comment> 
					<xsl:copy-of select="AirbagCheckModulePartNumber/ModulePartNumber"/>
					<xsl:comment>Delphi Airbag Control Module Part Number: 9 ASCII characters</xsl:comment>
					<xsl:copy-of select="AirbagCheckOEMPartNumber/ModulePartNumber"/>
					<xsl:comment>Airbag Control Module Hardware Serial Number ID: 4 ASCII characters for storage in MES</xsl:comment>
					<xsl:copy-of select="AirbagCollectTraceData/SerialNumber"/>
					<xsl:comment>Airbag Control Module Firmware Version: 18 ASCII characters</xsl:comment>
					<xsl:copy-of select="AirbagCheckFWChecksum/ModulePartNumber"/>
					<xsl:comment>Airbag Arming Status: Pass or Fail?</xsl:comment>
					<xsl:element name="Result">
						<xsl:value-of select="AirbagEnableAirBagModule/@Result"/> 
					</xsl:element>	
				</xsl:for-each>
				<xsl:comment>Airbag Control Module DTC's post rolltest (Will be of varying length.  Please confirm max memory allocated for similar procedure)</xsl:comment>
				<xsl:element name="DTC">
						<xsl:for-each select="BepVehicleTestResult/TestResult/DTC">
							<xsl:copy-of select="Airbag"/>
						</xsl:for-each>
				</xsl:element>
			</xsl:element>
		<xsl:text>&#xa;</xsl:text>
		</xsl:element>
	</xsl:template>
</xsl:stylesheet> 
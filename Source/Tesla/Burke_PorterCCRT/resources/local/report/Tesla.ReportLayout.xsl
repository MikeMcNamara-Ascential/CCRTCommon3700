<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

    <xsl:template match="/BepVehicleTestResult">
        <html>
            <body>
            
                <!-- Build Data -->
                <xsl:call-template name="BuildData"/>

                <!-- Test Results -->
                <xsl:call-template name="TestResults"/>

            </body>
        </html>
    </xsl:template>

    <!-- Build Data -->
    <xsl:template name="BuildData" match="/BepVehicleTestResult/VehicleBuild">
        <h3>
            <u>Tesla EOL Test Report</u>
        </h3>
        <table border="1" rules="all" cellpadding="1">
            <!--tr bgcolor="white">
                <th align="left" colspan="4">
                    <b>
                        <u>Vehicle Data</u>
                    </b>
                </th>
            </tr-->
            <tr>
                <td align="left">Date:</td>
                <td>
                    <xsl:value-of select="//TestResult/MachineInitialize/Date"/>
                </td>
                <td align="left">Time:</td>
                <td>
                    <xsl:value-of select="//TestResult/MachineInitialize/Time"/>
                </td>
            </tr>
            <tr>
                <td align="left">VIN:</td>
                <td align="left">
                    <xsl:value-of select="//VIN"/>
                </td>
                <td align="left">Model:</td>
                <td align="left">
                    <xsl:value-of select="//VehicleType"/>
                </td>
            </tr>
            <tr>
                <td align="left">CCRT Version:</td>
                <td colspan="3">
                    <xsl:value-of select="//TestResult/MachineInitialize/Version"/>
                </td>
            </tr>
            <tr>
                <td align="left">Machine #:</td>
                <td align="left">
                    <xsl:value-of select="//TestResult/MachineInitialize/MachineNumber"/>
                </td>
                <td/>
                <td/>
            </tr>
        </table>
    </xsl:template>

    <!-- Test Results -->
    <xsl:template name="TestResults" match="/BepVehicleTestResult/TestResult">

        <br/>
        <table border="1" rules="all" title="Test Results" cellpadding="1">
            <xsl:call-template name="headerRow"/>

            <xsl:call-template name="brakeResults"/>

            <xsl:call-template name="airbagResults"/>
			
			<!--xsl:call-template name="steeringResults"/-->
			
			<xsl:call-template name="drivetrainResults"/>

            <xsl:call-template name="overallResult"/>

        </table>
    </xsl:template>
	
	<xsl:template name="drivetrainResults">
		<!-- Drivetrain Results -->
		<tr bgcolor="white">
			<th align="left" rowspan="1" colspan="7">DRIVETRAIN</th>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">Reverse Test</td>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//DriveTrainReverseTest/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">Load Test Cycle</td>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//DriveTrainVehicleCheckoutCycle/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
	</xsl:template>
	
	<xsl:template name="steeringResults">
		<!-- Steering Results -->
		<tr bgcolor="white">
			<th align="left" rowspan="1" colspan="7">EPAS</th>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">Learn VIN</td>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//PowerSteeringProgramVIN/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">CheckVIN</td>
			<td width="20" align="left">
				<xsl:value-of select="//PowerSteeringCheckVIN/ModuleVIN"/>
			</td>
			<td width="10"/>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//PowerSteeringCheckVIN/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
	</xsl:template>
	
	<xsl:template name="airbagResults">
		<!-- Airbag Results -->
		<tr bgcolor="white">
			<th align="left" rowspan="1" colspan="7">AIRBAG</th>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">Part #</td>
			<td width="20" align="left">
				<xsl:value-of select="//AirbagCheckModulePartNumber/ModulePartNumber"/>
			</td>
			<td width="10"/>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//AirbagCheckModulePartNumber/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">Serial #</td>
			<td width="20" align="left">
				<xsl:value-of select="//AirbagCollectTraceData/SerialNumber"/>
			</td>
			<td width="10"/>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//AirbagCollectTraceData/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">OEM Part #</td>
			<td width="20" align="left">
				<xsl:value-of select="//AirbagCheckOEMPartNumber/ModulePartNumber"/>
			</td>
			<td width="10"/>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//AirbagCheckOEMPartNumber/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">SW Ver #</td>
			<td width="20" align="left">
				<xsl:value-of select="//AirbagCheckFWChecksum/ModulePartNumber"/>
			</td>
			<td width="10"/>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//AirbagCheckFWChecksum/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">Program VIN</td>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//AirbagProgramVIN/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">Check VIN</td>
			<td width="20" align="left">
				<xsl:value-of select="//AirbagCheckVIN/ModuleVIN"/>
			</td>
			<td width="10"/>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//AirbagCheckVIN/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">Arm Airbag</td>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//AirbagEnableAirBagModule/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">DTC Check</td>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:call-template name="showResultMultiple">
					<xsl:with-param name="RESULT_1" select="//AirbagReadFaults/@Result"/>
                    <xsl:with-param name="RESULT_2" select="//OcsReadFaults/@Result"/>
				</xsl:call-template>
			</td>
		</tr>		
		<xsl:for-each select="//DTC/Airbag//@DTCCode">
			<tr>
				<td width="10"/>
				<td width="70" align="left">0x<xsl:value-of select=". "/> - <xsl:value-of select=".."/></td>
			</tr>
		</xsl:for-each>
        <xsl:for-each select="//DTC/Ocs//@DTCCode">
            <tr>
                <td width="10"/>
                <td width="70" align="left">0x<xsl:value-of select=". "/> - <xsl:value-of select=".."/></td>
            </tr>
        </xsl:for-each>
		
	</xsl:template>
	
	<xsl:template name="brakeResults">
		<!-- ======================================================================================== -->
		<!-- Drag Test Results -->
		<!-- ======================================================================================== -->
		<tr bgcolor="white">
			<th align="left" rowspan="1" colspan="7">BRAKE</th>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">LF Drag</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeLeftFrontDragTest/DragForce"/>
			</td>
			<td width="10" align="left"/>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeLeftFrontDragTest/MaxDragForce"/>
			</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeLeftFrontDragTest/DragForce/@Units"/>
			</td>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeLeftFrontDragTest/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">RF Drag</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeRightFrontDragTest/DragForce"/>
			</td>
			<td width="10" align="left"/>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeRightFrontDragTest/MaxDragForce"/>
			</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeRightFrontDragTest/DragForce/@Units"/>
			</td>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeRightFrontDragTest/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">LR Drag</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeLeftRearDragTest/DragForce"/>
			</td>
			<td width="10" align="left"/>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeLeftRearDragTest/MaxDragForce"/>
			</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeLeftRearDragTest/DragForce/@Units"/>
			</td>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeLeftRearDragTest/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">RR Drag</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeRightRearDragTest/DragForce"/>
			</td>
			<td width="10" align="left"/>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeRightRearDragTest/MaxDragForce"/>
			</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeRightRearDragTest/DragForce/@Units"/>
			</td>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeRightRearDragTest/@Result"/>
				</xsl:call-template>
			</td>
		</tr>

		<!-- ======================================================================================== -->
		<!-- Brake Test Results -->
		<!-- ======================================================================================== -->
		<tr>
			<td width="10"/>
			<td align="left" width="14">LF Force</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeLeftFrontForceTest/BrakeForce"/>
			</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeLeftFrontForceTest/MinBrakeForce"/>
			</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeLeftFrontForceTest/MaxBrakeForce"/>
			</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeLeftFrontForceTest/BrakeForce/@Units"/>
			</td>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeLeftFrontForceTest/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">RF Force</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeRightFrontForceTest/BrakeForce"/>
			</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeRightFrontForceTest/MinBrakeForce"/>
			</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeRightFrontForceTest/MaxBrakeForce"/>
			</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeRightFrontForceTest/BrakeForce/@Units"/>
			</td>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeRightFrontForceTest/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">LR Force</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeLeftRearForceTest/BrakeForce"/>
			</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeLeftRearForceTest/MinBrakeForce"/>
			</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeLeftRearForceTest/MaxBrakeForce"/>
			</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeLeftRearForceTest/BrakeForce/@Units"/>
			</td>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeLeftRearForceTest/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">RR Force</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeRightRearForceTest/BrakeForce"/>
			</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeRightRearForceTest/MinBrakeForce"/>
			</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeRightRearForceTest/MaxBrakeForce"/>
			</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeRightRearForceTest/BrakeForce/@Units"/>
			</td>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeRightRearForceTest/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">Front Bal</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeFrontBalanceTest/Value"/>
			</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeFrontBalanceTest/MinPercentBalance"/>
			</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeFrontBalanceTest/MaxPercentBalance"/>
			</td>
			<td width="10" align="left">
				<xsl:text>%</xsl:text>
			</td>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeFrontBalanceTest/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">Rear Bal</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeRearBalanceTest/Value"/>
			</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeRearBalanceTest/MinPercentBalance"/>
			</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeRearBalanceTest/MaxPercentBalance"/>
			</td>
			<td width="10" align="left">
				<xsl:text>%</xsl:text>
			</td>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeRearBalanceTest/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">Frt/Rr Bal</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeFrontToRearBalanceTest/Value"/>
			</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeFrontToRearBalanceTest/MinPercentBalance"/>
			</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeFrontToRearBalanceTest/MaxPercentBalance"/>
			</td>
			<td width="10" align="left">
				<xsl:text>%</xsl:text>
			</td>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeFrontToRearBalanceTest/@Result"/>
				</xsl:call-template>
			</td>
		</tr>

		<!-- ABS Test Results -->
		<tr>
			<td width="10"/>
			<td align="left" width="14">LF Reduction</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeLeftFrontABSReduction/LeftFrontABSReductionPercent"/>
			</td>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeLeftFrontABSReduction/LeftFrontReductionMinParameter"/>
			</td>
			<td width="10" align="left">
				<xsl:text>%</xsl:text>
			</td>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeLeftFrontABSReduction/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">RF Reduction</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeRightFrontABSReduction/RightFrontABSReductionPercent"/>
			</td>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeRightFrontABSReduction/RightFrontReductionMinParameter"/>
			</td>
			<td width="10" align="left">
				<xsl:text>%</xsl:text>
			</td>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeRightFrontABSReduction/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">LR Reduction</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeLeftRearABSReduction/LeftRearABSReductionPercent"/>
			</td>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeLeftRearABSReduction/LeftRearReductionMinParameter"/>
			</td>
			<td width="10" align="left">
				<xsl:text>%</xsl:text>
			</td>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeLeftRearABSReduction/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">RR Reduction</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeRightRearABSReduction/RightRearABSReductionPercent"/>
			</td>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeRightRearABSReduction/RightRearReductionMinParameter"/>
			</td>
			<td width="10" align="left">
				<xsl:text>%</xsl:text>
			</td>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeRightRearABSReduction/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">LF Recovery</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeLeftFrontABSRecovery/LeftFrontABSRecoveryValue"/>
			</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeLeftFrontABSRecovery/LeftFrontRecoveryMinPercent"/>
			</td>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:text>%</xsl:text>
			</td>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeLeftFrontABSRecovery/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">RF Recovery</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeRightFrontABSRecovery/RightFrontABSRecoveryValue"/>
			</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeRightFrontABSRecovery/RightFrontRecoveryMinPercent"/>
			</td>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:text>%</xsl:text>
			</td>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeRightFrontABSRecovery/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">LR Recovery</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeLeftRearABSRecovery/LeftRearABSRecoveryValue"/>
			</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeLeftRearABSRecovery/LeftRearRecoveryMinPercent"/>
			</td>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:text>%</xsl:text>
			</td>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeLeftRearABSRecovery/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">RR Recovery</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeRightRearABSRecovery/RightRearABSRecoveryValue"/>
			</td>
			<td width="10" align="left">
				<xsl:value-of select="//BrakeRightRearABSRecovery/RightRearRecoveryMinPercent"/>
			</td>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:text>%</xsl:text>
			</td>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeRightRearABSRecovery/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">LF Sensor</td>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeLFSensorTest/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">RF Sensor</td>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeRFSensorTest/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">LR Sensor</td>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeLRSensorTest/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">RR Sensor</td>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeRRSensorTest/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">WSS Quality</td>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeSensorQualityTest/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">ABS Valve Cross</td>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeABSValveCrossCheck/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">Brake Switch Test</td>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeBrakeSwitchTest/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">Motor Type</td>
			<td width="20" align="left">
				<xsl:value-of select="//BrakeLearnPerformanceType/PerformanceType"/>
			</td>
			<td width="10"/>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeLearnPerformanceType/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">Brake Burnish Cycle</td>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeBrakeBurnishCycle/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">Park Brake</td>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeElectricParkBrake/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">Part #</td>
			<td width="20" align="left">
				<xsl:value-of select="//BrakeCheckPartNumber/ModulePartNumber"/>
			</td>
			<td width="10"/>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeCheckPartNumber/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">Learn VIN</td>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeProgramVIN/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">Check VIN</td>
			<td width="20" align="left">
				<xsl:value-of select="//BrakeCheckVIN/ModuleVIN"/>
			</td>
			<td width="10"/>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeCheckVIN/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">DTC Check</td>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//BrakeReadFaults/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
		<xsl:for-each select="//DTC/Brake//@DTCCode">
			<tr>
				<td width="10"/>
				<td width="70" align="left">0x<xsl:value-of select=". "/> - <xsl:value-of select=".."/></td>
			</tr>
		</xsl:for-each>
	</xsl:template>

	<xsl:template name="overallResult">
		<!-- Overall Result -->
		<tr bgcolor="white">
			<th align="left" rowspan="1" colspan="7">OVERALL</th>
		</tr>
		<tr>
			<td width="10"/>
			<td align="left" width="14">Overall Result</td>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10"/>
			<td width="10" align="left">
				<xsl:call-template name="showResult">
					<xsl:with-param name="RESULT" select="//OverallTest/@Result"/>
				</xsl:call-template>
			</td>
		</tr>
	</xsl:template>
	
    <!-- Inserts a spacer row between sections -->
    <xsl:template name="sectionSpacer">
        <tr bgcolor="white">
            <th align="left" colspan="7" height="15"/>
        </tr>
    </xsl:template>

    <!-- Callout to allow cutomization of test result flags -->
    <xsl:template name="showResult">
        <xsl:param name="RESULT"/>
        <i>
            <xsl:choose>
                <xsl:when test="$RESULT != ''">
                    <xsl:value-of select="$RESULT"/>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:text/>
                </xsl:otherwise>
            </xsl:choose>
        </i>
    </xsl:template>

    <!-- Callout to allow cutomization of test result flags -->
    <xsl:template name="showResultMultiple">
        <xsl:param name="RESULT_1"/>
        <xsl:param name="RESULT_2"/>
        <i>
            <xsl:choose>
                <xsl:when test="$RESULT_1 != '' && $RESULT_2 != ''">
                    <xsl:choose>
                        <xsl:when test="$RESULT_1 != 'Pass' || $RESULT_2 != 'Pass'">
                            <xsl:text>Fail</xsl:text>
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:text>Pass</xsl:text>
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:choose>
                        <xsl:when test="$RESULT_1 != ''">
                            <xsl:value-of select="$RESULT_1"/>
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:value-of select="$RESULT_2"/>
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:otherwise>
            </xsl:choose>
        </i>
    </xsl:template>

    <!-- Converts argument 1 from LBF to KGF -->
    <xsl:template name="convertForce">
        <xsl:param name="FORCE"/>
        <xsl:param name="UNITS"/>
        <xsl:choose>
            <!-- The force value is a valid number -->
            <xsl:when test="string(number($FORCE)) != 'NaN'">
                <xsl:choose>
                    <!-- Check if units are lbf -->
                    <xsl:when test="translate($UNITS,'LBF','lbf')='lbf'">
                        <xsl:value-of select="format-number(number($FORCE)*0.45359237,'#.##')"/>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:value-of select="format-number(number($FORCE),'#.##')"/>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>----</xsl:text>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <!-- Adds together four brake forces and converts if necessary -->
    <xsl:template name="addForces">
        <xsl:param name="FORCE1"/>
        <xsl:param name="FORCE2"/>
        <xsl:param name="FORCE3"/>
        <xsl:param name="FORCE4"/>
        <xsl:param name="UNITS"/>
        <xsl:choose>
            <!-- The force value is a valid number -->
            <xsl:when test="string(number($FORCE1)) != 'NaN'">
                <xsl:choose>
                    <xsl:when test="string(number($FORCE2)) != 'NaN'">
                        <xsl:choose>
                            <xsl:when test="string(number($FORCE3)) != 'NaN'">
                                <xsl:choose>
                                    <xsl:when test="string(number($FORCE4)) != 'NaN'">
                                        <xsl:choose>
                                            <!-- Check if units are lbf -->
                                            <xsl:when test="translate($UNITS,'LBF','lbf')='lbf'">
                                                <xsl:value-of select="format-number((number($FORCE1) + number($FORCE2) + number($FORCE3) + number($FORCE4))*0.45359237,'#.##')"/>
                                            </xsl:when>
                                            <xsl:otherwise>
                                                <xsl:value-of select="format-number((number($FORCE1) + number($FORCE2) + number($FORCE3) + number($FORCE4)), '#.##')"/>
                                            </xsl:otherwise>
                                        </xsl:choose>
                                    </xsl:when>
                                    <xsl:otherwise>
                                        <xsl:text>----</xsl:text>
                                    </xsl:otherwise>
                                </xsl:choose>
                            </xsl:when>
                            <xsl:otherwise>
                                <xsl:text>----</xsl:text>
                            </xsl:otherwise>
                        </xsl:choose>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:text>----</xsl:text>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>----</xsl:text>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <!-- Converts argument 1 from MPH to KPH -->
    <xsl:template name="convertSpeed">
        <xsl:param name="SPEED"/>
        <xsl:param name="UNITS"/>
        <xsl:choose>
            <!-- The force value is a valid number -->
            <xsl:when test="string(number($SPEED)) != 'NaN'">
                <xsl:choose>
                    <!-- Check if units are mph -->
                    <xsl:when test="translate($UNITS,'MPH','mph')='mph'">
                        <xsl:value-of select="format-number(number($SPEED)*1.609343387125249,'#.##')"/>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:value-of select="format-number(number($SPEED),'#.##')"/>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>----</xsl:text>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <!-- Converts argument from inches x 10 to mm -->
    <xsl:template name="convertWheelbase">
        <xsl:param name="VALUE"/>
        <xsl:choose>
            <!-- The measured value must be a valid number -->
            <xsl:when test="string(number($VALUE)) != 'NaN'">
                <xsl:value-of select="format-number(number($VALUE)*2.54 ,'#.##')"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text> --</xsl:text>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <xsl:template name="headerRow">
        <tr>
            <td width="10" align="left"/>
            <td align="left" width="14"/>
            <td width="10" align="left">Measured</td>
            <td width="10" align="left">Min</td>
            <td width="10" align="left">Max</td>
            <td width="10" align="left">Units</td>
            <td width="10" align="left">Result</td>
        </tr>
    </xsl:template>

    <xsl:template name="fixedHeaderRow">
        <tr>
            <th align="left" width="50"/>
            <th align="left" width="175"/>
            <th align="left" width="50">Result</th>
            <th align="left" width="100">Measured</th>
            <th align="left" width="175"/>
            <th align="left" width="50">Result</th>
            <th align="left" width="100">Measured</th>
        </tr>
    </xsl:template>

</xsl:stylesheet>

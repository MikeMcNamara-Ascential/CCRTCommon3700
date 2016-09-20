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
            <u>Vehicle Inspection Report</u>
        </h3>
        <table border="1" rules="all" cellpadding="1">
            <tr>
                <td align="right">Date:</td>
                <td>
                    <xsl:value-of select="//TestResult/MachineInitialize/Date"/>
                </td>
                <td align="right">Time:</td>
                <td>
                    <xsl:value-of select="//TestResult/MachineInitialize/Time"/>
                </td>
            </tr>
            <tr>
                <td align="left">
                    <u>Overall Result</u>
                </td>
                <td align="left">
                    <xsl:value-of select="//TestResult/OverallTest/@Result"/>
                </td>
                <td align="left"/>
                <td align="left"/>
            </tr>
        </table>

        <!-- Vehicle Build Data -->
        <xsl:call-template name="sectionSpacer"/>
        <table border="1" rules="all" cellpadding="0" cellspacing="0" width="50%">
            <tr bgcolor="white">
                <th align="center" colspan="6">
                    <b>
                        <u>Vehicle Build Data</u>
                    </b>
                </th>
            </tr>
            <xsl:call-template name="categorySpacer"/>
            <tr>
                <td align="left">VIN</td>
                <td align="left">
                    <xsl:value-of select="//VIN"/>
                </td>
                <td align="left"> </td>
                <td align="left"> </td>
            </tr>
            <tr>
                <td align="left">ABS Part#</td>
                <td align="left">
                    <xsl:value-of select="//ABSPartNo"/>
                </td>
                <td align="left">Axle Type</td>
                <td align="left">
                    <xsl:value-of select="//Axle"/>
                </td>
            </tr>
            <tr>
                <td align="left">Brake Type</td>
                <td align="left">
                    <xsl:value-of select="//BrakeType"/>
                </td>
                <td align="left">Drive Axle</td>
                <td align="left">
                    <xsl:value-of select="//DriveAxle"/>
                </td>
            </tr>
            <tr>
                <td align="left">PCM Part#</td>
                <td align="left">
                    <xsl:value-of select="//PCMPartNo"/>
                </td>
                <td align="left">Speed Control</td>
                <td align="left">
                    <xsl:value-of select="//SpeedControl"/>
                </td>
            </tr>
            <tr>
                <td align="left">TCM Part#</td>
                <td align="left">
                    <xsl:value-of select="//TCMASMPartNo"/>
                </td>
                <td align="left">Vehicle Type</td>
                <td align="left">
                    <xsl:value-of select="//VehicleType"/>
                </td>
            </tr>
            <tr>
                <td align="left">Book Code</td>
                <td align="left">
                    <xsl:value-of select="//BookCode"/>
                </td>
                <td align="left">Lot#</td>
                <td align="left">
                    <xsl:value-of select="//LotNumber"/>
                </td>
            </tr>
            <tr>
                <td align="left">Model Year</td>
                <td align="left">
                    <xsl:value-of select="//ModelYear"/>
                </td>
                <td align="left"> </td>
                <td align="left"> </td>
            </tr>
        </table>
    </xsl:template>

    <!-- Test Results -->
    <xsl:template name="TestResults" match="/BepVehicleTestResult/TestResult">

        <h3/>
        <table border="1" rules="all" title="Test Results" cellpadding="1">

            <!-- ======================================================================================= -->
            <!-- Drag Test Results -->
            <!-- ======================================================================================= -->
            <xsl:call-template name="sectionSpacer"/>
            <xsl:call-template name="headerRow"/>
            <tr bgcolor="white">
                <th align="left" width="20">Drag Test</th>
                <td align="right">LF</td>
                <td align="center"/>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftFrontDragTest/DragForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftFrontDragTest/MaxDragForce"/>
                </td>
                <td align="center">LBF</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeLeftFrontDragTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr bgcolor="white">
                <th align="Right" width="20"/>
                <td align="right">RF</td>
                <td align="center"/>
                <td align="center">
                    <xsl:value-of select="//BrakeRightFrontDragTest/DragForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightFrontDragTest/MaxDragForce"/>
                </td>
                <td align="center">LBF</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRightFrontDragTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr bgcolor="white">
                <th align="left" width="20"/>
                <td align="right">LR</td>
                <td align="center"/>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftRearDragTest/DragForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftRearDragTest/MaxDragForce"/>
                </td>
                <td align="center">LBF</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeLeftRearDragTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr bgcolor="white">
                <th align="Right" width="20"/>
                <td align="right">RR</td>
                <td align="center"/>
                <td align="center">
                    <xsl:value-of select="//BrakeRightRearDragTest/DragForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightRearDragTest/MaxDragForce"/>
                </td>
                <td align="center">LBF</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRightRearDragTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>

            <!-- ======================================================================================= -->
            <!-- Brake Test Results -->
            <!-- ======================================================================================= -->
            <xsl:call-template name="sectionSpacer"/>
            <xsl:call-template name="headerRow"/>
            <tr bgcolor="white">
                <th align="left" width="20">Brake Test</th>
                <td align="right">LF</td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftFrontForceTest/MinBrakeForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftFrontForceTest/BrakeForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftFrontForceTest/MaxBrakeForce"/>
                </td>
                <td align="center">LBF</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeLeftFrontForceTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr bgcolor="white">
                <th align="left" width="20"/>
                <td align="right">RF</td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightFrontForceTest/MinBrakeForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightFrontForceTest/BrakeForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightFrontForceTest/MaxBrakeForce"/>
                </td>
                <td align="center">LBF</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRightFrontForceTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr bgcolor="white">
                <th align="left" width="20"/>
                <td align="right">LR</td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftRearForceTest/MinBrakeForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftRearForceTest/BrakeForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftRearForceTest/MaxBrakeForce"/>
                </td>
                <td align="center">LBF</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeLeftRearForceTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr bgcolor="white">
                <th align="left" width="20"/>
                <td align="right">RR</td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightRearForceTest/MinBrakeForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightRearForceTest/BrakeForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightRearForceTest/MaxBrakeForce"/>
                </td>
                <td align="center">LBF</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRightRearForceTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <!-- Balance Results -->
            <tr>
                <th align="left" width="20"/>
                <td align="right">Front Bal</td>
                <td align="center">
                    <xsl:value-of select="//BrakeFrontBalanceTest/MinPercentBalance"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeFrontBalanceTest/Value"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeFrontBalanceTest/MaxPercentBalance"/>
                </td>
                <td align="center">%</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeFrontBalanceTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="left" width="20"/>
                <td align="right">Rear Bal</td>
                <td align="center">
                    <xsl:value-of select="//BrakeRearBalanceTest/MinPercentBalance"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRearBalanceTest/Value"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRearBalanceTest/MaxPercentBalance"/>
                </td>
                <td align="center">%</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRearBalanceTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="left" width="20"/>
                <td align="right">Frt/Rr Bal</td>
                <td align="center">
                    <xsl:value-of select="//BrakeFrontToRearBalanceTest/MinPercentBalance"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeFrontToRearBalanceTest/Value"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeFrontToRearBalanceTest/MaxPercentBalance"/>
                </td>
                <td align="center">%</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeFrontToRearBalanceTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>


            <!-- ======================================================================================= -->
            <!-- ABS Test Results -->
            <!-- ======================================================================================= -->
            <xsl:call-template name="sectionSpacer"/>
            <xsl:call-template name="headerRow"/>
            <tr>
                <th align="left" width="20">Sensor Test</th>
                <td align="right">LF</td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftFrontSensorSpeedTest/MinLimit"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftFrontSensorSpeedTest/SensorSpeed"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftFrontSensorSpeedTest/MaxLimit"/>
                </td>
                <td align="center">MPH</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeLeftFrontSensorSpeedTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="Right" width="20"/>
                <td align="right">RF</td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightFrontSensorSpeedTest/MinLimit"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightFrontSensorSpeedTest/SensorSpeed"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightFrontSensorSpeedTest/MaxLimit"/>
                </td>
                <td align="center">MPH</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRightFrontSensorSpeedTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="left" width="20"/>
                <td align="right">LR</td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftRearSensorSpeedTest/MinLimit"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftRearSensorSpeedTest/SensorSpeed"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftRearSensorSpeedTest/MaxLimit"/>
                </td>
                <td align="center">MPH</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeLeftRearSensorSpeedTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="Right" width="20"/>
                <td align="right">RR</td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightRearSensorSpeedTest/MinLimit"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightRearSensorSpeedTest/SensorSpeed"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightRearSensorSpeedTest/MaxLimit"/>
                </td>
                <td align="center">MPH</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRightRearSensorSpeedTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <!-- Reduction Results -->
            <tr>
                <th align="left" width="20">Valve Cycle Test</th>
                <td align="right">Front</td>
                <td align="center"/>
                <td align="center">  
                </td>
                <td align="center">
                </td>
                <td align="center"></td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeFrontValveCycleTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="Right" width="20"/>
                <td align="right">Rear</td>
                <td align="center"/>
                <td align="center">
                 </td>
                <td align="center">
                </td>
                <td align="center"></td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRearValveCycleTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <!-- ======================================================================================= -->
            <!-- Transmission Test Results -->
            <!-- ======================================================================================= -->
            <xsl:call-template name="sectionSpacer"/>
            <tr>
                <th align="left" width="20">Transmission</th>
                <td/>
                <th align="center">Gear</th>
                <th align="center">Result</th>
                <td/>
                <th align="center">PRNDL</th>
                <th align="center">Result</th>
            </tr>
            <tr>
                <td width="20"/>
                <td/>
                <td/>
                <td/>
                <td/>
                <td align="center">P</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransmissionShiftLeverTest/PRNDL_P"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td width="20"/>
                <td/>
                <td align="center">Reverse</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransmissionReverseGearObserved/@Result"/>
                    </xsl:call-template>
                </td>
                <td/>
                <td align="center">R</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransmissionShiftLeverTest/PRNDL_R"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td width="20"/>
                <td/>
                <td align="center">Neutral</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransmissionNeutralGearObserved/@Result"/>
                    </xsl:call-template>
                </td>
                <td/>
                <td align="center">N</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransmissionShiftLeverTest/PRNDL_N"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td width="20"/>
                <td/>
                <td align="center">4</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransmissionFourthGearObserved/@Result"/>
                    </xsl:call-template>
                </td>
                <td/>
                <td align="center">D4</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransmissionShiftLeverTest/PRNDL_D4"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td width="20"/>
                <td/>
                <td align="center">3</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransmissionThirdGearObserved/@Result"/>
                    </xsl:call-template>
                </td>
                <td/>
                <td align="center">D6</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransmissionShiftLeverTest/PRNDL_D6"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td width="20"/>
                <td/>
                <td align="center">2</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransmissionSecondGearObserved/@Result"/>
                    </xsl:call-template>
                </td>
                <td/>
                <td align="center">2</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransmissionShiftLeverTest/PRNDL_2"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td width="20"/>
                <td/>
                <td align="center">1</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransmissionFirstGearObserved/@Result"/>
                    </xsl:call-template>
                </td>
                <td/>
                <td align="center">1</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransmissionShiftLeverTest/PRNDL_1"/>
                    </xsl:call-template>
                </td>
            </tr>
		<!--TCM Inspection:Check DTC-->
            <tr>
                <td width="20"/>
                <td/>
                <td align="center">Check DTC</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransmissionReadFaults/@Result"/>
                    </xsl:call-template>
                </td>
                <td align="center">
                </td>
				<td align="center">
                <td align="center">
                    <xsl:call-template name="showResult">
                    </xsl:call-template>
                </td>				
				</td>				
            </tr>
		<!-- TCM Inspection:Clear DTC-->
            <tr>
                <td width="20"/>
                <td/>
                <td align="center">Clear DTC</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransmissionClearFaults/@Result"/>
                    </xsl:call-template>
                </td>
                <td align="center">
                </td>
                <td align="center">
                </td>	
                <td align="center">
                    <xsl:call-template name="showResult">
                    </xsl:call-template>
                </td>				
            </tr>		
            <tr>
                <td width="20"/>
                <th align="left">TCC Slip</th>
                <th align="center">Minimum</th>
                <th align="center">Actual</th>
                <th align="center">Maximum</th>
                <th align="center">Units</th>
                <th align="center">Result</th>
            </tr>
            <tr>
                <td width="20"/>
                <td/>
                <td align="center">
                    <xsl:value-of select="//TransmissionEvaluateTccSlipError/MinimumTccSlipError"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//TransmissionEvaluateTccSlipError/ActualTccSlipError"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//TransmissionEvaluateTccSlipError/MaximumTccSlipError"/>
                </td>
                <td align="center">RPM</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransmissionEvaluateTccSlipError/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
			
			<!-- ======================================================================================= -->
            <!-- Report Side Slip Test Results -->
            <!-- ======================================================================================= -->

			<tr bgcolor="white">
                    <th align="left" width="20">Side Slip Test</th>
                    <td align="right">Front</td>
                    <td align="center">
                        <xsl:value-of select="//MachineReportSideSlipValue/SideSlipMinValue"/>
                    </td>
                    <td align="center">
                        <xsl:value-of select="//MachineReportSideSlipValue/SideSlipValue"/>
                    </td>
                    <td align="center">
                        <xsl:value-of select="//MachineReportSideSlipValue/SideSlipMaxValue"/>
                    </td>
                    <td align="center">m/km</td>
                    <td align="center">
                        <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//MachineReportSideSlipValue/@Result"/>
                        </xsl:call-template>
                    </td>
                </tr>

            <!-- ======================================================================================= -->
            <!-- Emissions Test Results -->
            <!-- ======================================================================================= -->
            <xsl:call-template name="sectionSpacer"/>
            <xsl:call-template name="emissionsHeaderRow"/>
            <tr>
                <th align="left" width="20">ECM</th>
                <td align="center">Idle RPM</td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckIdleRPM/MinParam"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckIdleRPM/SensorReading"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckIdleRPM/MaxParam"/>
                </td>
                <td align="center">RPM</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//EmissionsCheckIdleRPM/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td width="20"/>
                <td align="center">VSS</td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckVehicleSpeedSensor/MinSpeed"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckVehicleSpeedSensor/VehicleSpeedSensor"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckVehicleSpeedSensor/MaxSpeed"/>
                </td>
                <td align="center">MPH</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//EmissionsCheckVehicleSpeedSensor/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
			<!--ECM Check VIN number-->
			<tr>
				<td width="20"/>
				<td align="center">VIN</td>
				<td align="center">
					<xsl:value-of select="//EmissionsCheckVIN/ModuleVIN"/>
				</td>
                <td align="center">
                </td>
                <td align="center">
                </td>				
				<td align="center">
				</td>
				<td align="center">
					<xsl:call-template name="showResult">
						<xsl:with-param name="RESULT" select="//EmissionsCheckVIN/@Result"/>
					</xsl:call-template>
				</td>
			</tr>			

		<!-- Check DTC-->
			<tr>
				<td width="20"/>
				<td align="center">Check DTC</td>
				<td align="center">
					<xsl:value-of select="//EmissionsReadFaults/ModuleVIN"/>
				</td>
                <td align="center">
                </td>
                <td align="center">
                </td>				
				<td align="center">
				</td>
				<td align="center">
					<xsl:call-template name="showResult">
						<xsl:with-param name="RESULT" select="//EmissionsReadFaults/@Result"/>
					</xsl:call-template>
				</td>
			</tr>	
		<!-- Clear DTC-->
			<tr>
				<td width="20"/>
				<td align="center">Clear DTC</td>
				<td align="center">
					<xsl:value-of select="//EmissionsClearFaults/ModuleVIN"/>
				</td>
                <td align="center">
                </td>
                <td align="center">
                </td>				
				<td align="center">
				</td>
				<td align="center">
					<xsl:call-template name="showResult">
						<xsl:with-param name="RESULT" select="//EmissionsClearFaults/@Result"/>
					</xsl:call-template>
				</td>
			</tr>	
		<!-- Idling Parameter Inspection-->
			<tr>
				<td width="20"/>
				<td align="center">Idling Parameter Inspection</td>
				<td align="center">
					<xsl:value-of select="//EmissionsClearFaults/ModuleVIN"/>
				</td>
                <td align="center">
                </td>
                <td align="center">
                </td>				
				<td align="center">
				</td>
				<td align="center">
					<xsl:call-template name="showResult">
						<xsl:with-param name="RESULT" select="//EmissionsClearFaults/@Result"/>
					</xsl:call-template>
				</td>
			</tr>
			
        </table>
            
        <!-- DTC Table -->
        <xsl:if test="count(//TestResult/DTC/child::*)">
            <h3/>
            <table border="1" rules="all" title="DTC - Module Faults" width="50%">
                <tr bgcolor="white">
                    <th align="center" colspan="3">Module Faults</th>
                </tr>
                <xsl:call-template name="categorySpacer"/>
                <xsl:for-each select="//TestResult/DTC/child::*">
                    <tr bgcolor="white">
                        <th align="center">
                            <xsl:value-of select="name()"/>
                        </th>
                        <th align="center">Description</th>
                    </tr>
                    <xsl:for-each select="child::*">
                        <tr>
                            <td align="center">
                                <xsl:value-of select="@DTCCode"/>
                            </td>
                            <td align="center">
                                <xsl:value-of select="."/>
                            </td>
                        </tr>
                    </xsl:for-each>
                </xsl:for-each>
            </table>
        </xsl:if>

        <!-- List of all test failures -->
        <xsl:if test="count(//TestResult/AllFailures/child::*)">
            <h3/>
            <table border="1" rules="all" title="All Failures" width="50%">
                <tr bgcolor="white">
                    <th align="center" colspan="3">Test Failures</th>
                </tr>
                <xsl:call-template name="categorySpacer"/>
                <tr bgcolor="white">
                    <th align="center">Test</th>
                    <th align="center">Reason</th>
                </tr>
                <xsl:for-each select="//TestResult/AllFailures/child::*">
                    <tr>
                        <td align="left">
                            <xsl:value-of select="name()"/>
                        </td>
                        <td align="Left">
                            <xsl:value-of select="@Description"/>
                        </td>
                    </tr>
                    <xsl:for-each select="child::*">
                        <tr>
                            <td align="center">
                                   - <xsl:value-of select="name()"/>:
                            </td>
                            <td align="center">
                                 <xsl:value-of select="."/>
                            </td>
                        </tr>
                    </xsl:for-each>
                </xsl:for-each>
            </table>
        </xsl:if>

    </xsl:template>

    <!-- Inserts a spacer row between sections -->
    <xsl:template name="sectionSpacer">
        <tr bgcolor="white">
            <th align="left" colspan="7" height="15"/>
        </tr>
    </xsl:template>
    <xsl:template name="categorySpacer">
        <tr bgcolor="white">
            <th align="left" colspan="8" height="15">
                <xsl:text>_____________________________________________________________________</xsl:text>
            </th>
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

    <xsl:template name="headerRow">
        <tr>
            <th align="center" width="20"/>
            <th align="center">Wheel</th>
            <th align="center">Minimum</th>
            <th align="center">Measured</th>
            <th align="center">Maximum</th>
            <th align="center">Units</th>
            <th align="center">Result</th>
        </tr>
    </xsl:template>
    <xsl:template name="emissionsHeaderRow">
        <tr>
            <th align="center" width="20"/>
            <th align="center">Sensor</th>
            <th align="center">Minimum</th>
            <th align="center">Measured</th>
            <th align="center">Maximum</th>
            <th align="center">Units</th>
            <th align="center">Result</th>
        </tr>
    </xsl:template>

    <xsl:template name="fixedHeaderRow">
        <tr>
            <th align="center" width="50"/>
            <th align="center" width="175"/>
            <th align="center" width="50">Result</th>
            <th align="center" width="100">Measured</th>
            <th align="center" width="175"/>
            <th align="center" width="50">Result</th>
            <th align="center" width="100">Measured</th>
        </tr>
    </xsl:template>

</xsl:stylesheet>

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
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TestResult/OverallTest/@Result"/>
                    </xsl:call-template>
                </td>
                <td align="left"/>
                <td align="right"/>
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
                <td align="right">
                    <xsl:value-of select="//VIN"/>
                </td>
                <td width="17"></td>
                <td align="left"> </td>
                <td align="right"> </td>
            </tr>
            <tr>
                <td align="left">ABS Part#</td>
                <td align="right">
                    <xsl:value-of select="//ABSPartNo"/>
                </td>
                <td/>
                <td align="left">Axle Type</td>
                <td align="right">
                    <xsl:value-of select="//Axle"/>
                </td>
            </tr>
            <tr>
                <td align="left">Brake Type</td>
                <td align="right">
                    <xsl:value-of select="//BrakeType"/>
                </td>
                <td/>
                <td align="left">Drive Axle</td>
                <td align="right">
                    <xsl:value-of select="//DriveAxle"/>
                </td>
            </tr>
            <tr>
                <td align="left">PCM Part#</td>
                <td align="right">
                    <xsl:value-of select="//PCMPartNo"/>
                </td>
                <td/>
                <td align="left">Speed Control</td>
                <td align="right">
                    <xsl:value-of select="//SpeedControl"/>
                </td>
            </tr>
            <tr>
                <td align="left">TCM Part#</td>
                <td align="right">
                    <xsl:value-of select="//TCMPartNo"/>
                </td>
                <td/>
                <td align="left">Vehicle Type</td>
                <td align="right">
                    <xsl:value-of select="//VehicleType"/>
                </td>
            </tr>
            <tr>
                <td align="left">Book Code</td>
                <td align="right">
                    <xsl:value-of select="//BookCode"/>
                </td>
                <td/>
                <td align="left">Lot#</td>
                <td align="right">
                    <xsl:value-of select="//LotNumber"/>
                </td>
            </tr>
            <tr>
                <td align="left">Model Year</td>
                <td align="right">
                    <xsl:value-of select="//ModelYear"/>
                </td>
                <td/>
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
                <th align="left" width="20"/>
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
                    <xsl:value-of select="//BrakeLeftRearForceTest/MinBrakeForceWithMultiplier"/>
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
                    <xsl:value-of select="//BrakeRightRearForceTest/MinBrakeForceWithMultiplier"/>
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
            <!-- Brake Time Results -->
            <tr>
                <th align="left" width="20"></th>
                <td align="right">Front Brake Time</td>
                <td/>
                <td align="center">
                    <xsl:value-of select="//BrakeFrontBrakeTime/BrakeTime"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeFrontBrakeTime/MaxBrakeTime"/>
                </td>
                <td align="center">sec</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeFrontBrakeTime/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="left" width="20"/>
                <td align="right">Rear Brake Time</td>
                <td/>
                <td align="center">
                    <xsl:value-of select="//BrakeRearBrakeTime/BrakeTime"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRearBrakeTime/MaxBrakeTime"/>
                </td>
                <td align="center">sec</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRearBrakeTime/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
			<tr>
                <th align="left" width="20"></th>
                <td align="right">Park Brake Time</td>
                <td/>
                <td align="center">
                    <xsl:value-of select="//BrakeParkBrakeTime/ParkBrakeTime"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeParkBrakeTime/MaxParkBrakeTime"/>
                </td>
                <td align="center">sec</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeParkBrakeTime/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>

            <!-- Balance Results -->
            <tr>
                <th align="left" width="20"></th>
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
            <xsl:call-template name="sectionSpacer"/>
            <tr>
                <th align="left" width="20">Valve Cycle Test</th>
                <td align="right">Front</td>
                <td align="center" colspan="4"></td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeFrontValveCycleTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="Right" width="20"/>
                <td align="right">Rear</td>
                <td align="center" colspan="4"></td>
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
                <th align="center"></th>
                <th align="center"></th>
            </tr>
            <tr>
                <td width="20"/>
                <td/>
                <td align="center">Park</td>
                
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransmissionShiftLeverTest/PRNDL_P"/>
                    </xsl:call-template>
                </td>
                <td/>
                <td/>
                <td align="center"></td>
            </tr>
            <tr>
                <td width="20"/>
                <td/>
                <td align="center">Reverse</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransmissionShiftLeverTest/PRNDL_R"/>
                    </xsl:call-template>
                </td>
                <td/>
                <td align="center"></td>
                <td align="center">
                </td>
            </tr>
            <tr>
                <td width="20"/>
                <td/>
                <td align="center">Neutral</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransmissionShiftLeverTest/PRNDL_N"/>
                    </xsl:call-template>
                </td>
                <td/>
                <td align="center"></td>
                <td align="center">
                    
                </td>
            </tr>
            <tr>
                <td width="20"/>
                <td/>
                <td align="center">6</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransmissionSixthGearObserved/@Result"/>
                    </xsl:call-template>
                </td>
                <td/>
                <td align="center"></td>
                <td align="center">
                </td>
            </tr>
            <tr>
                <td width="20"/>
                <td/>
                <td align="center">5</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransmissionFifthGearObserved/@Result"/>
                    </xsl:call-template>
                </td>
                <td/>
                <td align="center"></td>
                <td align="center">
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
                <td align="center"></td>
                <td align="center">
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
                <td align="center"></td>
                <td align="center">
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
                <td align="center"></td>
                <td align="center">
                    
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
                <td align="center"></td>
                <td align="center">
                </td>
            </tr>
				
            <!--TCM Inspection:Check DTC-->
            <tr>
                <th align="left" width="20">TCM</th>
                <td align="right">Check DTC</td>
                <td align="center" colspan="4"></td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransmissionReadFaults/@Result"/>
                    </xsl:call-template>
                </td>				
            </tr>
		<!-- TCM Inspection:Clear DTC-->
            <tr>
                <td width="20"/>
                <td align="right">Clear DTC</td>
                <td align="center" colspan="4"></td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransmissionClearFaults/@Result"/>
                    </xsl:call-template>
                </td>				
            </tr>
			
			<!-- ======================================================================================= -->
            <!-- Report Side Slip Test Results -->
            <!-- ======================================================================================= -->
            <xsl:call-template name="sectionSpacer"/>
			<tr>
                <th align="center" width="20"/>
                <th align="center"></th>
                <th align="center">Minimum</th>
                <th align="center">Measured</th>
                <th align="center">Maximum</th>
                <th align="center">Units</th>
                <th align="center">Result</th>
            </tr>
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

            <!-- Speedometer Test Results -->
            <xsl:call-template name="sectionSpacer"/>
			<tr bgcolor="white">
                    <th align="left" width="20">Speedometer Test</th>
                    <td align="right"></td>
                    <td align="center">
                        <xsl:value-of select="//MachineSpeedometer/MinSpeedometerSpeed"/>
                    </td>
                    <td align="center">
                        <xsl:value-of select="//MachineSpeedometer/SpeedometerSpeed"/>
                    </td>
                    <td align="center">
                        <xsl:value-of select="//MachineSpeedometer/MaxSpeedometerSpeed"/>
                    </td>
                    <td align="center">MPH</td>
                    <td align="center">
                        <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//MachineSpeedometer/@Result"/>
                        </xsl:call-template>
                    </td>
                </tr>

            <!-- ======================================================================================= -->
            <!-- Emissions Test Results -->
            <!-- ======================================================================================= -->
            <xsl:call-template name="sectionSpacer"/>
            <xsl:call-template name="emissionsHeaderRow"/>
            <tr>
                <th align="left" width="10">ECM</th>
                <td align="right">Idle RPM</td>
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
                <th align="left" width="10"></th>
                <td align="right">Engine Coolant Temperature</td>
                <td align="center">
                    <xsl:value-of select="//EmissionsWaitForEngineTemperature/MinTemperature"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsWaitForEngineTemperature/EngineTemperature"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsWaitForEngineTemperature/MaxTemperature"/>
                </td>
                <td align="center">°C</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//EmissionsWaitForEngineTemperature/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="left" width="10"></th>
                <td align="right">Atmospheric Pressure</td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckBarometricPressure/MinParam"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckBarometricPressure/SensorReading"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckBarometricPressure/MaxParam"/>
                </td>
                <td align="center">kPa</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//EmissionsCheckBarometricPressure/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="left" width="10"></th>
                <td align="right">Battery Voltage</td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckBatteryVoltage/MinParam"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckBatteryVoltage/SensorReading"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckBatteryVoltage/MaxParam"/>
                </td>
                <td align="center">V</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//EmissionsCheckBatteryVoltage/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="left" width="10"></th>
                <td align="right">EGR Opening Target</td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckEGROpeningTarget/MinParam"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckEGROpeningTarget/SensorReading"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckEGROpeningTarget/MaxParam"/>
                </td>
                <td align="center">%</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//EmissionsCheckEGROpeningTarget/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="left" width="10"></th>
                <td align="right">EGR Position</td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckEGRPosition/MinParam"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckEGRPosition/SensorReading"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckEGRPosition/MaxParam"/>
                </td>
                <td align="center">%</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//EmissionsCheckEGRPosition/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="left" width="10"></th>
                <td align="right">Engine Oil Pressure</td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckEngineOilPressure/MinParam"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckEngineOilPressure/SensorReading"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckEngineOilPressure/MaxParam"/>
                </td>
                <td align="center">V</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//EmissionsCheckEngineOilPressure/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="left" width="10"></th>
                <td align="right">Final Injection Amount</td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckFinalInjectionAmount/MinParam"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckFinalInjectionAmount/SensorReading"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckFinalInjectionAmount/MaxParam"/>
                </td>
                <td align="center">mm3/st</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//EmissionsCheckFinalInjectionAmount/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="left" width="10"></th>
                <td align="right">Fuel Injection Correction 1</td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckFuelInjectionCorrection1/MinParam"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckFuelInjectionCorrection1/SensorReading"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckFuelInjectionCorrection1/MaxParam"/>
                </td>
                <td align="center">mm3/st</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//EmissionsCheckFuelInjectionCorrection1/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="left" width="10"></th>
                <td align="right">Fuel Injection Correction 2</td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckFuelInjectionCorrection2/MinParam"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckFuelInjectionCorrection2/SensorReading"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckFuelInjectionCorrection2/MaxParam"/>
                </td>
                <td align="center">mm3/st</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//EmissionsCheckFuelInjectionCorrection2/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="left" width="10"></th>
                <td align="right">Fuel Injection Correction 3</td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckFuelInjectionCorrection3/MinParam"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckFuelInjectionCorrection3/SensorReading"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckFuelInjectionCorrection3/MaxParam"/>
                </td>
                <td align="center">mm3/st</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//EmissionsCheckFuelInjectionCorrection3/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="left" width="10"></th>
                <td align="right">Fuel Injection Correction 4</td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckFuelInjectionCorrection4/MinParam"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckFuelInjectionCorrection4/SensorReading"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckFuelInjectionCorrection4/MaxParam"/>
                </td>
                <td align="center">mm3/st</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//EmissionsCheckFuelInjectionCorrection4/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="left" width="10"></th>
                <td align="right">Fuel Rail Pressure</td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckFuelRailPressure/MinParam"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckFuelRailPressure/SensorReading"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckFuelRailPressure/MaxParam"/>
                </td>
                <td align="center">kPa</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//EmissionsCheckFuelRailPressure/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="left" width="10"></th>
                <td align="right">Fuel Temperature</td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckFuelTemperature/MinParam"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckFuelTemperature/SensorReading"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckFuelTemperature/MaxParam"/>
                </td>
                <td align="center">°C</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//EmissionsCheckFuelTemperature/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="left" width="10"></th>
                <td align="right">Intake Air Flow</td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckIntakeAirFlow/MinParam"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckIntakeAirFlow/SensorReading"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckIntakeAirFlow/MaxParam"/>
                </td>
                <td align="center">g/sec</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//EmissionsCheckIntakeAirFlow/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="left" width="10"></th>
                <td align="right">Intake Air Temp</td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckIntakeAirTemp/MinParam"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckIntakeAirTemp/SensorReading"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckIntakeAirTemp/MaxParam"/>
                </td>
                <td align="center">°C</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//EmissionsCheckIntakeAirTemp/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="left" width="10"></th>
                <td align="right">Main Injection Timing</td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckMainInjectionTiming/MinParam"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckMainInjectionTiming/SensorReading"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckMainInjectionTiming/MaxParam"/>
                </td>
                <td align="center">CR</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//EmissionsCheckMainInjectionTiming/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="left" width="10"></th>
                <td align="right">Pump Learning Status</td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckPumpLearningStatus/MinParam"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckPumpLearningStatus/SensorReading"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckPumpLearningStatus/MaxParam"/>
                </td>
                <td align="center"></td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//EmissionsCheckPumpLearningStatus/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="left" width="10"></th>
                <td align="right">Pump Learning Value</td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckPumpLearningValue/MinParam"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckPumpLearningValue/SensorReading"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckPumpLearningValue/MaxParam"/>
                </td>
                <td align="center">mA</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//EmissionsCheckPumpLearningValue/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="left" width="10"></th>
                <td align="right">Requested RPM</td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckRequestedRPM/MinParam"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckRequestedRPM/SensorReading"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckRequestedRPM/MaxParam"/>
                </td>
                <td align="center">RPM</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//EmissionsCheckRequestedRPM/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="left" width="10"></th>
                <td align="right">SVC Current Value</td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckSVCCurrentValue/MinParam"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckSVCCurrentValue/SensorReading"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckSVCCurrentValue/MaxParam"/>
                </td>
                <td align="center">mA</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//EmissionsCheckSVCCurrentValue/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th align="left" width="10"></th>
                <td align="right">Turbo Boost Pressure</td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckTurboBoostPressure/MinParam"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckTurboBoostPressure/SensorReading"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//EmissionsCheckTurboBoostPressure/MaxParam"/>
                </td>
                <td align="center">kPa</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//EmissionsCheckTurboBoostPressure/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td width="10"/>
                <td align="right">VSS</td>
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
				<td width="25"/>
				<td align="right">VIN</td>
				<td align="center" colspan="3">
					<xsl:value-of select="//EmissionsCheckVIN/ModuleVIN"/>
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
				<td align="right">Check DTC</td>
                <td align="center" colspan="4"></td>
                <td align="center">
                    <xsl:call-template name="showResult">
						<xsl:with-param name="RESULT" select="//EmissionsReadFaults/@Result"/>
					</xsl:call-template>
                </td>
			</tr>	
		<!-- Clear DTC-->
			<tr>
				<td width="20"/>
				<td align="right">Clear DTC</td>
				<td align="center" colspan="4"></td>
				<td align="center">
					<xsl:call-template name="showResult">
						<xsl:with-param name="RESULT" select="//EmissionsClearFaults/@Result"/>
					</xsl:call-template>
				</td>
			</tr>	
		<!-- Idling Parameter Inspection-->
			<tr>
				<td width="20"/>
				<td align="right">Idling Parameter Inspection</td>
				<td align="center" colspan="4"></td>
				<td align="center">
                    <xsl:choose>
                        <xsl:when
                                  test="//EmissionsCheckBarometricPressure/@Result='Pass' and 
                                        //EmissionsCheckBatteryVoltage/@Result='Pass'and 
                                        //EmissionsCheckEGROpeningTarget/@Result='Pass'and 
                                        //EmissionsCheckEGRPosition/@Result='Pass'and 
                                        //EmissionsCheckEngineOilPressure/@Result='Pass'and 
                                        //EmissionsCheckFinalInjectionAmount/@Result='Pass'and 
                                        //EmissionsCheckFuelInjectionCorrection1/@Result='Pass'and 
                                        //EmissionsCheckFuelInjectionCorrection2/@Result='Pass'and 
                                        //EmissionsCheckFuelInjectionCorrection3/@Result='Pass'and 
                                        //EmissionsCheckFuelInjectionCorrection4/@Result='Pass'and 
                                        //EmissionsCheckFuelTemperature/@Result='Pass'and 
                                        //EmissionsCheckFuelRailPressure/@Result='Pass'and 
                                        //EmissionsCheckIntakeAirFlow/@Result='Pass'and 
                                        //EmissionsCheckMainInjectionTiming/@Result='Pass'and 
                                        //EmissionsCheckPumpLearningStatus/@Result='Pass'and 
                                        //EmissionsCheckPumpLearningValue/@Result='Pass'and 
                                        //EmissionsCheckRequestedRPM/@Result='Pass'and 
                                        //EmissionsCheckSVCCurrentValue/@Result='Pass'and 
                                        //EmissionsCheckTurboBoostPressure/@Result='Pass'and 
                                        //EmissionsDifferentialPressure/@Result='Pass'and 
                                        //EmissionsSetup/@Result='Pass'and 
                                        //EmissionsWaitForEngineRunning/@Result='Pass'">
                            <i><xsl:text>Pass</xsl:text></i>
                        </xsl:when>
                        <xsl:otherwise>
                            <i><xsl:text>Fail</xsl:text></i>
                        </xsl:otherwise>
                    </xsl:choose>
                </td>
			</tr>

            <!--MAF Learn-->
            <tr>
				<td width="20"/>
				<td align="right">MAF Learn</td>
				<td align="center" colspan="4"></td>
				<td align="center">
					<xsl:call-template name="showResult">
						<xsl:with-param name="RESULT" select="//EmissionsMAFLearn/@Result"/>
					</xsl:call-template>
				</td>
			</tr>

            <!-- DPD Switch Check-->
			<tr>
				<td width="20"/>
				<td align="right">DPD Switch Check</td>
                <td/>
                <td align="center">Off
                </td>
                <td align="center">
					<xsl:call-template name="showResult">
						<xsl:with-param name="RESULT" select="//EmissionsDPDSwitchCheck/DPDSwitchOff"/>
					</xsl:call-template>
                </td>
                <td align="center">On
				</td>
				<td align="center">
					<xsl:call-template name="showResult">
						<xsl:with-param name="RESULT" select="//EmissionsDPDSwitchCheck/DPDSwitchOn"/>
					</xsl:call-template>
				</td>
			</tr>
            <!-- MEC Lock-->
            <tr>
                <td width="20" ></td>
                <td align="right">MEC Lock</td>
                <td align="center" colspan="4"></td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//EmissionsLockModule/@Result"/>
                    </xsl:call-template>
                </td>                
            </tr>


            <!-- DCU Inspection-->
            <xsl:call-template name="sectionSpacer"/>
            <tr>
                <th width="20" align="left">DCU Inspection</th>
                <td align="right">Check DTC</td>				
                <td align="center" colspan="4"></td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//DCUReadFaults/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th width="20" align="left"></th>
                <td align="right">Clear DTC</td>				
                <td align="center" colspan="4"></td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//DCUClearFaultsFinal/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            

            <!-- MEC Lock-->
            <tr>
                <td width="20" ></td>
                <td align="right">MEC Lock</td>
                <td align="center" colspan="4"></td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//DCULockModule/@Result"/>
                    </xsl:call-template>
                </td>                
            </tr>

            <xsl:call-template name="sectionSpacer"/>
            <!--Mimamori Results-->
            <tr>
                <th width="20" align="left">Mimamori Inspection</th>
                <td align="right">Total Mileage</td>				
                <td align="center" colspan="4"></td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//MimamoriCheckTotalMileage/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            
            <tr>
                <th width="20" align="left"></th>
                <td align="right">Fuel Economy</td>				
                <td align="center" colspan="4"></td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//MimamoriCheckFuelEconomy/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th width="20" align="left"></th>
                <td align="right">Data Record Memory</td>				
                <td align="center" colspan="4"></td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//MimamoriCheckDataRecordMemory/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th width="20" align="left"></th>
                <td align="right">Check Engine Oil</td>				
                <td align="center" colspan="4"></td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//MimamoriCheckOilTemperature/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th width="20" align="left"></th>
                <td align="right">Check DTC</td>				
                <td align="center" colspan="4"></td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//MimamoriReadFaults/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th width="20" align="left"></th>
                <td align="right">Clear DTC</td>				
                <td align="center" colspan="4"></td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//MimamoriClearFaults/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <th width="20" align="left"></th>
                <td align="right">Clear History</td>				
                <td align="center" colspan="4"></td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//MimamoriClearHistory/@Result"/>
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
                        <th align="center" width="15">
                            <xsl:value-of select="name()"/>
                        </th>
                        <th align="center">Description</th>
                    </tr>
                    <xsl:for-each select="child::*">
                        <tr>
                            <td align="center" width="15">
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

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
            <tr bgcolor="white">
                <th align="center" colspan="4">
                    <b>
                        <u>Vehicle Data</u>
                    </b>
                </th>
            </tr>
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
                <td align="center">
                    <xsl:value-of select="//TestResult/MachineInitialize/MachineNumber"/>
                </td>
                <td/>
                <td/>
            </tr>
        </table>
    </xsl:template>

    <!-- Test Results -->
    <xsl:template name="TestResults" match="/BepVehicleTestResult/TestResult">

        <h3/>
        <table border="1" rules="all" title="Test Results" cellpadding="1">
            <xsl:call-template name="headerRow"/>

            <!-- ======================================================================================== -->
            <!-- Drag Test Results -->
            <!-- ======================================================================================== -->
            <tr bgcolor="white">
                <th align="center" rowspan="1" colspan="7">DRAG TEST</th>
            </tr>
            <tr>
                <td/>
                <td align="left" width="14">Left Front</td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftFrontDragTest/DragForce"/>
                </td>
                <td align="center"/>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftFrontDragTest/MaxDragForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftFrontDragTest/DragForce/@Units"/>
                </td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeLeftFrontDragTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">Right Front</td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightFrontDragTest/DragForce"/>
                </td>
                <td align="center"/>
                <td align="center">
                    <xsl:value-of select="//BrakeRightFrontDragTest/MaxDragForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightFrontDragTest/DragForce/@Units"/>
                </td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRightFrontDragTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">Left Rear</td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftRearDragTest/DragForce"/>
                </td>
                <td align="center"/>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftRearDragTest/MaxDragForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftRearDragTest/DragForce/@Units"/>
                </td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeLeftRearDragTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">Right Rear</td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightRearDragTest/DragForce"/>
                </td>
                <td align="center"/>
                <td align="center">
                    <xsl:value-of select="//BrakeRightRearDragTest/MaxDragForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightRearDragTest/DragForce/@Units"/>
                </td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRightRearDragTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>

            <!-- ======================================================================================== -->
            <!-- Brake Test Results -->
            <!-- ======================================================================================== -->
            <tr bgcolor="white">
                <th align="center" rowspan="1" colspan="7">BRAKE TEST</th>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">Left Front</td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftFrontForceTest/BrakeForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftFrontForceTest/MinBrakeForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftFrontForceTest/MaxBrakeForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftFrontForceTest/BrakeForce/@Units"/>
                </td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeLeftFrontForceTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">Right Front</td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightFrontForceTest/BrakeForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightFrontForceTest/MinBrakeForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightFrontForceTest/MaxBrakeForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightFrontForceTest/BrakeForce/@Units"/>
                </td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRightFrontForceTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">Left Rear</td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftRearForceTest/BrakeForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftRearForceTest/MinBrakeForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftRearForceTest/MaxBrakeForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftRearForceTest/BrakeForce/@Units"/>
                </td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeLeftRearForceTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">Right Rear</td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightRearForceTest/BrakeForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightRearForceTest/MinBrakeForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightRearForceTest/MaxBrakeForce"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightRearForceTest/BrakeForce/@Units"/>
                </td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRightRearForceTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">Front Bal</td>
                <td align="center">
                    <xsl:value-of select="//BrakeFrontBalanceTest/Value"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeFrontBalanceTest/MinPercentBalance"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeFrontBalanceTest/MaxPercentBalance"/>
                </td>
                <td align="center">
                    <xsl:text>%</xsl:text>
                </td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeFrontBalanceTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">Rear Bal</td>
                <td align="center">
                    <xsl:value-of select="//BrakeRearBalanceTest/Value"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRearBalanceTest/MinPercentBalance"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRearBalanceTest/MaxPercentBalance"/>
                </td>
                <td align="center">
                    <xsl:text>%</xsl:text>
                </td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRearBalanceTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">Frt/Rr Bal</td>
                <td align="center">
                    <xsl:value-of select="//BrakeFrontToRearBalanceTest/Value"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeFrontToRearBalanceTest/MinPercentBalance"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeFrontToRearBalanceTest/MaxPercentBalance"/>
                </td>
                <td align="center">
                    <xsl:text>%</xsl:text>
                </td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeFrontToRearBalanceTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">Park Brake</td>
                <td/>
                <td/>
                <td/>
                <td/>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeElectricParkBrake/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>

            <!-- ABS Test Results -->
            <tr bgcolor="white">
                <th align="center" rowspan="1" colspan="7">ABS TEST</th>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">LF Reduction</td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftFrontABSReduction/LeftFrontABSReductionPercent"/>
                </td>
                <td/>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftFrontABSReduction/LeftFrontReductionMinParameter"/>
                </td>
                <td align="center">
                    <xsl:text>%</xsl:text>
                </td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeLeftFrontABSReduction/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">RF Reduction</td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightFrontABSReduction/RightFrontABSReductionPercent"/>
                </td>
                <td/>
                <td align="center">
                    <xsl:value-of select="//BrakeRightFrontABSReduction/RightFrontReductionMinParameter"/>
                </td>
                <td align="center">
                    <xsl:text>%</xsl:text>
                </td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRightFrontABSReduction/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">LR Reduction</td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftRearABSReduction/LeftRearABSReductionPercent"/>
                </td>
                <td/>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftRearABSReduction/LeftRearReductionMinParameter"/>
                </td>
                <td align="center">
                    <xsl:text>%</xsl:text>
                </td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeLeftRearABSReduction/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">RR Reduction</td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightRearABSReduction/RightRearABSReductionPercent"/>
                </td>
                <td/>
                <td align="center">
                    <xsl:value-of select="//BrakeRightRearABSReduction/RightRearReductionMinParameter"/>
                </td>
                <td align="center">
                    <xsl:text>%</xsl:text>
                </td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRightRearABSReduction/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">LF Recovery</td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftFrontABSRecovery/LeftFrontABSRecoveryValue"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftFrontABSRecovery/LeftFrontRecoveryMinPercent"/>
                </td>
                <td/>
                <td align="center">
                    <xsl:text>%</xsl:text>
                </td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeLeftFrontABSRecovery/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">RF Recovery</td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightFrontABSRecovery/RightFrontABSRecoveryValue"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightFrontABSRecovery/RightFrontRecoveryMinPercent"/>
                </td>
                <td/>
                <td align="center">
                    <xsl:text>%</xsl:text>
                </td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRightFrontABSRecovery/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">LR Recovery</td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftRearABSRecovery/LeftRearABSRecoveryValue"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftRearABSRecovery/LeftRearRecoveryMinPercent"/>
                </td>
                <td/>
                <td align="center">
                    <xsl:text>%</xsl:text>
                </td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeLeftRearABSRecovery/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">RR Recovery</td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightRearABSRecovery/RightRearABSRecoveryValue"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightRearABSRecovery/RightRearRecoveryMinPercent"/>
                </td>
                <td/>
                <td align="center">
                    <xsl:text>%</xsl:text>
                </td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRightRearABSRecovery/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">LF Sensor</td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftFrontSensorSpeedTest/SensorSpeed"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftFrontSensorSpeedTest/MinLimit"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftFrontSensorSpeedTest/MaxLimit"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftFrontSensorSpeedTest/SensorSpeed/@Units"/>
                </td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeLeftFrontSensorSpeedTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">RF Sensor</td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightFrontSensorSpeedTest/SensorSpeed"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightFrontSensorSpeedTest/MinLimit"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightFrontSensorSpeedTest/MaxLimit"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightFrontSensorSpeedTest/SensorSpeed/@Units"/>
                </td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRightFrontSensorSpeedTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">LR Sensor</td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftRearSensorSpeedTest/SensorSpeed"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftRearSensorSpeedTest/MinLimit"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftRearSensorSpeedTest/MaxLimit"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeLeftRearSensorSpeedTest/SensorSpeed/@Units"/>
                </td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeLeftRearSensorSpeedTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">RR Sensor</td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightRearSensorSpeedTest/SensorSpeed"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightRearSensorSpeedTest/MinLimit"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightRearSensorSpeedTest/MaxLimit"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//BrakeRightRearSensorSpeedTest/SensorSpeed/@Units"/>
                </td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRightRearSensorSpeedTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>

            <!-- Cruise Control Result -->
            <!--tr bgcolor="white">
                <th align="center" rowspan="1" colspan="7">SPEED CONTROL</th>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">Result</td>
                <td/>
                <td/>
                <td/>
                <td/>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//SpeedControlOverallTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr-->

            <!-- Cruise Control Result -->
            <tr bgcolor="white">
                <th align="center" rowspan="1" colspan="7">AIRBAG</th>
            </tr>
            <!--tr>
                <td/>
                <td align="right" width="14">FAD Resistance</td>
                <td align="center">
                    <xsl:value-of select="//AirbagResistanceValueFAD/MeasuredValue"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//AirbagResistanceValueFAD/MinValue"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//AirbagResistanceValueFAD/MaxValue"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//AirbagResistanceValueFAD/MeasuredValue/@Units"/>
                </td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//AirbagResistanceValueFAD/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">BPD Resistance</td>
                <td align="center">
                    <xsl:value-of select="//AirbagResistanceValueBPD/MeasuredValue"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//AirbagResistanceValueBPD/MinValue"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//AirbagResistanceValueBPD/MaxValue"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//AirbagResistanceValueBPD/MeasuredValue/@Units"/>
                </td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//AirbagResistanceValueBPD/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">BPP Resistance</td>
                <td align="center">
                    <xsl:value-of select="//AirbagResistanceValueBPP/MeasuredValue"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//AirbagResistanceValueBPP/MinValue"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//AirbagResistanceValueBPP/MaxValue"/>
                </td>
                <td align="center">
                    <xsl:value-of select="//AirbagResistanceValueBPP/MeasuredValue/@Units"/>
                </td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//AirbagResistanceValueBPP/@Result"/>
                    </xsl:call-template>
                </td>
            </tr-->
            <tr>
                <td/>
                <td align="right" width="14">Part Number</td>
                <td/>
                <td/>
                <td/>
                <td/>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//AirbagCheckPartNumber/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">DTC Check</td>
                <td/>
                <td/>
                <td/>
                <td/>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//AirbagReadFaults/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>

            <!-- Overall Result -->
            <tr bgcolor="white">
                <th align="center" rowspan="1" colspan="7">OVERALL</th>
            </tr>
            <tr>
                <td/>
                <td align="right" width="14">Overall Result</td>
                <td/>
                <td/>
                <td/>
                <td/>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//OverallTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>

        </table>
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
            <td align="center"/>
            <td align="center" width="14"/>
            <td align="center">Measured</td>
            <td align="center">Min</td>
            <td align="center">Max</td>
            <td align="center">Units</td>
            <td align="center">Result</td>
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

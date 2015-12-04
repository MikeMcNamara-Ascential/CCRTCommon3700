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
            <u>Vehicle Test Report</u>
        </h3>

        <!-- Test Time, Date and Machine number -->
        <table border="1" rules="all" cellpadding="0" cellspacing="0" width="50%">
            <tr>
                <td align="left">
                    <u>Overall Result</u>
                </td>
                <td align="left">
                    <xsl:value-of select="//TestResult/OverallTest/@Result"/>
                </td>
                <td align="left">
                    <u>Test Date</u>
                </td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TestResult/MachineInitialize/Date"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td align="left">
                    <u>Machine</u>
                </td>
                <!--
                <td align="left">5</td>
                -->
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TestResult/MachineInitialize/MachineNumber"/>
                    </xsl:call-template>
                </td>
                <td align="left">
                    <u>Test Time</u>
                </td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TestResult/MachineInitialize/Time"/>
                    </xsl:call-template>
                </td>
            </tr>
        </table>
        <h3/>
    
        <!-- Vehicle Build Data -->
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
                <td align="left">Driver #</td>
                <td align="left">
                    <xsl:value-of select="//TestResult/MachineDriverNumber/DriverNumber"/>
                </td>
            </tr>
            <tr>
                <td align="left">PFS Status</td>
                <td align="left">
                    <xsl:value-of select="//PerformAbortTest"/>
                </td>
                <td align="left">Body Style</td>
                <td align="left">
                    <xsl:value-of select="//BodyStyle"/>
                </td>
            </tr>
            <tr>
                <td align="left">Transmission</td>
                <td align="left">
                    <xsl:value-of select="//Transmission"/>
                </td>
                <td align="left">Brake Type</td>
                <td align="left">
                    <xsl:value-of select="//BrakeType"/>
                </td>
            </tr>
            <tr>
                <td align="left">T-Case</td>
                <td align="left">
                    <xsl:value-of select="//TransferCase"/>
                </td>
                <td align="left">Axle Type</td>
                <td align="left">
                    <xsl:value-of select="//Axle"/>
                </td>
            </tr>
            <tr>
                <td align="left">Speed Control</td>
                <td align="left">
                    <xsl:value-of select="//SpeedControl"/>
                </td>
                <td align="left">SKREEM</td>
                <td align="left">
                    <xsl:value-of select="//SentryKeyImmobilizer"/>
                </td>
            </tr>
            <tr>
                <td align="left">Airbag</td>
                <td align="left">
                    <xsl:value-of select="//Airbag"/>
                </td>
                <td align="left">TPM Type</td>
                <td align="left">
                    <xsl:value-of select="//TirePressureMonitor"/>
                </td>
            </tr>
            <tr>
                <td align="left">Market Type</td>
                <td align="left">
                    <xsl:value-of select="//MarketType"/>
                </td>
                <td align="left">Radio</td>
                <td align="left">
                    <xsl:value-of select="//Suspension"/>
                </td>
            </tr>
            <tr>
                <td align="left">Engine Size</td>
                <td align="left">
                    <xsl:value-of select="//TestResult/EmissionsDetermineModuleConfig/EngineType"/>
                </td>
            </tr>
        </table>
    </xsl:template>

    <!-- Test Results -->
    <xsl:template name="TestResults" match="/BepVehicleTestResult/TestResult">

        <h3/>
        <table border="1" rules="all" title="Test Results" cellpadding="1" cellspacing="2" width="%50">
            <tr bgcolor="white">
                <th align="center" colspan="9">
                    <b>
                        <u>Test Results</u>
                    </b>
                </th>
            </tr>
            <xsl:call-template name="categorySpacer"/>

            <!-- Drag Test Results -->
            <xsl:if test="//BrakeAnalyzeDragTest/@Result!='Pass'">
                <tr bgcolor="white">
                    <td align="left">Drag Test</td>
                    <td>
                    </td>
                    <td align="left">
                        <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//BrakeAnalyzeDragTest/@Result"/>
                        </xsl:call-template>
                    </td>
                    <td>
                    </td>
                    <td>
                    </td>
                    <td>
                    </td>
                    <td>
                    </td>
                </tr>
                <tr>
                    <td align="left">LF</td>
                    <td align="left">
                        <xsl:call-template name="showTestNumber">
                            <xsl:with-param name="PARAM" select="//BrakeLeftFrontDragTest/DragForce"/>
                        </xsl:call-template>
                    </td>
                    <td align="left">
                        <xsl:text>[max:</xsl:text>
                        <xsl:call-template name="showTestParam">
                            <xsl:with-param name="PARAM" select="//BrakeLeftFrontDragTest/MaxDragForce"/>
                        </xsl:call-template>
                        <xsl:text>]</xsl:text>
                    </td>
                    <td align="left">RF</td>
                    <td align="left">
                        <xsl:call-template name="showTestNumber">
                            <xsl:with-param name="PARAM" select="//BrakeRightFrontDragTest/DragForce"/>
                        </xsl:call-template>
                    </td>
                    <td align="left">
                        <xsl:text>[max:</xsl:text>
                        <xsl:call-template name="showTestParam">
                            <xsl:with-param name="PARAM" select="//BrakeRightFrontDragTest/MaxDragForce"/>
                        </xsl:call-template>
                        <xsl:text>]</xsl:text>
                    </td>
                </tr>
                <tr>
                    <td align="left">LR</td>
                    <td align="left">
                        <xsl:call-template name="showTestNumber">
                            <xsl:with-param name="PARAM" select="//BrakeLeftRearDragTest/DragForce"/>
                        </xsl:call-template>
                    </td>
                    <td align="left">
                        <xsl:text>[max:</xsl:text>
                        <xsl:call-template name="showTestParam">
                            <xsl:with-param name="PARAM" select="//BrakeLeftRearDragTest/MaxDragForce"/>
                        </xsl:call-template>
                        <xsl:text>]</xsl:text>
                    </td>
                    <td align="left">RR</td>
                    <td align="left">
                        <xsl:call-template name="showTestNumber">
                            <xsl:with-param name="PARAM" select="//BrakeRightRearDragTest/DragForce"/>
                        </xsl:call-template>
                    </td>
                    <td align="left">
                        <xsl:text>[max:</xsl:text>
                        <xsl:call-template name="showTestParam">
                            <xsl:with-param name="PARAM" select="//BrakeRightRearDragTest/MaxDragForce"/>
                        </xsl:call-template>
                        <xsl:text>]</xsl:text>
                    </td>
                </tr>
            </xsl:if>

            <!-- Brake Test Results -->
            <xsl:if test="//BrakeAnalyzeBrakeTest/@Result!='Pass'">
                <xsl:call-template name="sectionSpacer"/>
                <tr bgcolor="white">
                    <td align="Left">Brake Test</td>
                    <td>
                    </td>
                    <td align="left">
                        <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//BrakeAnalyzeBrakeTest/@Result"/>
                        </xsl:call-template>
                    </td>
                    <td>
                    </td>
                    <td>
                    </td>
                    <td>
                    </td>
                    <td>
                    </td>
                </tr>
                <tr>
                    <td align="left">LF</td>
                    <td align="left">
                        <xsl:call-template name="showTestNumber">
                            <xsl:with-param name="PARAM" select="//BrakeLeftFrontForceTest/BrakeForce"/>
                        </xsl:call-template>
                    </td>
                    <td align="left">
                        <xsl:text>[</xsl:text>
                        <xsl:call-template name="showTestParams">
                            <xsl:with-param name="MIN_PARAM" select="//BrakeLeftFrontForceTest/MinBrakeForce"/>
                            <xsl:with-param name="MAX_PARAM" select="//BrakeLeftFrontForceTest/MaxBrakeForce"/>
                        </xsl:call-template>
                        <xsl:text>]</xsl:text>
                    </td>
                    <td align="left">Bal-F</td>
                    <td align="left">
                        <xsl:call-template name="showTestNumber">
                            <xsl:with-param name="PARAM" select="//BrakeFrontBalanceTest/Value"/>
                        </xsl:call-template>
                        <xsl:text>%</xsl:text>
                    </td>
                    <td align="left">
                        <xsl:text>[</xsl:text>
                        <xsl:call-template name="showTestParams">
                            <xsl:with-param name="MIN_PARAM" select="//BrakeFrontBalanceTest/MinPercentBalance"/>
                            <xsl:with-param name="MAX_PARAM" select="//BrakeFrontBalanceTest/MaxPercentBalance"/>
                        </xsl:call-template>
                        <xsl:text>%</xsl:text>
                        <xsl:text>]</xsl:text>
                    </td>
                </tr>
                <tr>
                    <td align="left">RF</td>
                    <td align="left">
                        <xsl:call-template name="showTestNumber">
                            <xsl:with-param name="PARAM" select="//BrakeRightFrontForceTest/BrakeForce"/>
                        </xsl:call-template>
                    </td>
                    <td align="left">
                        <xsl:text>[</xsl:text>
                        <xsl:call-template name="showTestParams">
                            <xsl:with-param name="MIN_PARAM" select="//BrakeRightFrontForceTest/MinBrakeForce"/>
                            <xsl:with-param name="MAX_PARAM" select="//BrakeRightFrontForceTest/MaxBrakeForce"/>
                        </xsl:call-template>
                        <xsl:text>]</xsl:text>
                    </td>
                    <td align="left">Bal-R</td>
                    <td align="left">
                        <xsl:call-template name="showTestNumber">
                            <xsl:with-param name="PARAM" select="//BrakeRearBalanceTest/Value"/>
                        </xsl:call-template>
                        <xsl:text>%</xsl:text>
                    </td>
                    <td align="left">
                        <xsl:text>[</xsl:text>
                        <xsl:call-template name="showTestParams">
                            <xsl:with-param name="MIN_PARAM" select="//BrakeRearBalanceTest/MinPercentBalance"/>
                            <xsl:with-param name="MAX_PARAM" select="//BrakeRearBalanceTest/MaxPercentBalance"/>
                        </xsl:call-template>
                        <xsl:text>%</xsl:text>
                        <xsl:text>]</xsl:text>
                    </td>
                </tr>
                <tr>
                    <td align="left">LR</td>
                    <td align="left">
                        <xsl:call-template name="showTestNumber">
                            <xsl:with-param name="PARAM" select="//BrakeLeftRearForceTest/BrakeForce"/>
                        </xsl:call-template>
                    </td>
                    <td align="left">
                        <xsl:text>[</xsl:text>
                        <xsl:call-template name="showTestParams">
                            <xsl:with-param name="MIN_PARAM" select="//BrakeLeftRearForceTest/MinBrakeForce"/>
                            <xsl:with-param name="MAX_PARAM" select="//BrakeLeftRearForceTest/MaxBrakeForce"/>
                        </xsl:call-template>
                        <xsl:text>]</xsl:text>
                    </td>
                    <td align="left">Bal-FR</td>
                    <td align="left">
                        <xsl:call-template name="showTestNumber">
                            <xsl:with-param name="PARAM" select="//BrakeFrontToRearBalanceTest/Value"/>
                        </xsl:call-template>
                        <xsl:text>%</xsl:text>
                    </td>
                    <td align="left" colspan="4">
                        <xsl:text>[</xsl:text>
                        <xsl:call-template name="showTestParams">
                            <xsl:with-param name="MIN_PARAM" select="//BrakeFrontToRearBalanceTest/MinPercentBalance"/>
                            <xsl:with-param name="MAX_PARAM" select="//BrakeFrontToRearBalanceTest/MaxPercentBalance"/>
                        </xsl:call-template>
                        <xsl:text>%</xsl:text>
                        <xsl:text>]</xsl:text>
                    </td>
                </tr>
                <tr>
                    <td align="left">RR</td>
                    <td align="left">
                        <xsl:call-template name="showTestNumber">
                            <xsl:with-param name="PARAM" select="//BrakeRightRearForceTest/BrakeForce"/>
                        </xsl:call-template>
                    </td>
                    <td align="left">
                        <xsl:text>[</xsl:text>
                        <xsl:call-template name="showTestParams">
                            <xsl:with-param name="MIN_PARAM" select="//BrakeRightRearForceTest/MinBrakeForce"/>
                            <xsl:with-param name="MAX_PARAM" select="//BrakeRightRearForceTest/MaxBrakeForce"/>
                        </xsl:call-template>
                        <xsl:text>]</xsl:text>
                    </td>
                    <td>
                    </td>
                    <td>
                    </td>
                    <td>
                    </td>
                </tr>
            </xsl:if>
    
            <!-- ABS Test Results -->
            <xsl:if test="//BrakeEvaluateABS/@Result!='Pass'">
                <xsl:call-template name="sectionSpacer"/>
                <tr bgcolor="white">
                    <th align="center" rowspan="6">ABS Test</th>
                </tr>
                <tr>
                    <td align="left">Recovery</td>
                    <td align="left" colspan="2">
                        <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//BrakeEvaluateABS/OverallRecovery"/>
                        </xsl:call-template>
                    </td>
                    <td align="left">Reduction</td>
                    <td align="left" colspan="2">
                        <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//BrakeEvaluateABS/OverallReduction"/>
                        </xsl:call-template>
                    </td>
                </tr>
                <tr>
                    <td align="left">LF</td>
                    <td align="left">
                        <xsl:call-template name="showTestNumber">
                            <xsl:with-param name="PARAM" select="//BrakeLeftFrontABSRecovery/LeftFrontABSRecoveryValue"/>
                        </xsl:call-template>
                        <xsl:text>%</xsl:text>
                    </td>
                    <td align="left">
                        <xsl:text>[</xsl:text>
                        <xsl:call-template name="showTestParam">
                            <xsl:with-param name="PARAM" select="//BrakeLeftFrontABSRecovery/LeftFrontRecoveryMinPercent"/>
                        </xsl:call-template>
                        <xsl:text>%</xsl:text>
                        <xsl:text>]</xsl:text>
                    </td>
                    <td align="left">LF</td>
                    <td align="left">
                        <xsl:call-template name="showTestNumber">
                            <xsl:with-param name="PARAM" select="//BrakeLeftFrontABSReduction/LeftFrontABSReductionPercent"/>
                        </xsl:call-template>
                        <xsl:text>%</xsl:text>
                    </td>
                    <td align="left">
                        <xsl:text>[</xsl:text>
                        <xsl:call-template name="showTestParam">
                            <xsl:with-param name="PARAM" select="//BrakeLeftFrontABSReduction/LeftFrontReductionMinParameter"/>
                        </xsl:call-template>
                        <xsl:text>%</xsl:text>
                        <xsl:text>]</xsl:text>
                    </td>
                </tr>
                <tr>
                    <td align="left">RF</td>
                    <td align="left">
                        <xsl:call-template name="showTestNumber">
                            <xsl:with-param name="PARAM" select="//BrakeRightFrontABSRecovery/RightFrontABSRecoveryValue"/>
                        </xsl:call-template>
                        <xsl:text>%</xsl:text>
                    </td>
                    <td align="left">
                        <xsl:text>[</xsl:text>
                        <xsl:call-template name="showTestParam">
                            <xsl:with-param name="PARAM" select="//BrakeRightFrontABSRecovery/RightFrontRecoveryMinPercent"/>
                        </xsl:call-template>
                        <xsl:text>%</xsl:text>
                        <xsl:text>]</xsl:text>
                    </td>
                    <td align="left">RF</td>
                    <td align="left">
                        <xsl:call-template name="showTestNumber">
                            <xsl:with-param name="PARAM" select="//BrakeRightFrontABSReduction/RightFrontABSReductionPercent"/>
                        </xsl:call-template>
                        <xsl:text>%</xsl:text>
                    </td>
                    <td align="left">
                        <xsl:text>[</xsl:text>
                        <xsl:call-template name="showTestParam">
                            <xsl:with-param name="PARAM" select="//BrakeRightFrontABSReduction/RightFrontReductionMinParameter"/>
                        </xsl:call-template>
                        <xsl:text>%</xsl:text>
                        <xsl:text>]</xsl:text>
                    </td>
                </tr>
                <tr>
                    <td align="left">LR</td>
                    <td align="left">
                        <xsl:call-template name="showTestNumber">
                            <xsl:with-param name="PARAM" select="//BrakeLeftRearABSRecovery/LeftRearABSRecoveryValue"/>
                        </xsl:call-template>
                        <xsl:text>%</xsl:text>
                    </td>
                    <td align="left">
                        <xsl:text>[</xsl:text>
                        <xsl:call-template name="showTestParam">
                            <xsl:with-param name="PARAM" select="//BrakeLeftRearABSRecovery/LeftRearRecoveryMinPercent"/>
                        </xsl:call-template>
                        <xsl:text>%</xsl:text>
                        <xsl:text>]</xsl:text>
                    </td>
                    <td align="left">LR</td>
                    <td align="left">
                        <xsl:call-template name="showTestNumber">
                            <xsl:with-param name="PARAM" select="//BrakeLeftRearABSReduction/LeftRearABSReductionPercent"/>
                        </xsl:call-template>
                        <xsl:text>%</xsl:text>
                    </td>
                    <td align="left">
                        <xsl:text>[</xsl:text>
                        <xsl:call-template name="showTestParam">
                            <xsl:with-param name="PARAM" select="//BrakeLeftRearABSReduction/LeftRearReductionMinParameter"/>
                        </xsl:call-template>
                        <xsl:text>%</xsl:text>
                        <xsl:text>]</xsl:text>
                    </td>
                </tr>
                <tr>
                    <td align="left">RR</td>
                    <td align="left">
                        <xsl:call-template name="showTestNumber">
                            <xsl:with-param name="PARAM" select="//BrakeRightRearABSRecovery/RightRearABSRecoveryValue"/>
                        </xsl:call-template>
                        <xsl:text>%</xsl:text>
                    </td>
                    <td align="left">
                        <xsl:text>[</xsl:text>
                        <xsl:call-template name="showTestParam">
                            <xsl:with-param name="PARAM" select="//BrakeRightRearABSRecovery/RightRearRecoveryMinPercent"/>
                        </xsl:call-template>
                        <xsl:text>%</xsl:text>
                        <xsl:text>]</xsl:text>
                    </td>
                    <td align="left">RR</td>
                    <td align="left">
                        <xsl:call-template name="showTestNumber">
                            <xsl:with-param name="PARAM" select="//BrakeRightRearABSReduction/RightRearABSReductionPercent"/>
                        </xsl:call-template>
                        <xsl:text>%</xsl:text>
                    </td>
                    <td align="left">
                        <xsl:text>[</xsl:text>
                        <xsl:call-template name="showTestParam">
                            <xsl:with-param name="PARAM" select="//BrakeRightRearABSReduction/RightRearReductionMinParameter"/>
                        </xsl:call-template>
                        <xsl:text>%</xsl:text>
                        <xsl:text>]</xsl:text>
                    </td>
                </tr>
            </xsl:if>

            <!-- ETC Test Results -->
            <xsl:if test="//VehicleBuild/SpeedControl = 'ETC'">

                <!-- ETC Zero Speed Results -->
                <xsl:if test="//ETCEvaluateZeroSpeed/@Result!='Pass'">
                    <xsl:call-template name="sectionSpacer"/>
                    <tr bgcolor="white">
                        <td align="Left" colspan="3">ETC Zero Speed Test</td>
                        <td align="left" colspan="2">Result</td>
                        <td align="left">
                            <xsl:call-template name="showResult">
                                <xsl:with-param name="RESULT" select="//ETCEvaluateZeroSpeed/@Result"/>
                            </xsl:call-template>
                        </td>
                    </tr>
                    <tr bgcolor="white">
                        <td align="left" colspan="2">Sensor Reading</td>
                        <td align="left">
                            <xsl:call-template name="showTestNumber">
                                <xsl:with-param name="PARAM" select="//ETCEvaluateZeroSpeedRear/AxleSensorReading"/>
                            </xsl:call-template>
                            <xsl:text>mph</xsl:text>
                        </td>
                        <td align="left" colspan="2">Range</td>
                        <td align="left">
                            <xsl:text>[</xsl:text>
                            <xsl:call-template name="showTestParams">
                                <xsl:with-param name="MIN_PARAM" select="//ETCEvaluateZeroSpeedRear/LowerLimit"/>
                                <xsl:with-param name="MAX_PARAM" select="//ETCEvaluateZeroSpeedRear/UpperLimit"/>
                            </xsl:call-template>
                            <xsl:text>]</xsl:text>
                        </td>
                    </tr>
                </xsl:if>

                <!-- ETC Mid Speed Results -->
                <xsl:if test="//ETCEvaluateMidSpeed/@Result!='Pass'">
                    <xsl:call-template name="sectionSpacer"/>
                    <tr bgcolor="white">
                        <td align="Left" colspan="3">ETC Mid Speed Test</td>
                        <td align="left" colspan="2">Result</td>
                        <td align="left">
                            <xsl:call-template name="showResult">
                                <xsl:with-param name="RESULT" select="//ETCEvaluateMidSpeed/@Result"/>
                            </xsl:call-template>
                        </td>
                    </tr>
                    <tr bgcolor="white">
                        <td align="left" colspan="2">Sensor Reading</td>
                        <td align="left">
                            <xsl:call-template name="showTestNumber">
                                <xsl:with-param name="PARAM" select="//ETCEvaluateMidSpeedRear/AxleSensorReading"/>
                            </xsl:call-template>
                            <xsl:text>mph</xsl:text>
                        </td>
                        <td align="left" colspan="2">Range</td>
                        <td align="left">
                            <xsl:text>[</xsl:text>
                            <xsl:call-template name="showTestParams">
                                <xsl:with-param name="MIN_PARAM" select="//ETCEvaluateMidSpeedRear/LowerLimit"/>
                                <xsl:with-param name="MAX_PARAM" select="//ETCEvaluateMidSpeedRear/UpperLimit"/>
                            </xsl:call-template>
                            <xsl:text>]</xsl:text>
                        </td>
                    </tr>
                </xsl:if>

                <!-- ETC High Speed Results -->
                <xsl:if test="//ETCEvaluateHighSpeed/@Result!='Pass'">
                    <xsl:call-template name="sectionSpacer"/>
                    <tr bgcolor="white">
                        <td align="Left" colspan="3">ETC High Speed Test</td>
                        <td align="left" colspan="2">Result</td>
                        <td align="left">
                            <xsl:call-template name="showResult">
                                <xsl:with-param name="RESULT" select="//ETCEvaluateHighSpeed/@Result"/>
                            </xsl:call-template>
                        </td>
                    </tr>
                    <tr bgcolor="white">
                        <td align="left" colspan="2">Sensor Reading</td>
                        <td align="left">
                            <xsl:call-template name="showTestNumber">
                                <xsl:with-param name="PARAM" select="//ETCEvaluateHighSpeedRear/AxleSensorReading"/>
                            </xsl:call-template>
                            <xsl:text>mph</xsl:text>
                        </td>
                        <td align="left" colspan="2">Range</td>
                        <td align="left">
                            <xsl:text>[</xsl:text>
                            <xsl:call-template name="showTestParams">
                                <xsl:with-param name="MIN_PARAM" select="//ETCEvaluateHighSpeedRear/LowerLimit"/>
                                <xsl:with-param name="MAX_PARAM" select="//ETCEvaluateHighSpeedRear/UpperLimit"/>
                            </xsl:call-template>
                            <xsl:text>]</xsl:text>
                        </td>
                    </tr>
                </xsl:if>
            </xsl:if>

            <!-- Part Number Results -->
            <xsl:call-template name="categorySpacer"/>
            <tr bgcolor="white">
                <td align="Left"/>
                <td align="left">Broadcast</td>
                <td align="Left"/>
                <td align="left">Vehicle</td>
                <td align="Left"/>
                <td align="Left">Result</td>
            </tr>
            <tr bgcolor="white">
                <td align="Left">PCM #</td>
                <xsl:if test="//VehicleBuild/MarketType = 'GPEC2'">
                    <td align="left">
                        <xsl:value-of select="//TestResult/EmissionsCheckPartNumber/BroadcastPartNumber"/>
                    </td>
                    <td align="Left"/>
                    <td align="left">
                        <xsl:value-of select="//TestResult/EmissionsCheckPartNumber/ModulePartNumber"/>
                    </td>
                    <td align="Left"/>
                    <td align="left">
                        <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//TestResult/EmissionsCheckPartNumber/@Result"/>
                        </xsl:call-template>
                    </td>
                </xsl:if>
                <xsl:if test="//VehicleBuild/MarketType = 'GPEC3'">
                    <td align="left">
                        <xsl:value-of select="//TestResult/EmissionsCheckPartNumber/BroadcastPartNumber"/>
                    </td>
                    <td align="Left"/>
                    <td align="left">
                        <xsl:value-of select="//TestResult/EmissionsCheckPartNumber/ModulePartNumber"/>
                    </td>
                    <td align="Left"/>
                    <td align="left">
                        <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//TestResult/EmissionsCheckPartNumber/@Result"/>
                        </xsl:call-template>
                    </td>
                </xsl:if>
                <xsl:if test="//VehicleBuild/MarketType = 'DIESEL'">
                    <td align="left">
                        <xsl:value-of select="//TestResult/DieselEmissionsCheckPartNumber/BroadcastPartNumber"/>
                    </td>
                    <td align="Left"/>
                    <td align="left">
                        <xsl:value-of select="//TestResult/DieselEmissionsCheckPartNumber/ModulePartNumber"/>
                    </td>
                    <td align="Left"/>
                    <td align="left">
                        <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//TestResult/DieselEmissionsCheckPartNumber/@Result"/>
                        </xsl:call-template>
                    </td>
                </xsl:if>
                <xsl:if test="//VehicleBuild/MarketType = 'DIESEL UDS'">
                    <td align="left">
                        <xsl:value-of select="//TestResult/DieselEmissionsCheckPartNumber/BroadcastPartNumber"/>
                    </td>
                    <td align="Left"/>
                    <td align="left">
                        <xsl:value-of select="//TestResult/DieselEmissionsCheckPartNumber/ModulePartNumber"/>
                    </td>
                    <td align="Left"/>
                    <td align="left">
                        <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//TestResult/DieselEmissionsCheckPartNumber/@Result"/>
                        </xsl:call-template>
                    </td>
                </xsl:if>
            </tr>
            <tr bgcolor="white">
                <td align="Left">ABS #</td>
                <td align="left">
                    <xsl:value-of select="//TestResult/BrakeCheckPartNumber/BroadcastPartNumber"/>
                </td>
                <td align="Left"/>
                <td align="left">
                    <xsl:value-of select="//TestResult/BrakeCheckPartNumber/ModulePartNumber"/>
                </td>
                <td align="Left"/>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TestResult/BrakeCheckPartNumber/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <xsl:if test="//VehicleBuild/Transmission = '948TE'">
                <tr bgcolor="white">
                    <td align="Left">TCM #</td>
                    <td align="left">
                        <xsl:value-of select="//TestResult/TransmissionCheckPartNumber/BroadcastPartNumber"/>
                    </td>
                    <td align="Left"/>
                    <td align="left">
                        <xsl:value-of select="//TestResult/TransmissionCheckPartNumber/ModulePartNumber"/>
                    </td>
                    <td align="Left"/>
                    <td align="left">
                        <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//TestResult/TransmissionCheckPartNumber/@Result"/>
                        </xsl:call-template>
                    </td>
                </tr>
            </xsl:if>
            <!--Shifter module -->
            <xsl:if test="//VehicleBuild/Transmission = '948TE'">
                <tr bgcolor="white">
                    <td align="Left">GSM #</td>
                    <td align="left">
                        <xsl:value-of select="//TestResult/TransmissionReadShifterModulePartNumber/BroadcastPartNumber"/>
                    </td>
                    <td align="Left"/>
                    <td align="left">
                        <xsl:value-of select="//TestResult/TransmissionReadShifterModulePartNumber/ReadPartNumber"/>
                    </td>
                    <td align="Left"/>
                    <td align="left">
                        <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//TestResult/TransmissionReadShifterModulePartNumber/@Result"/>
                        </xsl:call-template>
                    </td>
                </tr>
            </xsl:if>
            <!--Airbag module -->
            <xsl:if test="//VehicleBuild/Airbag != 'NotEquipped' ">
                <tr bgcolor="white">
                    <td align="Left">ORC #</td>
                    <td align="left">
                        <xsl:value-of select="//TestResult/AirbagCheckPartNumber/BroadcastPartNumber"/>
                    </td>
                    <td align="Left"/>
                    <td align="left">
                        <xsl:value-of select="//TestResult/AirbagCheckPartNumber/ModulePartNumber"/>
                    </td>
                    <td align="Left"/>
                    <td align="left">
                        <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//TestResult/AirbagCheckPartNumber/@Result"/>
                        </xsl:call-template>
                    </td>
                </tr>
            </xsl:if>


            <!-- Overall Test Results -->
            <xsl:call-template name="categorySpacer"/>
            <tr bgcolor="white">
                <td align="Left">Overall</td>
                <td align="left">
                    <xsl:value-of select="//TestResult/OverallTest/@Result"/>
                </td>
                <td>
                </td>
                <td align="left">TPM</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TPMOverallTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td align="left">Brake</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeOverallTest/@Result"/>
                    </xsl:call-template>
                </td>
                <td>
                </td>
                <td align="left">RFHM</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//RFHMOverallTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td align="left">Emissions</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//EmissionsOverallTest/@Result"/>
                    </xsl:call-template>
                </td>
                <td>
                </td>
                <td align="left">Parktronics</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//ParkAssistOverallTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td align="left">Transmission</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransmissionOverallTest/@Result"/>
                    </xsl:call-template>
                </td>
                <td>
                </td>
                <td align="left">T-case</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransferCaseOverallTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td align="left">Cruise</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//SpeedControlOverallTest/@Result"/>
                    </xsl:call-template>
                </td>
                <td>
                </td>
                <td align="left">EPB</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//EpbOverallTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td align="left">ETC</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//ETCOverallTest/@Result"/>
                    </xsl:call-template>
                </td>
                <td>
                </td>
                <td align="left">Cluster</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//ClusterOverallTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td align="left">Airbag</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//AirbagOverallTest/@Result"/>
                    </xsl:call-template>
                </td>
                <td>
                </td>
                <td align="left">OCS</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//OcsOverallTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td align="left">OCM</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//OCMOverallTest/@Result"/>
                    </xsl:call-template>
                </td>
                <td>
                </td>
                <td align="left">Gateway</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//GatewayOverallTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td align="left">Steering Sensor</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//SASOverallTest/@Result"/>
                    </xsl:call-template>
                </td>
                <td>
                </td>
                <td align="left">Fault Check</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//ModuleFaultReaderOverallTest/@Result"/>
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
                </xsl:for-each>
            </table>
        </xsl:if>
    </xsl:template>

    <!-- Inserts a spacer row between sections -->
    <xsl:template name="sectionSpacer">
        <tr bgcolor="white">
            <th align="left" colspan="8" height="15"/>
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
        <xsl:choose>
            <xsl:when test="$RESULT = 'TestNotStarted'">
                <xsl:text>--</xsl:text>
            </xsl:when>
            <xsl:when test="$RESULT = 'TestNotRequired'">
                <xsl:text>Not Required</xsl:text>
            </xsl:when>
            <xsl:when test="$RESULT = 'Yes'">
                <xsl:text>Pass</xsl:text>
            </xsl:when>
            <xsl:when test="$RESULT = 'No'">
                <xsl:text>Fail</xsl:text>
            </xsl:when>
            <xsl:when test="$RESULT != ''">
                <xsl:value-of select="$RESULT"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>--</xsl:text>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <!-- Callout to format the min and max test parameters -->
    <xsl:template name="showTestParams">
        <xsl:param name="MIN_PARAM"/>
        <xsl:param name="MAX_PARAM"/>
        <xsl:choose>
            <xsl:when test="$MIN_PARAM != ''">
                <xsl:value-of select="format-number(number($MIN_PARAM),'###')"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>---</xsl:text>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:text> -</xsl:text>
        <xsl:choose>
            <xsl:when test="$MAX_PARAM != ''">
                <xsl:value-of select="format-number(number($MAX_PARAM),'###')"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>---</xsl:text>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <!-- Callout to format the test parameters -->
    <xsl:template name="showTestParam">
        <xsl:param name="PARAM"/>
        <xsl:choose>
            <xsl:when test="$PARAM != ''">
                <xsl:value-of select="format-number(number($PARAM),'###')"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>---</xsl:text>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <!-- Callout to format the test number -->
    <xsl:template name="showTestNumber">
        <xsl:param name="PARAM"/>
        <xsl:choose>
            <xsl:when test="$PARAM != ''">
                <xsl:value-of select="format-number(number($PARAM),'###0.##')"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>---</xsl:text>
            </xsl:otherwise>
        </xsl:choose>
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
            <th align="center"/>
            <th align="center"/>
            <th align="center">Result</th>
            <th align="center">Measured</th>
            <th align="center"/>
            <th align="center">Result</th>
            <th align="center">Measured</th>
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

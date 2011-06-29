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
    <xsl:variable name="columnWidth" select="10"/>
    <!-- Build Data -->
    <xsl:template name="BuildData" match="/BepVehicleTestResult/VehicleBuild">
        <table border="1" rules="all" cellpadding="1">
            <tr>
                <td align="left">Build Data:</td>
            </tr>
            <tr>
                <td width="7" align="left">Date:</td>
                <td align="left"><xsl:value-of select="//TestResult/MachineInitialize/Date"/></td>
            </tr>
            <tr>
                <td width="7" align="left">Time:</td>
                <td align="left"><xsl:value-of select="//TestResult/MachineInitialize/Time"/></td>
            </tr>
            <tr>
                <td width="7" align="left">VIN #:</td>
                <td align="left"><xsl:value-of select="//VehicleBuild/VIN"/></td>
            </tr>
            <tr>
                <td width="7" align="left">ZTS #:</td>
                <td align="left"><xsl:value-of select="//VehicleBuild/ZTSCode"/></td>
            </tr>
        </table>
    </xsl:template>

    <!-- Test Results -->
    <xsl:template name="TestResults" match="/BepVehicleTestResult/TestResult">
        <table border="1" rules="all" title="Test Results" cellpadding="1">
            <!-- Drag Test Results -->
            <xsl:call-template name="sectionSpacer"/>
            <tr>
                <td align="left">Test Results:</td>
            </tr>
            <tr>
                <td width="4" align="left"/>
                <td width="7" align="left">Low:</td>
                <td width="7" align="left">High:</td>
                <td width="7" align="left">Value:</td>
                <td width="7" align="left">Result:</td>
                <!-- xsl:call-template name="fixedHeaderRow"/ -->
            </tr>
            <tr>
                <td align="left">Drags: KGF</td>
            </tr>
            <tr>
                <td align="left">
                    <table border="0">
                        <tr>
                            <td width="4">LF:</td>
                            <td width="7">----</td>
                            <td width="7">
                                <xsl:call-template name="convertForce">
                                    <xsl:with-param name="FORCE" select="//BrakeLeftFrontDragTest/MaxDragForce"/>
                                    <xsl:with-param name="UNITS" select="//BrakeLeftFrontDragTest/MaxDragForce/@Units"/>
                                </xsl:call-template>
                            </td>
                            <td width="7">
                                <xsl:call-template name="convertForce">
                                    <xsl:with-param name="FORCE" select="//BrakeLeftFrontDragTest/DragForce"/>
                                    <xsl:with-param name="UNITS" select="//BrakeLeftFrontDragTest/DragForce/@Units"/>
                                </xsl:call-template>
                            </td>
                            <td width="7">
                                <xsl:call-template name="showResult">
                                    <xsl:with-param name="RESULT" select="//BrakeLeftFrontDragTest/@Result"/>
                                </xsl:call-template>
                            </td>
                        </tr>
                    </table>
                </td>
            </tr>

            <tr>
                <td width="4" align="left">RF:</td>
                <td width="7" align="left">----</td>
                <td width="7" align="left">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightFrontDragTest/MaxDragForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightFrontDragTest/MaxDragForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td width="7" align="left">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightFrontDragTest/DragForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightFrontDragTest/DragForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td width="7">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRightFrontDragTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td width="4" align="left">LR:</td>
                <td width="7" align="left">----</td>
                <td width="7" align="left">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftRearDragTest/MaxDragForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftRearDragTest/MaxDragForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td  width="7" align="left">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftRearDragTest/DragForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftRearDragTest/DragForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td>
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeLeftRearDragTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td width="4" align="left">RR:</td>
                <td  width="7" align="left">----</td>
                <td  width="7" align="left">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightRearDragTest/MaxDragForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightRearDragTest/MaxDragForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td  width="7" align="left">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightRearDragTest/DragForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightRearDragTest/DragForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td>
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRightRearDragTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>

            <!-- Brake Test Results -->
            <tr>
                <td align="left">Brake: KGF</td>
            </tr>
            <tr>
                <td  width="4" align="left">LF:</td>
                <td  width="7" align="left">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftFrontForceTest/MinBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftFrontForceTest/BrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td  width="7" align="left">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftFrontForceTest/MaxBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftFrontForceTest/BrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td  width="7" align="left">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftFrontForceTest/BrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftFrontForceTest/BrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td>
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeLeftFrontForceTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td width="4" align="left">RF:</td>
                <td  width="7" align="left">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightFrontForceTest/MinBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightFrontForceTest/BrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td  width="7" align="left">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightFrontForceTest/MaxBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightFrontForceTest/BrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td  width="7" align="left">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightFrontForceTest/BrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightFrontForceTest/BrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td>
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRightFrontForceTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td width="4" align="left">LR:</td>
                <td  width="7" align="left">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftRearForceTest/MinBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftRearForceTest/BrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td  width="7" align="left">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftRearForceTest/MaxBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftRearForceTest/BrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td  width="7" align="left">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftRearForceTest/BrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftRearForceTest/BrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td>
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeLeftRearForceTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td width="4" align="left">RR:</td>
                <td  width="7" align="left">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightRearForceTest/MinBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightRearForceTest/BrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td  width="7" align="left">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightRearForceTest/MaxBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightRearForceTest/BrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td  width="7" align="left">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightRearForceTest/BrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightRearForceTest/BrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td>
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRightRearForceTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>

            <!-- Brake Balance Results -->
            <tr>
                <td  width="7" align="left">Balance: %</td>
            </tr>
            <tr>
                <td width="4" align="left">F:</td>
                <td  width="7" align="left">
                    <xsl:value-of select="//BrakeFrontBalanceTest/MinPercentBalance"/>
                </td>
                <td  width="7" align="left">
                    <xsl:value-of select="//BrakeFrontBalanceTest/MaxPercentBalance"/>
                </td>
                <td  width="7" align="left">
                    <xsl:value-of select="//BrakeFrontBalanceTest/Value"/>
                </td>
                <td>
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeFrontBalanceTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td width="4" align="left">R:</td>
                <td  width="7" align="left">
                    <xsl:value-of select="//BrakeRearBalanceTest/MinPercentBalance"/>
                </td>
                <td  width="7" align="left">
                    <xsl:value-of select="//BrakeRearBalanceTest/MaxPercentBalance"/>
                </td>
                <td  width="7" align="left">
                    <xsl:value-of select="//BrakeRearBalanceTest/Value"/>
                </td>
                <td>
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRearBalanceTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td width="4" align="left">F/R:</td>
                <td  width="7" align="left">
                    <xsl:value-of select="//BrakeFrontToRearBalanceTest/MinPercentBalance"/>
                </td>
                <td  width="7" align="left">
                    <xsl:value-of select="//BrakeFrontToRearBalanceTest/MaxPercentBalance"/>
                </td>
                <td  width="7" align="left">
                    <xsl:value-of select="//BrakeFrontToRearBalanceTest/Value"/>
                </td>
                <td>
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRearBalanceTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
	
            <!-- ABS Reduction Results -->
            <tr>
                <td  width="7" align="left">Dump: %</td>
            </tr>
            <tr>
                <td width="4" align="left">LF:</td>
                <td  width="7" align="left">
                    <xsl:value-of select="//BrakeLeftFrontABSReduction/LeftFrontReductionMinParameter"/>
                </td>
                <td  width="7" align="left">----</td>
                <td  width="7" align="left">
                    <xsl:value-of select="//BrakeLeftFrontABSReduction/LeftFrontABSReductionPercent"/>
                </td>
                <td>
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeLeftFrontABSReduction/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td width="4" align="left">RF:</td>
                <td  width="7" align="left">
                    <xsl:value-of select="//BrakeRightFrontABSReduction/RightFrontReductionMinParameter"/>
                </td>
                <td  width="7" align="left">----</td>
                <td  width="7" align="left">
                    <xsl:value-of select="//BrakeRightFrontABSReduction/RightFrontABSReductionPercent"/>
                </td>
                <td>
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRightFrontABSReduction/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td width="4" align="left">LR:</td>
                <td  width="7" align="left">
                    <xsl:value-of select="//BrakeLeftRearABSReduction/LeftRearReductionMinParameter"/>
                </td>
                <td  width="7" align="left">----</td>
                <td  width="7" align="left">
                    <xsl:value-of select="//BrakeLeftRearABSReduction/LeftRearABSReductionPercent"/>
                </td>
                <td width="7" align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeLeftRearABSReduction/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td width="4" align="left">RR:</td>
                <td  width="7" align="left">
                    <xsl:value-of select="//BrakeRightRearABSReduction/RightRearReductionMinParameter"/>
                </td>
                <td  width="7" align="left">----</td>
                <td  width="7" align="left">
                    <xsl:value-of select="//BrakeRightRearABSReduction/RightRearABSReductionPercent"/>
                </td>
                <td width="7" align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRightRearABSReduction/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>

            <!-- ABS Recovyer Results -->
            <tr>
                <td align="left">Build: %</td>
            </tr>
            <tr>
                <td width="4" align="left">LF:</td>
                <td  width="7" align="left">
                    <xsl:value-of select="//BrakeLeftFrontABSRecovery/LeftFrontRecoveryMinPercent"/>
                </td>
                <td  width="7" align="left">----</td>
                <td  width="7" align="left">
                    <xsl:value-of select="//BrakeLeftFrontABSRecovery/LeftFrontABSRecoveryValue"/>
                </td>
                <td width="7" align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeLeftFrontABSRecovery/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td  width="4" align="left">RF:</td>
                <td  width="7" align="left">
                    <xsl:value-of select="//BrakeRightFrontABSRecovery/RightFrontRecoveryMinPercent"/>
                </td>
                <td  width="7" align="left">----</td>
                <td  width="7" align="left">
                    <xsl:value-of select="//BrakeRightFrontABSRecovery/RightFrontABSRecoveryValue"/>
                </td>
                <td width="7" align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRightFrontABSRecovery/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td  width="4" align="left">LR:</td>
                <td  width="7" align="left">
                    <xsl:value-of select="//BrakeLeftRearABSRecovery/LeftRearRecoveryMinPercent"/>
                </td>
                <td  width="7" align="left">----</td>
                <td  width="7" align="left">
                    <xsl:value-of select="//BrakeLeftRearABSRecovery/LeftRearABSRecoveryValue"/>
                </td>
                <td width="7" align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeLeftRearABSRecovery/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td  width="4" align="left">RR:</td>
                <td  width="7" align="left">
                    <xsl:value-of select="//BrakeRightRearABSRecovery/RightRearRecoveryMinPercent"/>
                </td>
                <td  width="7" align="left">----</td>
                <td  width="7" align="left">
                    <xsl:value-of select="//BrakeRightRearABSRecovery/RightRearABSRecoveryValue"/>
                </td>
                <td width="7" align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRightRearABSRecovery/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>

            <!-- Speedometer test results -->
            <tr>
                <td align="left">Speedo: KPH</td>
            </tr>
            <tr>
                <td width="4" align="left"/>
                <td  width="7" align="left">
                    <xsl:call-template name="convertSpeed">
                        <xsl:with-param name="SPEED" select="//MachineSpeedometer/MinSpeedometerSpeed"/>
                        <xsl:with-param name="UNITS" select="//MachineSpeedometer/MinSpeedometerSpeed/@Units"/>
                    </xsl:call-template>
                </td>
                <td  width="7" align="left">
                    <xsl:call-template name="convertSpeed">
                        <xsl:with-param name="SPEED" select="//MachineSpeedometer/MaxSpeedometerSpeed"/>
                        <xsl:with-param name="UNITS" select="//MachineSpeedometer/MaxSpeedometerSpeed/@Units"/>
                    </xsl:call-template>
                </td>
                <td  width="7" align="left">
                    <xsl:call-template name="convertSpeed">
                        <xsl:with-param name="SPEED" select="//MachineSpeedometer/SpeedometerSpeed"/>
                        <xsl:with-param name="UNITS" select="//MachineSpeedometer/SpeedometerSpeed/@Units"/>
                    </xsl:call-template>
                </td>
                <td  width="7" align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//MachineSpeedometer/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td align="left">Horn: dB</td>
            </tr>
            <tr>
                <td width="4" align="left"/>
                <td  width="7" align="left">
                    <xsl:value-of select="//SoundLevelSoundPressureLevelTest/MinSoundPressureLevel"/>
                </td>
                <td  width="7" align="left">----</td>
                <td  width="7" align="left">
                    <xsl:value-of select="//SoundLevelSoundPressureLevelTest/SoundPressureLevel"/>
                </td>
                <td  width="7" align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//SoundLevelSoundPressureLevelTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <!-- Component Overall Test Resulst -->
            <tr>
                <td width="25" align="left">Reverse:</td>
                <td width="7" align="left">    
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//MachineReverse/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td width="25" align="left">Axle Ratio:</td>
                <td width="7" align="left">    
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//MachineAxleRatio/@Result"/>
                    </xsl:call-template>
                </td>
            </tr> 

            <!-- Overal Test Result -->
            <tr bgcolor="white">
                <td width="25" align="left">Overall Test Result:</td>
                <td width="7" align="left">
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

    <!-- Inserts a spacer row between sections -->
    <xsl:template name="columnSpacer">
        <th align="left" colspan="1"/>
    </xsl:template>

    <!-- Callout to allow customization of test result flags -->
    <xsl:template name="showResult">
        <xsl:param name="RESULT"/>
        <xsl:choose>
            <xsl:when test="$RESULT != ''">
                <xsl:choose>
                    <xsl:when test="$RESULT='Pass'">
                        <xsl:text>Pass</xsl:text>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:text>Fail</xsl:text>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>No Test</xsl:text>
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
                        <xsl:value-of select="format-number(number($FORCE)*0.45359237,'#.00')"/>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:value-of select="format-number(number($FORCE),'#.00')"/>
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
                        <xsl:value-of select="format-number(number($SPEED)*1.609343387125249,'#.00')"/>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:value-of select="format-number(number($SPEED),'#.00')"/>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>----</xsl:text>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <!-- Converts argument 1 from Inches x10 to mm -->
    <xsl:template name="convertWheelbase">
        <xsl:param name="POSITION"/>
        <xsl:value-of select="format-number(number($POSITION)*2.54,'#.00')"/>
    </xsl:template>

</xsl:stylesheet>

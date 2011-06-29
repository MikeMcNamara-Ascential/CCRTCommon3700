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
            <u>ABS Protocolo</u>
        </h3>
        <table border="1" rules="all" cellpadding="1">
            <tr bgcolor="white">
                <td align="left" width="12">BEP-Roll1</td>
                <td width="12">
                    <xsl:value-of select="//TestResult/MachineInitialize/Date"/>
                </td>
                <td width="12">
                    <xsl:value-of select="//TestResult/MachineInitialize/Time"/>
                </td>
            </tr>
            <tr>
                <td align="left" width="12">Chassi:</td>
                <td align="left">
                    <xsl:value-of select="//VIN"/>
                </td>
            </tr>
            <tr>
                <td align="left" width="18">Tipo de Freio:</td>
                <td align="left" width="13">
                    <xsl:call-template name="displayBrakeType">
                        <xsl:with-param name="BRAKETYPE" select="//BrakeType"/>
                    </xsl:call-template>
                </td>
            </tr>
        </table>
    </xsl:template>

    <!-- Test Results -->
    <xsl:template name="TestResults" match="/BepVehicleTestResult/TestResult">

        <h3/>
        <table border="1" rules="all" title="Test Results" cellpadding="1">

            <!-- ======================================================================================== -->
            <!-- Drag Test Results -->
            <!-- ======================================================================================== -->
            <tr bgcolor="white">
                <td align="right" width="14">TESTE ARRASTE</td>
                <td width="5"/>
                <td align="right" width="5">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeAnalyzeDragTest/@Result"/>
                    </xsl:call-template>
                </td>
                <td width="5"/>
                <td width="7"/>
            </tr>
            <xsl:call-template name="headerRow"/>
            <tr>
                <td align="left" width="14">Diant LE</td>
                <td align="center" width="5"/>
                <td align="center" width="5">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftFrontDragTest/DragForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftFrontDragTest/DragForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center" width="5">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftFrontDragTest/MaxDragForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftFrontDragTest/MaxDragForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center" width="7">N</td>
            </tr>
            <tr>
                <td align="right" width="14">Diant LD</td>
                <td align="center" width="5"/>
                <td align="center" width="5">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightFrontDragTest/DragForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightFrontDragTest/DragForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center" width="5">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightFrontDragTest/MaxDragForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightFrontDragTest/MaxDragForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center" width="7">N</td>
            </tr>
            <tr>
                <td align="right" width="14">Tras LE</td>
                <td align="center" width="5"/>
                <td align="center" width="5">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftRearDragTest/DragForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftRearDragTest/DragForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center" width="5">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftRearDragTest/MaxDragForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftRearDragTest/MaxDragForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center" width="7">N</td>
            </tr>
            <tr>
                <td align="right" width="14">Tras LD</td>
                <td align="center" width="5"/>
                <td align="center" width="5">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightRearDragTest/DragForce"/>
                        <xsl:with-param name="UNITS"  select="//BrakeRightRearDragTest/DragForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center" width="5">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightRearDragTest/MaxDragForce"/>
                        <xsl:with-param name="UNITS"  select="//BrakeRightRearDragTest/MaxDragForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center" width="7">N</td>
            </tr>

            <!-- ======================================================================================== -->
            <!-- Brake Test Results -->
            <!-- ======================================================================================== -->
            <xsl:call-template name="sectionSpacer"/>
            <tr bgcolor="white">
                <td align="right" width="14">TESTE DE FREIO</td>
                <td width="5"/>
                <td align="right" width="5">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeAnalyzeBrakeTest/@Result"/>
                    </xsl:call-template>
                </td>
                <td width="5"/>
                <td width="7"/>
            </tr>
            <xsl:call-template name="headerRow"/>
            <tr>
                <td align="right" width="14">Diant LE</td>
                <td align="center" width="5">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftFrontForceTest/MinBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftFrontForceTest/MinBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center" width="5">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftFrontForceTest/BrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftFrontForceTest/BrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center" width="5">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftFrontForceTest/MaxBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftFrontForceTest/MaxBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center" width="7">N</td>
            </tr>
            <tr>
                <td align="right" width="14">Diant LD</td>
                <td align="center" width="5">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightFrontForceTest/MinBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightFrontForceTest/MinBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center" width="5">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightFrontForceTest/BrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightFrontForceTest/BrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center" width="5">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightFrontForceTest/MaxBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightFrontForceTest/MaxBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center" width="7">N</td>
            </tr>
            <tr>
                <td align="right" width="14">Tras LE</td>
                <td align="center" width="5">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftRearForceTest/MinBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftRearForceTest/MinBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center" width="5">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftRearForceTest/BrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftRearForceTest/BrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center" width="5">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftRearForceTest/MaxBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftRearForceTest/MaxBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center" width="7">N</td>
            </tr>
            <tr>
                <td align="right" width="14">Tras LD</td>
                <td align="center" width="5">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightRearForceTest/MinBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightRearForceTest/MinBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center" width="5">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightRearForceTest/BrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightRearForceTest/BrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center" width="5">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightRearForceTest/MaxBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightRearForceTest/MaxBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center" width="7">N</td>
            </tr>

            <!-- ======================================================================================== -->
            <!-- Hand Brake Test Results -->
            <!-- ======================================================================================== -->
            <xsl:call-template name="sectionSpacer"/>
            <tr>
                <td align="right" width="14">FREIO DE MAO</td>
                <td width="5"/>
                <td align="right" width="5">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeAnalyzeDynamicParkBrake/@Result"/>
                    </xsl:call-template>
                </td>
                <td width="5"/>
                <td width="7"/>
            </tr>
            <xsl:call-template name="headerRow"/>
            <tr>
                <td align="right" width="14">Tras LE</td>
                <td align="center" width="5">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftRearParkBrakeForceTest/MinParkBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftRearParkBrakeForceTest/MinParkBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center" width="5">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftRearParkBrakeForceTest/ParkBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftRearParkBrakeForceTest/ParkBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center" width="5">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftRearParkBrakeForceTest/MaxParkBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftRearParkBrakeForceTest/MaxParkBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center" width="7">N</td>
            </tr>
            <tr>
                <td align="right" width="14">Tras LD</td>
                <td align="center" width="5">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightRearParkBrakeForceTest/MinParkBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightRearParkBrakeForceTest/MinParkBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center" width="5">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightRearParkBrakeForceTest/ParkBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightRearParkBrakeForceTest/ParkBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center" width="5">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightRearParkBrakeForceTest/MaxParkBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightRearParkBrakeForceTest/MaxParkBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center" width="7">N</td>
            </tr>

            <!-- ======================================================================================== -->
            <!-- ABS Wheel Speed Sensor Test Results -->
            <!-- ======================================================================================== -->
            <xsl:if test="//BrakePerformSensorQualityTest">
                <xsl:call-template name="sectionSpacer"/>
                <tr>
                    <td align="right" width="14">SENSORES ABS</td>
                    <td width="5"/>
                    <td align="right" width="5">
                        <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//BrakePerformSensorQualityTest/@Result"/>
                        </xsl:call-template>
                    </td>
                    <td width="5"/>
                    <td width="7"/>
                </tr>
                <xsl:call-template name="headerRow"/>
                <tr>
                    <td align="right" width="14">Diant LE</td>
                    <td align="center" width="5">
                        <xsl:call-template name="convertSpeed">
                            <xsl:with-param name="SPEED" select="//BrakePerformSensorQualityTest/LeftFrontMinSpeedValue"/>
                            <xsl:with-param name="UNITS" select="//BrakePerformSensorQualityTest/LeftFrontMinSpeedValue/@Units"/>
                        </xsl:call-template>
                    </td>
                    <td align="center" width="5">
                        <xsl:call-template name="convertSpeed">
                            <xsl:with-param name="SPEED" select="//BrakePerformSensorQualityTest/LeftFrontSensorSpeed"/>
                            <xsl:with-param name="UNITS" select="//BrakePerformSensorQualityTest/LeftFrontSensorSpeed/@Units"/>
                        </xsl:call-template>
                    </td>
                    <td align="center" width="5">
                        <xsl:call-template name="convertSpeed">
                            <xsl:with-param name="SPEED" select="//BrakePerformSensorQualityTest/LeftFrontMaxSpeedValue"/>
                            <xsl:with-param name="UNITS" select="//BrakePerformSensorQualityTest/LeftFrontMaxSpeedValue/@Units"/>
                        </xsl:call-template>
                    </td>
                    <td align="center" width="7">Km/h</td>
                </tr>
                <tr>
                    <td align="right" width="14">Diant LD</td>
                    <td align="center" width="5">
                        <xsl:call-template name="convertSpeed">
                            <xsl:with-param name="SPEED" select="//BrakePerformSensorQualityTest/RightFrontMinSpeedValue"/>
                            <xsl:with-param name="UNITS" select="//BrakePerformSensorQualityTest/RightFrontMinSpeedValue/@Units"/>
                        </xsl:call-template>
                    </td>
                    <td align="center" width="5">
                        <xsl:call-template name="convertSpeed">
                            <xsl:with-param name="SPEED" select="//BrakePerformSensorQualityTest/RightFrontSensorSpeed"/>
                            <xsl:with-param name="UNITS" select="//BrakePerformSensorQualityTest/RightFrontSensorSpeed/@Units"/>
                        </xsl:call-template>
                    </td>
                    <td align="center" width="5">
                        <xsl:call-template name="convertSpeed">
                            <xsl:with-param name="SPEED" select="//BrakePerformSensorQualityTest/RightFrontMaxSpeedValue"/>
                            <xsl:with-param name="UNITS" select="//BrakePerformSensorQualityTest/RightFrontMaxSpeedValue/@Units"/>
                        </xsl:call-template>
                    </td>
                    <td align="center" width="7">Km/h</td>
                </tr>
                <tr>
                    <td align="right" width="14">Tras LE</td>
                    <td align="center" width="5">
                        <xsl:call-template name="convertSpeed">
                            <xsl:with-param name="SPEED" select="//BrakePerformSensorQualityTest/LeftRearMinSpeedValue"/>
                            <xsl:with-param name="UNITS" select="//BrakePerformSensorQualityTest/LeftRearMinSpeedValue/@Units"/>
                        </xsl:call-template>
                    </td>
                    <td align="center" width="5">
                        <xsl:call-template name="convertSpeed">
                            <xsl:with-param name="SPEED" select="//BrakePerformSensorQualityTest/LeftRearSensorSpeed"/>
                            <xsl:with-param name="UNITS" select="//BrakePerformSensorQualityTest/LeftRearSensorSpeed/@Units"/>
                        </xsl:call-template>
                    </td>
                    <td align="center" width="5">
                        <xsl:call-template name="convertSpeed">
                            <xsl:with-param name="SPEED" select="//BrakePerformSensorQualityTest/LeftRearMaxSpeedValue"/>
                            <xsl:with-param name="UNITS" select="//BrakePerformSensorQualityTest/LeftRearMaxSpeedValue/@Units"/>
                        </xsl:call-template>
                    </td>
                    <td align="center" width="7">Km/h</td>
                </tr>
                <tr>
                    <td align="right" width="14">Tras LD</td>
                    <td align="center" width="5">
                        <xsl:call-template name="convertSpeed">
                            <xsl:with-param name="SPEED" select="//BrakePerformSensorQualityTest/RightRearMinSpeedValue"/>
                            <xsl:with-param name="UNITS" select="//BrakePerformSensorQualityTest/RightRearMinSpeedValue/@Units"/>
                        </xsl:call-template>
                    </td>
                    <td align="center" width="5">
                        <xsl:call-template name="convertSpeed">
                            <xsl:with-param name="SPEED" select="//BrakePerformSensorQualityTest/RightRearSensorSpeed"/>
                            <xsl:with-param name="UNITS" select="//BrakePerformSensorQualityTest/RightRearSensorSpeed/@Units"/>
                        </xsl:call-template>
                    </td>
                    <td align="center" width="5">
                        <xsl:call-template name="convertSpeed">
                            <xsl:with-param name="SPEED" select="//BrakePerformSensorQualityTest/RightRearMaxSpeedValue"/>
                            <xsl:with-param name="UNITS" select="//BrakePerformSensorQualityTest/RightRearMaxSpeedValue/@Units"/>
                        </xsl:call-template>
                    </td>
                    <td align="center" width="7">Km/h</td>
                </tr>
            </xsl:if>

            <xsl:if test="//BrakeLowSpeedSensorTest">
                <xsl:call-template name="sectionSpacer"/>
                <tr>
                    <td align="right" width="14">SENSORES ABS</td>
                    <td width="5"/>
                    <td align="right" width="5">
                        <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//BrakeLowSpeedSensorTest/@Result"/>
                        </xsl:call-template>
                    </td>
                    <td width="5"/>
                    <td width="7"/>
                </tr>
                <xsl:call-template name="headerRow"/>
                <tr>
                    <td align="right" width="14">Diant LE</td>
                    <td align="center" width="5">
                        <xsl:call-template name="convertSpeed">
                            <xsl:with-param name="SPEED" select="//BrakeLowSpeedSensorTest/LeftFrontMinSpeedValue"/>
                            <xsl:with-param name="UNITS" select="//BrakeLowSpeedSensorTest/LeftFrontMinSpeedValue/@Units"/>
                        </xsl:call-template>
                    </td>
                    <td align="center" width="5">
                        <xsl:call-template name="convertSpeed">
                            <xsl:with-param name="SPEED" select="//BrakeLowSpeedSensorTest/LeftFrontSensorSpeed"/>
                            <xsl:with-param name="UNITS" select="//BrakeLowSpeedSensorTest/LeftFrontSensorSpeed/@Units"/>
                        </xsl:call-template>
                    </td>
                    <td align="center" width="5">
                        <xsl:call-template name="convertSpeed">
                            <xsl:with-param name="SPEED" select="//BrakeLowSpeedSensorTest/LeftFrontMaxSpeedValue"/>
                            <xsl:with-param name="UNITS" select="//BrakeLowSpeedSensorTest/LeftFrontMaxSpeedValue/@Units"/>
                        </xsl:call-template>
                    </td>
                    <td align="center" width="7">Km/h</td>
                </tr>
                <tr>
                    <td align="right" width="14">Diant LD</td>
                    <td align="center" width="5">
                        <xsl:call-template name="convertSpeed">
                            <xsl:with-param name="SPEED" select="//BrakeLowSpeedSensorTest/RightFrontMinSpeedValue"/>
                            <xsl:with-param name="UNITS" select="//BrakeLowSpeedSensorTest/RightFrontMinSpeedValue/@Units"/>
                        </xsl:call-template>
                    </td>
                    <td align="center" width="5">
                        <xsl:call-template name="convertSpeed">
                            <xsl:with-param name="SPEED" select="//BrakeLowSpeedSensorTest/RightFrontSensorSpeed"/>
                            <xsl:with-param name="UNITS" select="//BrakeLowSpeedSensorTest/RightFrontSensorSpeed/@Units"/>
                        </xsl:call-template>
                    </td>
                    <td align="center" width="5">
                        <xsl:call-template name="convertSpeed">
                            <xsl:with-param name="SPEED" select="//BrakeLowSpeedSensorTest/RightFrontMaxSpeedValue"/>
                            <xsl:with-param name="UNITS" select="//BrakeLowSpeedSensorTest/RightFrontMaxSpeedValue/@Units"/>
                        </xsl:call-template>
                    </td>
                    <td align="center" width="7">Km/h</td>
                </tr>
                <tr>
                    <td align="right" width="14">Tras LE</td>
                    <td align="center" width="5">
                        <xsl:call-template name="convertSpeed">
                            <xsl:with-param name="SPEED" select="//BrakeLowSpeedSensorTest/LeftRearMinSpeedValue"/>
                            <xsl:with-param name="UNITS" select="//BrakeLowSpeedSensorTest/LeftRearMinSpeedValue/@Units"/>
                        </xsl:call-template>
                    </td>
                    <td align="center" width="5">
                        <xsl:call-template name="convertSpeed">
                            <xsl:with-param name="SPEED" select="//BrakeLowSpeedSensorTest/LeftRearSensorSpeed"/>
                            <xsl:with-param name="UNITS" select="//BrakeLowSpeedSensorTest/LeftRearSensorSpeed/@Units"/>
                        </xsl:call-template>
                    </td>
                    <td align="center" width="5">
                        <xsl:call-template name="convertSpeed">
                            <xsl:with-param name="SPEED" select="//BrakeLowSpeedSensorTest/LeftRearMaxSpeedValue"/>
                            <xsl:with-param name="UNITS" select="//BrakeLowSpeedSensorTest/LeftRearMaxSpeedValue/@Units"/>
                        </xsl:call-template>
                    </td>
                    <td align="center" width="7">Km/h</td>
                </tr>
                <tr>
                    <td align="right" width="14">Tras LD</td>
                    <td align="center" width="5">
                        <xsl:call-template name="convertSpeed">
                            <xsl:with-param name="SPEED" select="//BrakeLowSpeedSensorTest/RightRearMinSpeedValue"/>
                            <xsl:with-param name="UNITS" select="//BrakeLowSpeedSensorTest/RightRearMinSpeedValue/@Units"/>
                        </xsl:call-template>
                    </td>
                    <td align="center" width="5">
                        <xsl:call-template name="convertSpeed">
                            <xsl:with-param name="SPEED" select="//BrakeLowSpeedSensorTest/RightRearSensorSpeed"/>
                            <xsl:with-param name="UNITS" select="//BrakeLowSpeedSensorTest/RightRearSensorSpeed/@Units"/>
                        </xsl:call-template>
                    </td>
                    <td align="center" width="5">
                        <xsl:call-template name="convertSpeed">
                            <xsl:with-param name="SPEED" select="//BrakeLowSpeedSensorTest/RightRearMaxSpeedValue"/>
                            <xsl:with-param name="UNITS" select="//BrakeLowSpeedSensorTest/RightRearMaxSpeedValue/@Units"/>
                        </xsl:call-template>
                    </td>
                    <td align="center" width="7">Km/h</td>
                </tr>
            </xsl:if>

            <!-- ======================================================================================== -->
            <!-- ABS Test Results -->
            <!-- ======================================================================================== -->
            <xsl:if test="//BrakeEvaluateABS">
                <xsl:call-template name="sectionSpacer"/>
                <tr>
                    <td align="right" width="14">ABS</td>
                    <td width="5"/>
                    <td align="right" width="5">
                        <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//BrakeEvaluateABS/@Result"/>
                        </xsl:call-template>
                    </td>
                    <td width="5"/>
                    <td width="7"/>
                </tr>
                <xsl:call-template name="headerRow"/>
                <tr>
                    <td align="right" width="14">Red Diant LE</td>
                    <td align="center" width="5"/>
                    <td align="center" width="5">
                        <xsl:value-of select="format-number(number(//BrakeLeftFrontABSReduction/LeftFrontABSReductionPercent),'#')"/>
                    </td>
                    <td align="center" width="5">
                        <xsl:value-of select="format-number(number(//BrakeLeftFrontABSReduction/LeftFrontReductionMinParameter),'#')"/>
                    </td>
                    <td align="center" width="7">%</td>
                </tr>
                <tr>
                    <td align="right" width="14">Red Diant LD</td>
                    <td align="center" width="5"/>
                    <td align="center" width="5">
                        <xsl:value-of select="format-number(number(//BrakeRightFrontABSReduction/RightFrontABSReductionPercent),'#')"/>
                    </td>
                    <td align="center" width="5">
                        <xsl:value-of select="format-number(number(//BrakeRightFrontABSReduction/RightFrontReductionMinParameter),'#')"/>
                    </td>
                    <td align="center" width="7">%</td>
                </tr>
                <tr>
                    <td align="right" width="14">Red Tras LE</td>
                    <td align="center" width="5"/>
                    <td align="center" width="5">
                        <xsl:value-of select="format-number(number(//BrakeLeftRearABSReduction/LeftRearABSReductionPercent),'#')"/>
                    </td>
                    <td align="center" width="5">
                        <xsl:value-of select="format-number(number(//BrakeLeftRearABSReduction/LeftRearReductionMinParameter),'#')"/>
                    </td>
                    <td align="center" width="7">%</td>
                </tr>
                <tr>
                    <td align="right" width="14">Red Tras LD</td>
                    <td align="center" width="5"/>
                    <td align="center" width="5">
                        <xsl:value-of select="format-number(number(//BrakeRightRearABSReduction/RightRearABSReductionPercent),'#')"/>
                    </td>
                    <td align="center" width="5">
                        <xsl:value-of select="format-number(number(//BrakeRightRearABSReduction/RightRearReductionMinParameter),'#')"/>
                    </td>
                    <td align="center" width="7">%</td>
                </tr>
                <tr>
                    <td align="right" width="14">Rec Diant LE</td>
                    <td align="center" width="5">
                        <xsl:value-of select="format-number(number(//BrakeLeftFrontABSRecovery/LeftFrontRecoveryMinPercent),'#')"/>
                    </td>
                    <td align="center" width="5">
                        <xsl:value-of select="format-number(number(//BrakeLeftFrontABSRecovery/LeftFrontABSRecoveryValue),'#')"/>
                    </td>
                    <td align="center" width="5"/>
                    <td align="center" width="7">%</td>
                </tr>
                <tr>
                    <td align="right" width="14">Rec Diant LD</td>
                    <td align="center" width="5">
                        <xsl:value-of select="format-number(number(//BrakeRightFrontABSRecovery/RightFrontRecoveryMinPercent),'#')"/>
                    </td>
                    <td align="center" width="5">
                        <xsl:value-of select="format-number(number(//BrakeRightFrontABSRecovery/RightFrontABSRecoveryValue),'#')"/>
                    </td>
                    <td align="center" width="5"/>
                    <td align="center" width="7">%</td>
                </tr>
                <tr>
                    <td align="right" width="14">Rec Tras LE</td>
                    <td align="center" width="5">
                        <xsl:value-of select="format-number(number(//BrakeLeftRearABSRecovery/LeftRearRecoveryMinPercent),'#')"/>
                    </td>
                    <td align="center" width="5">
                        <xsl:value-of select="format-number(number(//BrakeLeftRearABSRecovery/LeftRearABSRecoveryValue),'#')"/>
                    </td>
                    <td align="center" width="5"/>
                    <td align="center" width="7">%</td>
                </tr>
                <tr>
                    <td align="right" width="14">Rec Tras LD</td>
                    <td align="center" width="5">
                        <xsl:value-of select="format-number(number(//BrakeRightRearABSRecovery/RightRearRecoveryMinPercent),'#')"/>
                    </td>
                    <td align="center" width="5">
                        <xsl:value-of select="format-number(number(//BrakeRightRearABSRecovery/RightRearABSRecoveryValue),'#')"/>
                    </td>
                    <td align="center" width="5"/>
                    <td align="center" width="7">%</td>
                </tr>
            </xsl:if>
                            
            <!-- ======================================================================================== -->
            <!-- Overall Result -->
            <!-- ======================================================================================== -->
            <xsl:call-template name="sectionSpacer"/>
            <xsl:call-template name="sectionSpacer"/>

            <tr>
                <td align="right" width="14">TESTE</td>
                <td align="right" width="7">
                    <xsl:text>-</xsl:text>
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//OverallTest/@Result"/>
                    </xsl:call-template>
                    <xsl:text>-</xsl:text>
                </td>
                <td width="5"/>
                <td width="5"/>
                <td width="5"/>
            </tr>

            <xsl:call-template name="sectionSpacer"/>
            <xsl:call-template name="sectionSpacer"/>
            <xsl:call-template name="sectionSpacer"/>
            <xsl:call-template name="sectionSpacer"/>

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

        <xsl:choose>
            <xsl:when test="$RESULT = 'Pass'">
                <xsl:text>OK</xsl:text>
            </xsl:when>
            <xsl:when test="$RESULT != 'Pass'">
                <xsl:text>FALHA</xsl:text>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <!-- Converts argument 1 from LBF to Newtons -->
    <xsl:template name="convertForce">
        <xsl:param name="FORCE"/>
        <xsl:param name="UNITS"/>
        <xsl:choose>
            <!-- The force value is a valid number -->
            <xsl:when test="string(number($FORCE)) != 'NaN'">
                <xsl:choose>
                    <!-- Check if units are lbf -->
                    <xsl:when test="translate($UNITS,'LBF','lbf')='lbf'">
                        <xsl:value-of select="format-number(number($FORCE)*4.448222,'#')"/>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:value-of select="format-number(number($FORCE),'#')"/>
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
                        <xsl:value-of select="format-number(number($SPEED)*1.609343387125249,'#.#')"/>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:value-of select="format-number(number($SPEED),'#.#')"/>
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
            <td align="center" width="14"/>
            <td align="center" width="5">Min</td>
            <td align="center" width="5">Atu</td>
            <td align="center" width="5">Max</td>
            <td align="center" width="7">Unid</td>
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

    <xsl:template name="displayBrakeType">
        <xsl:param name="BRAKETYPE"/>
        <xsl:choose>
            <xsl:when test="$BRAKETYPE = 'Base Brake'">
                <xsl:text>No ABS</xsl:text>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="$BRAKETYPE"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

</xsl:stylesheet>

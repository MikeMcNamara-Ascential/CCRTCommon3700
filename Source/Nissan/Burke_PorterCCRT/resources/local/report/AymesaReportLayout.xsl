<?xml version="1.0"?>
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
            <u>Aymesa Reporte De Inspeccion Vehicular</u>
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
                <td align="left">VIN</td>
                <td align="left">
                    <u>
                        <xsl:value-of select="//VIN"/>
                    </u>
                </td>
                <td align="left"/>
                <td align="left"/>
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
                <th align="left" width="20">Prueba de Arrastre</th>
                <td align="right">LF</td>
                <td align="center"/>
                <td align="center">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftFrontDragTest/DragForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftFrontDragTest/DragForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftFrontDragTest/MaxDragForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftFrontDragTest/MaxDragForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center">KGF</td>
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
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightFrontDragTest/DragForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightFrontDragTest/DragForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightFrontDragTest/MaxDragForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightFrontDragTest/MaxDragForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center">KGF</td>
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
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftRearDragTest/DragForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftRearDragTest/DragForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftRearDragTest/MaxDragForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftRearDragTest/MaxDragForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center">KGF</td>
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
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightRearDragTest/DragForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightRearDragTest/DragForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightRearDragTest/MaxDragForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightRearDragTest/MaxDragForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center">KGF</td>
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
                <th align="left" width="20">Prueba de Frenado</th>
                <td align="right">LF</td>
                <td align="center">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftFrontForceTest/MinBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftFrontForceTest/MinBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftFrontForceTest/BrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftFrontForceTest/BrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftFrontForceTest/MaxBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftFrontForceTest/MaxBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center">KGF</td>
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
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightFrontForceTest/MinBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightFrontForceTest/MinBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightFrontForceTest/BrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightFrontForceTest/BrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightFrontForceTest/MaxBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightFrontForceTest/MaxBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center">KGF</td>
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
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftRearForceTest/MinBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftRearForceTest/MinBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftRearForceTest/BrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftRearForceTest/BrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftRearForceTest/MaxBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftRearForceTest/MaxBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center">KGF</td>
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
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightRearForceTest/MinBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightRearForceTest/MinBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightRearForceTest/BrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightRearForceTest/BrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightRearForceTest/MaxBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightRearForceTest/MaxBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center">KGF</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRightRearForceTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <!-- Balance Results -->
            <tr>
                <td align="right" width="20">Balance Delantero</td>
                <th align="left"/>
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
                <td align="right" width="20">Balance Posterior</td>
                <th align="left"/>
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
                <td align="right" width="20">Delantero/Posterior</td>
                <th align="left"/>
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
            <!-- Park Brake Test Results -->
            <!-- ======================================================================================= -->
            <xsl:call-template name="sectionSpacer"/>
            <xsl:call-template name="headerRow"/>
            <tr>
                <td align="right" width="20">Freno de Mano</td>
                <th align="right">LR</th>
                <td align="center">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftRearParkBrakeForceTest/MinParkBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftRearParkBrakeForceTest/MinParkBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftRearParkBrakeForceTest/ParkBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftRearParkBrakeForceTest/ParkBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeLeftRearParkBrakeForceTest/MaxParkBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeLeftRearParkBrakeForceTest/MaxParkBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center">KGF</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeLeftRearParkBrakeForceTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td align="right" width="20"/>
                <th align="right">RR</th>
                <td align="center">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightRearParkBrakeForceTest/MinParkBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightRearParkBrakeForceTest/MinParkBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightRearParkBrakeForceTest/ParkBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightRearParkBrakeForceTest/ParkBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center">
                    <xsl:call-template name="convertForce">
                        <xsl:with-param name="FORCE" select="//BrakeRightRearParkBrakeForceTest/MaxParkBrakeForce"/>
                        <xsl:with-param name="UNITS" select="//BrakeRightRearParkBrakeForceTest/MaxParkBrakeForce/@Units"/>
                    </xsl:call-template>
                </td>
                <td align="center">KGF</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeRightRearParkBrakeForceTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>


            <!-- ======================================================================================= -->
            <!-- Axle Ratio Test Results -->
            <!-- ======================================================================================= -->
            <xsl:call-template name="sectionSpacer"/>
            <xsl:call-template name="generalHeaderRow"/>
            <tr>
                <td align="right" width="25">Relacion de Transmission</td>
                <th align="right" width="5"/>
                <td align="center">
                    <xsl:value-of select="format-number(number(//TransferCaseCheckAxleRatio/AxleRatioMin),'#.##')"/>
                </td>
                <td align="center">
                    <xsl:value-of select="format-number(number(//TransferCaseCheckAxleRatio/ActualAxleRatio),'#.##')"/>
                </td>
                <td align="center">
                    <xsl:value-of select="format-number(number(//TransferCaseCheckAxleRatio/AxleRatioMax),'#.##')"/>
                </td>
                <td align="center">%</td>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransferCaseCheckAxleRatio/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>


            <!-- ======================================================================================= -->
            <!-- Reverse Test Results -->
            <!-- ======================================================================================= -->
            <xsl:call-template name="sectionSpacer"/>
            <tr bgcolor="white">
                <th align="left" width="25">Prueba de Reversa</th>
                <td align="center"/>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//MachineReverse/@Result"/>
                    </xsl:call-template>
                </td>
                <td align="center"/>
                <td align="center"/>
                <td align="center"/>
            </tr>


            <!-- ======================================================================================= -->
            <!-- Operator Buyoff Results -->
            <!-- ======================================================================================= -->
            <tr bgcolor="white">
                <th align="left" width="25">Luces de Reversa</th>
                <td align="center"/>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//MachineCheckReverseLamps/@Result"/>
                    </xsl:call-template>
                </td>
                <td align="center"/>
                <td align="center"/>
                <td align="center"/>
            </tr>
            <tr bgcolor="white">
                <th align="left" width="25">Luces de Freno</th>
                <td align="center"/>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//MachineBrakeLampCheck/@Result"/>
                    </xsl:call-template>
                </td>
                <td align="center"/>
                <td align="center"/>
                <td align="center"/>
            </tr>
            <tr bgcolor="white">
                <th align="left" width="25">Direccional Izquierda</th>
                <td align="center"/>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//MachineLeftTurnSignalLampCheck/@Result"/>
                    </xsl:call-template>
                </td>
                <td align="center"/>
                <td align="center"/>
                <td align="center"/>
            </tr>
            <tr bgcolor="white">
                <th align="left" width="25">Direccional Derecha</th>
                <td align="center"/>
                <td align="center">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//MachineRightTurnSignalLampCheck/@Result"/>
                    </xsl:call-template>
                </td>
                <td align="center"/>
                <td align="center"/>
                <td align="center"/>
            </tr>

        </table>
    </xsl:template>


    <!-- Inserts a spacer row between sections -->
    <xsl:template name="sectionSpacer">
        <tr bgcolor="white">
            <th align="left" colspan="7" height="15"/>
        </tr>
    </xsl:template>

    <!-- Adds a header row -->
    <xsl:template name="headerRow">
        <tr>
            <th align="center" width="20"/>
            <th align="center">Neumatico</th>
            <th align="center">Minimo</th>
            <th align="center">Medido</th>
            <th align="center">Maximo</th>
            <th align="center">Unidad</th>
            <th align="center">Resultado</th>
        </tr>
    </xsl:template>

    <!-- Adds a header row -->
    <xsl:template name="generalHeaderRow">
        <tr>
            <th align="center" width="25"/>
            <th align="center" width="5"/>
            <th align="center">Minimo</th>
            <th align="center">Medido</th>
            <th align="center">Maximo</th>
            <th align="center">Unidad</th>
            <th align="center">Resultado</th>
        </tr>
    </xsl:template>

    <!-- Callout to allow cutomization of test result flags -->
    <xsl:template name="showResult">
        <xsl:param name="RESULT"/>
        <i>
            <xsl:choose>
                <xsl:when test="$RESULT != ''">
                    <xsl:choose>
			        <xsl:when test="$RESULT = 'Pass'">
			            <xsl:text>OK</xsl:text>
			        </xsl:when>
                        <xsl:otherwise>
                            <xsl:text>NOK</xsl:text>
                        </xsl:otherwise>
                    </xsl:choose>
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

</xsl:stylesheet>

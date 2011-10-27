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
            <u>Relatorio de Teste do Veiculo</u>
        </h3>

        <!-- Test Time, Date and Machine number -->
        <table border="1" rules="all" cellpadding="0" cellspacing="0" width="50%">
            <tr>
                <td align="left">
                    <u>Resultado Geral</u>
                </td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//OverallTest/@Result"/>
                    </xsl:call-template>
                </td>
                <td align="left">
                    <u>Data do Teste</u>
                </td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BuildDate"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td align="left">
                    <u>Maquina</u>
                </td>
                <td align="left">1</td>
                <!--
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TestResult/MachineInitialize/MachineNumber"/>
                    </xsl:call-template>
                </td>
                -->
                <td align="left">
                    <u>Hora do Teste</u>
                </td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BuildTime"/>
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
                        <u>Dados da Configuracao do Veiculo</u>
                    </b>
                </th>
            </tr>
            <xsl:call-template name="categorySpacer"/>
            <tr>
                <td align="left">VIN</td>
                <td align="left">
                    <xsl:value-of select="//VIN"/>
                </td>
                <td align="left">Numero de Producao</td>
                <td align="left">
                    <xsl:value-of select="//ProductionNumber"/>
                </td>
            </tr>
            <tr>
                <td align="left">Entre Eixos</td>
                <td align="left">
                    <xsl:value-of select="//Wheelbase"/>
                </td>
                <td align="left">Numero de Serie</td>
                <td align="left">
                    <xsl:value-of select="//SequenceNumber"/>
                </td>
            </tr>
            <tr>
                <td align="left">Potencia do Motor</td>
                <td align="left">
                    <xsl:value-of select="//EnginePower"/>
                </td>
                <td align="left">Tamanho do Pneu</td>
                <td align="left">
                    <xsl:value-of select="//TireSize"/>
                </td>
            </tr>
            <tr>
                <td align="left">Velocidade Maxima</td>
                <td align="left">
                    <xsl:value-of select="//MaxSpeed"/>
                </td>
                <td align="left">Tracao</td>
                <td align="left">
                    <xsl:value-of select="//Traction"/>
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
                        <u>Resultados do Teste</u>
                    </b>
                </th>
            </tr>
            <xsl:call-template name="categorySpacer"/>

            <!-- Brake Test Results -->
            <xsl:if test="//BrakeAnalyzeBrakeTest/@Result!='Pass'">
                <xsl:call-template name="sectionSpacer"/>
                <tr bgcolor="white">
                    <th align="center" rowspan="12">Teste de Freio</th>
                </tr>
                <tr>
                <td align="left">Forca Esquerda Dianteira</td>
                    <td align="left">
                        <xsl:call-template name="convertForce">
                            <xsl:with-param name="FORCE" select="//BrakeLeftFrontForceTest/BrakeForce"/>
                        </xsl:call-template>
                    </td>
                    <td align="left">
                        <xsl:text>[</xsl:text>
                        <xsl:call-template name="convertForce">
                            <xsl:with-param name="FORCE" select="//BrakeLeftFrontForceTest/MinBrakeForce"/>
                        </xsl:call-template>
                        <xsl:text> - </xsl:text>
                        <xsl:call-template name="convertForce">
                            <xsl:with-param name="FORCE" select="//BrakeLeftFrontForceTest/MaxBrakeForce"/>
                        </xsl:call-template>
                        <xsl:text>]</xsl:text>
                        <xsl:text>  kgf</xsl:text>
                    </td>
                </tr>
                <tr>
                <td align="left">Forca Direita Dianteira</td>
                    <td align="left">
                        <xsl:call-template name="convertForce">
                            <xsl:with-param name="FORCE" select="//BrakeRightFrontForceTest/BrakeForce"/>
                        </xsl:call-template>
                    </td>
                    <td align="left">
                        <xsl:text>[</xsl:text>
                        <xsl:call-template name="convertForce">
                            <xsl:with-param name="FORCE" select="//BrakeRightFrontForceTest/MinBrakeForce"/>
                        </xsl:call-template>
                        <xsl:text> - </xsl:text>
                        <xsl:call-template name="convertForce">
                            <xsl:with-param name="FORCE" select="//BrakeRightFrontForceTest/MaxBrakeForce"/>
                        </xsl:call-template>
                        <xsl:text>]</xsl:text>
                        <xsl:text>  kgf</xsl:text>
                    </td>
                </tr>
                <tr>
                <td align="left">Forca Traseira Esquerda</td>
                    <td align="left">
                        <xsl:call-template name="convertForce">
                            <xsl:with-param name="FORCE" select="//BrakeLeftRearForceTest/BrakeForce"/>
                        </xsl:call-template>
                    </td>
                    <td align="left">
                        <xsl:text>[</xsl:text>
                        <xsl:call-template name="convertForce">
                            <xsl:with-param name="FORCE" select="//BrakeLeftRearForceTest/MinBrakeForce"/>
                        </xsl:call-template>
                        <xsl:text> - </xsl:text>
                        <xsl:call-template name="convertForce">
                            <xsl:with-param name="FORCE" select="//BrakeLeftRearForceTest/MaxBrakeForce"/>
                        </xsl:call-template>
                        <xsl:text>]</xsl:text>
                        <xsl:text>  kgf</xsl:text>
                    </td>
                </tr>
                <tr>
                <td align="left">Forca Traseira Direita</td>
                    <td align="left">
                        <xsl:call-template name="convertForce">
                            <xsl:with-param name="FORCE" select="//BrakeRightRearForceTest/BrakeForce"/>
                        </xsl:call-template>
                    </td>
                    <td align="left">
                        <xsl:text>[</xsl:text>
                        <xsl:call-template name="convertForce">
                            <xsl:with-param name="FORCE" select="//BrakeRightRearForceTest/MinBrakeForce"/>
                        </xsl:call-template>
                        <xsl:text> - </xsl:text>
                        <xsl:call-template name="convertForce">
                            <xsl:with-param name="FORCE" select="//BrakeRightRearForceTest/MaxBrakeForce"/>
                        </xsl:call-template>
                        <xsl:text>]</xsl:text>
                        <xsl:text>  kgf</xsl:text>
                    </td>
                    <td>
                    </td>
                    <td>
                    </td>
                </tr>
                <tr>
                <td align="left">Forca Tandem Esquerda</td>
                    <td align="left">
                        <xsl:call-template name="convertForce">
                            <xsl:with-param name="FORCE" select="//BrakeLeftTandemForceTest/BrakeForce"/>
                        </xsl:call-template>
                    </td>
                    <td align="left">
                        <xsl:text>[</xsl:text>
                        <xsl:call-template name="convertForce">
                            <xsl:with-param name="FORCE" select="//BrakeLeftTandemForceTest/MinBrakeForce"/>
                        </xsl:call-template>
                        <xsl:text> - </xsl:text>
                        <xsl:call-template name="convertForce">
                            <xsl:with-param name="FORCE" select="//BrakeLeftTandemForceTest/MaxBrakeForce"/>
                        </xsl:call-template>
                        <xsl:text>]</xsl:text>
                        <xsl:text>  kgf</xsl:text>
                    </td>
                </tr>
                <tr>
                <td align="left">Forca Tandem Direita</td>
                    <td align="left">
                        <xsl:call-template name="convertForce">
                            <xsl:with-param name="FORCE" select="//BrakeRightTandemForceTest/BrakeForce"/>
                        </xsl:call-template>
                    </td>
                    <td align="left">
                        <xsl:text>[</xsl:text>
                        <xsl:call-template name="convertForce">
                            <xsl:with-param name="FORCE" select="//BrakeRightTandemForceTest/MinBrakeForce"/>
                        </xsl:call-template>
                        <xsl:text> - </xsl:text>
                        <xsl:call-template name="convertForce">
                            <xsl:with-param name="FORCE" select="//BrakeRightTandemForceTest/MaxBrakeForce"/>
                        </xsl:call-template>
                        <xsl:text>]</xsl:text>
                        <xsl:text>  kgf</xsl:text>
                    </td>
                    <td>
                    </td>
                    <td>
                    </td>
                </tr>
                <tr>
                <td align="left">Balanco - Dianteiro</td>
                    <td align="left">
                        <xsl:call-template name="showTestNumber">
                            <xsl:with-param name="PARAM" select="//BrakeFrontBalanceTest/Value"/>
                        </xsl:call-template>
                        <xsl:text> %</xsl:text>
                    </td>
                    <td align="left">
                        <xsl:text>[</xsl:text>
                        <xsl:call-template name="showTestParams">
                            <xsl:with-param name="MIN_PARAM" select="//BrakeFrontBalanceTest/MinPercentBalance"/>
                            <xsl:with-param name="MAX_PARAM" select="//BrakeFrontBalanceTest/MaxPercentBalance"/>
                        </xsl:call-template>
                        <xsl:text>]</xsl:text>
                        <xsl:text> %</xsl:text>
                    </td>
                </tr>
                <tr>
                <td align="left">Balanco - Traseiro</td>
                    <td align="left">
                        <xsl:call-template name="showTestNumber">
                            <xsl:with-param name="PARAM" select="//BrakeRearBalanceTest/Value"/>
                        </xsl:call-template>
                        <xsl:text> %</xsl:text>
                    </td>
                    <td align="left">
                        <xsl:text>[</xsl:text>
                        <xsl:call-template name="showTestParams">
                            <xsl:with-param name="MIN_PARAM" select="//BrakeRearBalanceTest/MinPercentBalance"/>
                            <xsl:with-param name="MAX_PARAM" select="//BrakeRearBalanceTest/MaxPercentBalance"/>
                        </xsl:call-template>
                        <xsl:text>]</xsl:text>
                        <xsl:text> %</xsl:text>
                    </td>
                </tr>
                <tr>
                <td align="left">Balanco - Tandem</td>
                    <td align="left">
                        <xsl:call-template name="showTestNumber">
                            <xsl:with-param name="PARAM" select="//BrakeTandemBalanceTest/Value"/>
                        </xsl:call-template>
                        <xsl:text> %</xsl:text>
                    </td>
                    <td align="left">
                        <xsl:text>[</xsl:text>
                        <xsl:call-template name="showTestParams">
                            <xsl:with-param name="MIN_PARAM" select="//BrakeTandemBalanceTest/MinPercentBalance"/>
                            <xsl:with-param name="MAX_PARAM" select="//BrakeTandemBalanceTest/MaxPercentBalance"/>
                        </xsl:call-template>
                        <xsl:text>]</xsl:text>
                        <xsl:text> %</xsl:text>
                    </td>
                </tr>
                <tr>
                <td align="left">Balanco - Dianteiro/Traseiro</td>
                    <td align="left">
                        <xsl:call-template name="showTestNumber">
                            <xsl:with-param name="PARAM" select="//BrakeFrontToRearBalanceTest/Value"/>
                        </xsl:call-template>
                        <xsl:text> %</xsl:text>
                    </td>
                    <td align="left" colspan="2">
                        <xsl:text>[</xsl:text>
                        <xsl:call-template name="showTestParams">
                            <xsl:with-param name="MIN_PARAM" select="//BrakeFrontToRearBalanceTest/MinPercentBalance"/>
                            <xsl:with-param name="MAX_PARAM" select="//BrakeFrontToRearBalanceTest/MaxPercentBalance"/>
                        </xsl:call-template>
                        <xsl:text>]</xsl:text>
                        <xsl:text> %</xsl:text>
                    </td>
                </tr>
                <tr>
                <td align="left">Balanco - Tandem/Traseiro</td>
                    <td align="left">
                        <xsl:call-template name="showTestNumber">
                            <xsl:with-param name="PARAM" select="//BrakeTandemToRearBalanceTest/Value"/>
                        </xsl:call-template>
                        <xsl:text> %</xsl:text>
                    </td>
                    <td align="left" colspan="2">
                        <xsl:text>[</xsl:text>
                        <xsl:call-template name="showTestParams">
                            <xsl:with-param name="MIN_PARAM" select="//BrakeTandemToRearBalanceTest/MinPercentBalance"/>
                            <xsl:with-param name="MAX_PARAM" select="//BrakeTandemToRearBalanceTest/MaxPercentBalance"/>
                        </xsl:call-template>
                        <xsl:text>]</xsl:text>
                        <xsl:text> %</xsl:text>
                    </td>
                </tr>
            </xsl:if>
    
            <!-- Parking Brake Test Results -->
            <xsl:if test="//BrakeAnalyzeParkBrake/@Result!='Pass'">
                <xsl:call-template name="sectionSpacer"/>
                <tr bgcolor="white">
                <th align="center" rowspan="6">Freio de Estacionamento</th>
                </tr>
                <tr>

                    <td align="left" colspan="2">
                    <i>Distancia Maxima</i>
                    </td>
                    <td align="left" colspan="2">
                        <i>
                            <xsl:call-template name="showResult">
                                <xsl:with-param name="RESULT" select="//BrakeAnalyzeParkBrake/MaximumDistance"/>
                            </xsl:call-template>
                        </i>
                    </td>
                </tr>
                <tr>
                <td align="left" colspan="2">Distancia da Roda Esquerda Traseira</td>
                    <td align="left" colspan="2">
                        <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//BrakeAnalyzeParkBrake/LeftRearWheelDistance"/>
                        </xsl:call-template>
                    </td>
                </tr>
                <tr>
                <td align="left" colspan="2">Distancia da Roda Direita Traseira</td>
                    <td align="left" colspan="2">
                        <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//BrakeAnalyzeParkBrake/RightRearWheelDistance"/>
                        </xsl:call-template>
                    </td>
                </tr>
                <tr>
                <td align="left" colspan="2">Distancia da Roda Esquerda Tandem</td>
                    <td align="left" colspan="2">
                        <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//BrakeAnalyzeParkBrake/LeftTandemWheelDistance"/>
                        </xsl:call-template>
                    </td>
                </tr>
                <tr>
                <td align="left" colspan="2">Distancia da Roda Direita Tandem</td>
                    <td align="left" colspan="2">
                        <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//BrakeAnalyzeParkBrake/RightTandemWheelDistance"/>
                        </xsl:call-template>
                    </td>
                </tr>
            </xsl:if>
    
            <!-- Tachograph Test Results -->
            <xsl:if test="//TachographOverallTest/@Result!='Pass'">
                <xsl:call-template name="sectionSpacer"/>
                <tr bgcolor="white">
                <th align="center" rowspan="4">Tacografo</th>
                </tr>
                <tr>
                <td align="left" colspan="2">Quilometros Percorridos</td>
                    <td align="left" colspan="2">
                        <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//TachographTachoDetail/KmTraveled"/>
                        </xsl:call-template>
                        <xsl:text>  km</xsl:text>
                    </td>
                </tr>
                <tr>
                <td align="left" colspan="2">Numero de Pulsos</td>
                    <td align="left" colspan="2">
                        <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//TachographTachoDetail/PulseCount"/>
                        </xsl:call-template>
                        <xsl:text>  pulses</xsl:text>
                    </td>
                </tr>
                <tr>
                <td align="left" colspan="2">Fator W</td>
                    <td align="left" colspan="2">
                        <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//TachographTachoDetail/W"/>
                        </xsl:call-template>
                    </td>
                </tr>
            </xsl:if>

            <!-- Overall Test Results -->
            <xsl:call-template name="categorySpacer"/>
            <tr bgcolor="white">
                <th align="center" rowspan="8">Geral</th>
            </tr>
            <tr bgcolor="white">
                <td align="Left">Geral</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//OverallTest/@Result"/>
                    </xsl:call-template>
                </td>
                <td>
                </td>
                <td align="left">Validacao do Operador</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//MachineDriverPassFail/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td align="left">Freio</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeOverallTest/@Result"/>
                    </xsl:call-template>
                </td>
                <td>
                </td>
                <td align="left">Freio de Estacionamento</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeAnalyzeParkBrake/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td align="left">Eixo</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//AxleOverallTest/@Result"/>
                    </xsl:call-template>
                </td>
                <td>
                </td>
                <td align="left">Tacografo</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TachographOverallTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td align="left">Transmissao</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransmissionOverallTest/@Result"/>
                    </xsl:call-template>
                </td>
                <td>
                </td>
                <td align="left">Diferencial</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//TransferCaseOverallTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td align="left">Emissoes</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//EmissionsOverallTest/@Result"/>
                    </xsl:call-template>
                </td>
                <td>
                </td>
                <td align="left">Maquina</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//MachineOverallTest/@Result"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td align="left">Freio ABS</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//BrakeStaticAbsTest/@Result"/>
                    </xsl:call-template>
                </td>
                <td>
                </td>
                <td align="left"></td>
                <td align="left"></td>
            </tr>
            <tr>
                <td align="left">Bloquero Trans.</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//AxleTransversalLockTest/@Result"/>
                    </xsl:call-template>
                </td>
                <td>
                </td>
                <td align="left">Bloquero Long</td>
                <td align="left">
                    <xsl:call-template name="showResult">
                        <xsl:with-param name="RESULT" select="//AxleLongitudinalLockTest/@Result"/>
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
        <!--xsl:if test="count(//TestResult/AllFailures/child::*)">
            <h3/>
            <table border="1" rules="all" title="All Failures" width="50%">
                <tr bgcolor="white">
                    <th align="center" colspan="3">Falhas do Teste</th>
                </tr>
                <xsl:call-template name="categorySpacer"/>
                <tr bgcolor="white">
                </tr>
                <xsl:for-each select="//TestResult/AllFailures/child::*">
                    <tr>
                        <td align="Left">
                            <xsl:value-of select="@Description"/>
                        </td>
                    </tr>
                </xsl:for-each>
            </table>
        </xsl:if-->
        <xsl:if test="count(//TestResult/AllFailures/child::*)">
            <h3/>
            <table border="1" rules="all" title="All Failures" width="50%">
                <tr bgcolor="white">
                    <th align="center" colspan="3">Falhas do Teste</th>
                </tr>
                <xsl:call-template name="categorySpacer"/>
                <tr bgcolor="white">
                    <th align="center">Teste</th>
                    <th align="center">Resultados do Teste</th>
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
                <xsl:text>----</xsl:text>
            </xsl:when>
            <xsl:when test="$RESULT = 'TestNotRequired'">
                <xsl:text>Nao Exigida</xsl:text>
            </xsl:when>
            <xsl:when test="$RESULT = 'Yes'">
                <xsl:text>Passou</xsl:text>
            </xsl:when>
            <xsl:when test="$RESULT = 'No'">
                <xsl:text>Falhou</xsl:text>
            </xsl:when>
            <xsl:when test="$RESULT = 'Pass'">
                <xsl:text>Passou</xsl:text>
            </xsl:when>
            <xsl:when test="$RESULT = 'Fail'">
                <xsl:text>Falhou</xsl:text>
            </xsl:when>
            <xsl:when test="$RESULT = 'Abort'">
                <xsl:text>Abortado</xsl:text>
            </xsl:when>
            <xsl:when test="$RESULT = 'TestInProgress'">
                <xsl:text>----</xsl:text>
            </xsl:when>
            <xsl:when test="$RESULT = 'Timeout'">
                <xsl:text>Estouro de Tempo</xsl:text>
            </xsl:when>
            <xsl:when test="$RESULT = 'Testing'">
                <xsl:text>----</xsl:text>
            </xsl:when>
            <xsl:when test="$RESULT = 'Not Tested'">
                <xsl:text>----</xsl:text>
            </xsl:when>
            <xsl:when test="$RESULT = 'Skip'">
                <xsl:text>----</xsl:text>
            </xsl:when>
            <xsl:when test="$RESULT != ''">
                <xsl:value-of select="$RESULT"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>----</xsl:text>
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
        <xsl:text> - </xsl:text>
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
        <xsl:choose>
            <!-- The force value is a valid number -->
            <xsl:when test="string(number($FORCE)) != 'NaN'">
                <xsl:value-of select="format-number(number($FORCE)*0.45359237,'#.##')"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>---</xsl:text>
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
                <xsl:text>---</xsl:text>
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

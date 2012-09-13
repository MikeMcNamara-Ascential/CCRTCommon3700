<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:template match="/BepVehicleTestResult">
        <fo:root xmlns:fo="http://www.w3.org/1999/XSL/Format">
            <fo:layout-master-set>
                <fo:simple-page-master master-name="LetterPage" page-width="3.9in" page-height="1.9in">
                    <!-- Page template goes here -->
                    <fo:region-body region-name="PageBody" margin="0.8in"/>
                </fo:simple-page-master>
            </fo:layout-master-set>
            <fo:page-sequence master-reference="LetterPage">
                <!-- Page content goes here -->
                <fo:flow flow-name="PageBody">
                    <fo:table border-collapse="collapse" table-layout="fixed" text-align="center" width="100%">
                        <fo:table-column column-width="3in" />
                        <fo:table-body>
                            <fo:table-row display-align="center">
                                <fo:table-cell>
                                    <fo:block font-weight="bold">Bolero Roller and Brake Testing Report</fo:block>
                                    <fo:block font-size="10pt" margin-bottom="1pt" line-height="12pt" margin-top="1pt" color="rgb(0,0,0,255)" >
                                        <fo:leader leader-pattern="space" speak-numeral="1.001" content-height="12pt" leader-length="2.001%" >
                                            <!-- Blank line -->
                                        </fo:leader>
                                    </fo:block>
                                </fo:table-cell>
                            </fo:table-row>
                        </fo:table-body>
                    </fo:table>
                    <fo:table border-collapse="collapse" table-layout="fixed" text-align="left" width="100%">
                        <fo:table-column column-width="0.75in" />
                        <fo:table-column column-width="1.25in" />
                        <fo:table-column column-width="0.50in" />
                        <fo:table-column column-width="0.50in" />
                        <fo:table-body>
                            <fo:table-row>
                                <fo:table-cell>
                                    <fo:block font-size="8pt">VIN:</fo:block>
                                </fo:table-cell>
                                <fo:table-cell padding-left="1pt">
                                    <fo:block font-size="8pt">
                                        <xsl:value-of select="//VIN"/>
                                    </fo:block>
                                </fo:table-cell>
                                <fo:table-cell>
                                    <fo:block font-size="8pt">Date:</fo:block>
                                </fo:table-cell>
                                <fo:table-cell padding-left="1pt">
                                    <fo:block font-size="8pt">
                                        <xsl:value-of select="//TestResult/MachineInitialize/Date"/>
                                    </fo:block>
                                </fo:table-cell>
                            </fo:table-row>
                            <fo:table-row>
                                <fo:table-cell>
                                    <fo:block font-size="10pt" margin-bottom="1pt" line-height="12pt" margin-top="1pt" color="rgb(0,0,0,255)" >
                                        <fo:leader leader-pattern="space" speak-numeral="1.001" content-height="12pt" leader-length="2.001%" >
                                            <!-- Blank line -->
                                        </fo:leader>
                                    </fo:block>
                                </fo:table-cell>
                                <fo:table-cell padding-left="1pt">
                                    <fo:block font-size="10pt" margin-bottom="1pt" line-height="12pt" margin-top="1pt" color="rgb(0,0,0,255)" >
                                        <fo:leader leader-pattern="space" speak-numeral="1.001" content-height="12pt" leader-length="2.001%" >
                                            <!-- Blank line -->
                                        </fo:leader>
                                    </fo:block>
                                </fo:table-cell>
                                <fo:table-cell>
                                    <fo:block font-size="8pt">Time:</fo:block>
                                    <fo:block font-size="10pt" margin-bottom="1pt" line-height="12pt" margin-top="1pt" color="rgb(0,0,0,255)" >
                                        <fo:leader leader-pattern="space" speak-numeral="1.001" content-height="12pt" leader-length="2.001%" >
                                            <!-- Blank line -->
                                        </fo:leader>
                                    </fo:block>
                                </fo:table-cell>
                                <fo:table-cell padding-left="1pt">
                                    <fo:block font-size="8pt">
                                        <xsl:value-of select="//TestResult/MachineInitialize/Time"/>
                                    </fo:block>
                                    <fo:block font-size="10pt" margin-bottom="1pt" line-height="12pt" margin-top="1pt" color="rgb(0,0,0,255)" >
                                        <fo:leader leader-pattern="space" speak-numeral="1.001" content-height="12pt" leader-length="2.001%" >
                                            <!-- Blank line -->
                                        </fo:leader>
                                    </fo:block>
                                </fo:table-cell>
                            </fo:table-row>
                        </fo:table-body>
                    </fo:table>

                    <!-- Test Results -->
                    <fo:table border-collapse="collapse" table-layout="fixed" text-align="left" width="100%">
                        <fo:table-column column-width="0.50in" />
                        <fo:table-column column-width="0.50in" />
                        <fo:table-column column-width="0.50in" />
                        <fo:table-column column-width="0.50in" />
                        <fo:table-column column-width="0.50in" />
                        <fo:table-column column-width="0.50in" />
                        <fo:table-body>
                            <fo:table-row>
                                <fo:table-cell>
                                    <fo:block font-size="8pt">Gear 1:</fo:block>
                                </fo:table-cell>
                                <fo:table-cell padding-left="1pt">
                                    <fo:block font-size="8pt">
                                        <xsl:call-template name="showResult">
                                            <xsl:with-param name="RESULT" select="//TransmissionBlingGearSpeedTest_1/@Result"/>
                                        </xsl:call-template>
                                    </fo:block>
                                </fo:table-cell>
                                <fo:table-cell>
                                    <fo:block font-size="8pt">LHFR:</fo:block>
                                </fo:table-cell>
                                <fo:table-cell padding-left="1pt">
                                    <fo:block font-size="8pt">
                                        <xsl:call-template name="showResult">
                                            <xsl:with-param name="RESULT" select="//BrakeLeftFrontForceTest/@Result"/>
                                        </xsl:call-template>
                                    </fo:block>
                                </fo:table-cell>
                                <fo:table-cell>
                                    <fo:block font-size="8pt">4WDH:</fo:block>
                                </fo:table-cell>
                                <fo:table-cell padding-left="1pt">
                                    <fo:block font-size="8pt">
                                        <xsl:call-template name="showResult">
                                            <xsl:with-param name="RESULT" select="//TransferCaseCheck4HIAxleBalance/@Result"/>
                                        </xsl:call-template>
                                    </fo:block>
                                </fo:table-cell>
                            </fo:table-row>

                            <fo:table-row>
                                <fo:table-cell>
                                    <fo:block font-size="8pt">Gear 2:</fo:block>
                                </fo:table-cell>
                                <fo:table-cell padding-left="1pt">
                                    <fo:block font-size="8pt">
                                        <xsl:call-template name="showResult">
                                            <xsl:with-param name="RESULT" select="//TransmissionSpeedBasedGearTest_2/@Result"/>
                                        </xsl:call-template>
                                    </fo:block>
                                </fo:table-cell>
                                <fo:table-cell>
                                    <fo:block font-size="8pt">LHRR:</fo:block>
                                </fo:table-cell>
                                <fo:table-cell padding-left="1pt">
                                    <fo:block font-size="8pt">
                                        <xsl:call-template name="showResult">
                                            <xsl:with-param name="RESULT" select="//BrakeLeftRearForceTest/@Result"/>
                                        </xsl:call-template>
                                    </fo:block>
                                </fo:table-cell>
                                <fo:table-cell>
                                    <fo:block font-size="8pt">4WDL:</fo:block>
                                </fo:table-cell>
                                <fo:table-cell padding-left="1pt">
                                    <fo:block font-size="8pt">
                                        <xsl:call-template name="showResult">
                                            <xsl:with-param name="RESULT" select="//TransferCaseCheck4LOAxleBalance/@Result"/>
                                        </xsl:call-template>
                                    </fo:block>
                                </fo:table-cell>
                            </fo:table-row>

                            <fo:table-row>
                                <fo:table-cell>
                                    <fo:block font-size="8pt">Gear 3:</fo:block>
                                </fo:table-cell>
                                <fo:table-cell padding-left="1pt">
                                    <fo:block font-size="8pt">
                                        <xsl:call-template name="showResult">
                                            <xsl:with-param name="RESULT" select="//TransmissionSpeedBasedGearTest_3/@Result"/>
                                        </xsl:call-template>
                                    </fo:block>
                                </fo:table-cell>
                                <fo:table-cell>
                                    <fo:block font-size="8pt">RHFR:</fo:block>
                                </fo:table-cell>
                                <fo:table-cell padding-left="1pt">
                                    <fo:block font-size="8pt">
                                        <xsl:call-template name="showResult">
                                            <xsl:with-param name="RESULT" select="//BrakeRightFrontForceTest/@Result"/>
                                        </xsl:call-template>
                                    </fo:block>
                                </fo:table-cell>
                                <fo:table-cell>
                                    <fo:block font-size="8pt">RWAL:</fo:block>
                                </fo:table-cell>
                                <fo:table-cell padding-left="1pt">
                                    <fo:block font-size="8pt">
                                        <xsl:call-template name="showResult">
                                            <xsl:with-param name="RESULT" select="//BrakeRwalSensorTest/@Result"/>
                                        </xsl:call-template>
                                    </fo:block>
                                </fo:table-cell>
                            </fo:table-row>

                            <fo:table-row>
                                <fo:table-cell>
                                    <fo:block font-size="8pt">Gear 4:</fo:block>
                                </fo:table-cell>
                                <fo:table-cell padding-left="1pt">
                                    <fo:block font-size="8pt">
                                        <xsl:call-template name="showResult">
                                            <xsl:with-param name="RESULT" select="//TransmissionBlingGearSpeedTest_4/@Result"/>
                                        </xsl:call-template>
                                    </fo:block>
                                </fo:table-cell>
                                <fo:table-cell>
                                    <fo:block font-size="8pt">RHRR:</fo:block>
                                </fo:table-cell>
                                <fo:table-cell padding-left="1pt">
                                    <fo:block font-size="8pt">
                                        <xsl:call-template name="showResult">
                                            <xsl:with-param name="RESULT" select="//BrakeRightRearForceTest/@Result"/>
                                        </xsl:call-template>
                                    </fo:block>
                                </fo:table-cell>
                                <fo:table-cell>
                                    <fo:block font-size="8pt">Axle Ratio:</fo:block>
                                </fo:table-cell>
                                <fo:table-cell padding-left="1pt">
                                    <fo:block font-size="8pt">
                                        <xsl:call-template name="showResult">
                                            <xsl:with-param name="RESULT" select="//TransferCaseCheckAxleRatio/@Result"/>
                                        </xsl:call-template>
                                    </fo:block>
                                </fo:table-cell>
                            </fo:table-row>

                            <fo:table-row>
                                <fo:table-cell>
                                    <fo:block font-size="8pt">Gear 5:</fo:block>
                                </fo:table-cell>
                                <fo:table-cell padding-left="1pt">
                                    <fo:block font-size="8pt">
                                        <xsl:call-template name="showResult">
                                            <xsl:with-param name="RESULT" select="//TransmissionSpeedBasedGearTest_5/@Result"/>
                                        </xsl:call-template>
                                    </fo:block>
                                </fo:table-cell>
                                <fo:table-cell>
                                    <fo:block font-size="8pt">FR Balance:</fo:block>
                                </fo:table-cell>
                                <fo:table-cell padding-left="1pt">
                                    <fo:block font-size="8pt">
                                        <xsl:call-template name="showResult">
                                            <xsl:with-param name="RESULT" select="//BrakeFrontBalanceTest/@Result"/>
                                        </xsl:call-template>
                                    </fo:block>
                                </fo:table-cell>
                                <fo:table-cell>
                                    <fo:block font-size="8pt">Park Brake:</fo:block>
                                </fo:table-cell>
                                <fo:table-cell padding-left="1pt">
                                    <fo:block font-size="8pt">
                                        <xsl:call-template name="showResult">
                                            <xsl:with-param name="RESULT" select="//BrakeAnalyzeDynamicParkBrake/@Result"/>
                                        </xsl:call-template>
                                    </fo:block>
                                </fo:table-cell>
                            </fo:table-row>

                            <fo:table-row>
                                <fo:table-cell/>
                                <fo:table-cell padding-left="1pt"/>
                                <fo:table-cell>
                                    <fo:block font-size="8pt">RR Balance:</fo:block>
                                </fo:table-cell>
                                <fo:table-cell padding-left="1pt">
                                    <fo:block font-size="8pt">
                                        <xsl:call-template name="showResult">
                                            <xsl:with-param name="RESULT" select="//BrakeRearBalanceTest/@Result"/>
                                        </xsl:call-template>
                                    </fo:block>
                                </fo:table-cell>
                                <fo:table-cell/>
                                <fo:table-cell padding-left="1pt"/>
                            </fo:table-row>

                            <fo:table-row>
                                <fo:table-cell number-columns-spanned="2">
                                    <fo:block font-size="10pt" margin-bottom="1pt" line-height="12pt" margin-top="1pt" color="rgb(0,0,0,255)" >
                                        <fo:leader leader-pattern="space" speak-numeral="1.001" content-height="12pt" leader-length="2.001%" >
                                            <!-- Blank line -->
                                        </fo:leader>
                                    </fo:block>
                                    <fo:block font-size="8pt">Test Result:</fo:block>
                                </fo:table-cell>
                                <fo:table-cell padding-left="1pt" number-columns-spanned="2">
                                    <fo:block font-size="10pt" margin-bottom="1pt" line-height="12pt" margin-top="1pt" color="rgb(0,0,0,255)" >
                                        <fo:leader leader-pattern="space" speak-numeral="1.001" content-height="12pt" leader-length="2.001%" >
                                            <!-- Blank line -->
                                        </fo:leader>
                                    </fo:block>
                                    <fo:block font-size="8pt">
                                        <xsl:call-template name="showResult">
                                            <xsl:with-param name="RESULT" select="//OverallTest/@Result"/>
                                        </xsl:call-template>
                                    </fo:block>
                                </fo:table-cell>
                            </fo:table-row>

                        </fo:table-body>
                    </fo:table>

                </fo:flow>
            </fo:page-sequence>
        </fo:root>
    </xsl:template>
    
    <!-- Callout to allow cutomization of test result flags -->
    <xsl:template name="showResult">
         <xsl:param name="RESULT"/>
              <xsl:choose>
                   <xsl:when test="$RESULT != ''">
                        <xsl:value-of select="$RESULT"/>
                   </xsl:when>
                   <xsl:otherwise>
                        <xsl:text/>
                   </xsl:otherwise>
              </xsl:choose>
    </xsl:template>


</xsl:stylesheet>

<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
     <xsl:template match="/BepVehicleTestResult">
          <!--?xml version="1.0" encoding="ISO-8859-1"?-->
          <fo:root xmlns:fo="http://www.w3.org/1999/XSL/Format">
               <fo:layout-master-set>
                    <fo:simple-page-master master-name="LetterPage" page-width="8.5in" page-height="11in">
                         <!-- Page template goes here -->
                         <fo:region-body region-name="PageBody" margin="0.3in"/>
                    </fo:simple-page-master>
               </fo:layout-master-set>
               <fo:page-sequence master-reference="LetterPage">
                    <!-- Page content goes here -->
                    <fo:flow flow-name="PageBody">
                         <fo:block font-weight="bold">Chassis End of Line Testing Report
                              <fo:block></fo:block>
                         </fo:block>
                         <!-- Build Info -->
                         <fo:table border-collapse="collapse" table-layout="fixed" text-align="center" width="100%">
                              <fo:table-column column-width="1.7in" />
                              <fo:table-column column-width="2in" />
                              <fo:table-column column-width="1.5in" />
                              <fo:table-column column-width="1in" />
                              <fo:table-header>
                                   <fo:table-row>
                                        <fo:table-cell padding-left="1pt" border="1pt solid black" number-columns-spanned="4">
                                             <fo:block>Chassis Data</fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                              </fo:table-header>
                              <fo:table-body>
                                   <fo:table-row>
                                        <fo:table-cell padding-left="1pt" border="1pt solid black">
                                             <fo:block>Date</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell padding-left="1pt" border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//TestResult/MachineInitialize/Date"/>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell padding-left="1pt" border="1pt solid black">
                                             <fo:block>Time</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell padding-left="1pt" border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//TestResult/MachineInitialize/Time"/>
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell padding-left="1pt" border="1pt solid black">
                                             <fo:block>Chassis #/VIN</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell padding-left="1pt" border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//VIN"/>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell padding-left="1pt" border="1pt solid black">
                                             <fo:block>Chassis Model</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell padding-left="1pt" border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//ChassisModel"/>
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell padding-left="1pt" border="1pt solid black">
                                             <fo:block>Engine #</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell padding-left="1pt" border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//EngineNumber"/>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell padding-left="1pt" border="1pt solid black">
                                             <fo:block>Engine Type</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell padding-left="1pt" border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//EngineType"/>
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell padding-left="1pt" border="1pt solid black">
                                             <fo:block>Axle Ratio</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell padding-left="1pt" border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//AxleRatio"/>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell padding-left="1pt" border="1pt solid black">
                                             <fo:block>Tire Size</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell padding-left="1pt" border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//TireSize"/>
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell padding-left="1pt" border="1pt solid black">
                                             <fo:block>Axle Center to Center</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell padding-left="1pt" border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="convertWheelbase">
                                                       <xsl:with-param name="VALUE" select="//WheelbasePositionInchesX10"/>
                                                  </xsl:call-template>
                                                  mm
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell padding-left="1pt" border="1pt solid black">
                                             <fo:block>Tested By</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell padding-left="1pt" border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//DriverNumber"/>
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                              </fo:table-body>
                         </fo:table>
                         <!-- Test Results -->
                         <fo:table border-collapse="collapse" table-layout="fixed" text-align="center" width="100%">
                              <fo:table-column column-width="1.8in" />
                              <fo:table-column column-width="1.2in" />
                              <fo:table-column column-width="0.6in" />
                              <fo:table-column column-width="1.2in" />
                              <fo:table-column column-width="1.1in" />
                              <fo:table-column column-width="0.6in" />
                              <fo:table-column column-width="1.2in" />
                              <fo:table-header>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="2">
                                             <fo:block/>
                                        </fo:table-cell>
                                        <fo:table-cell padding-left="1pt" border="1pt solid black">
                                             <fo:block>Result</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell padding-left="1pt" border="1pt solid black">
                                             <fo:block>Measured</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell padding-left="1pt" border="1pt solid black">
                                             <fo:block/>
                                        </fo:table-cell>
                                        <fo:table-cell padding-left="1pt" border="1pt solid black">
                                             <fo:block>Result</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell padding-left="1pt" border="1pt solid black">
                                             <fo:block>Measured</fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                              </fo:table-header>
                              <fo:table-body>
                                   <!-- Side Slip Test -->
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black" number-rows-spanned="2" display-align="center">
                                             <fo:block font-weight="bold" text-align="center">1. SIDE SLIP TEST</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Left Toe In</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//SideSlip/FrontSideSlipValue/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//SideSlip/FrontSideSlipValue"/>
                                                  mm/m
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Right Toe In</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//SideSlip/RearSideSlipValue/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//SideSlip/RearSideSlipValue"/>
                                                  mm/m
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Overall Result</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="5">
                                             <fo:block text-align="center" font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//SideSlipOverallTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <!-- Lock to Lock Test -->
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black" number-rows-spanned="2" display-align="center">
                                             <fo:block font-weight="bold" text-align="center">2. LOCK TO LOCK TEST</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Left Angle</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//HLA/LeftWheelLeftAngleResult"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//HLA/LeftWheelLeftAngle"/>
                                                  deg
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Right Angle</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//HLA/RightWheelLeftAngleResult"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//HLA/RightWheelLeftAngle"/>
                                                  deg
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Overall Result</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="5">
                                             <fo:block text-align="center" font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//HLAOverallSASTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   
                                   <!-- Headlight Test -->
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black" number-rows-spanned="11" display-align="center">
                                             <fo:block font-weight="bold" text-align="center">3. HEADLIGHT TEST</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="3">
                                             <fo:block font-weight="bold" text-align="center">Left Low Beam</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="3">
                                             <fo:block font-weight="bold" text-align="center">Right Low Beam</fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Intensity</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//HLA/LeftLampLuminousIntensity/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//HLA/LeftLampLuminousIntensity"/>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Intensity</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//HLA/RightLampLuminousIntensity/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//HLA/RightLampLuminousIntensity"/>
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Inclination</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//HLA/LeftLampUpDownDeviationAngle/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//HLA/LeftLampUpDownDeviationAngle"/>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Inclination</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//HLA/RightLampUpDownDeviationAngle/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//HLA/RightLampUpDownDeviationAngle"/>
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Direction</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//HLA/LeftLampLeftRightDeviationAngle/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//HLA/LeftLampLeftRightDeviationAngle"/>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Direction</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//HLA/RightLampLeftRightDeviationAngle/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//HLA/RightLampLeftRightDeviationAngle"/>
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Blinding</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//HLA/LeftLampHeight/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//HLA/LeftLampHeight"/>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Blinding</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//HLA/RightLampHeight/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//HLA/RightLampHeight"/>
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="3">
                                             <fo:block font-weight="bold" text-align="center">Left High Beam</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="3">
                                             <fo:block font-weight="bold" text-align="center">Right High Beam</fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Intensity</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//HLA/LeftLampHighLuminousIntensity/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//HLA/LeftLampHighLuminousIntensity"/>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Intensity</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//HLA/RightLampHighLuminousIntensity/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//HLA/RightLampHighLuminousIntensity"/>
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Inclination</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//HLA/LeftLampHighUpDownDeviationAngle/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//HLA/LeftLampHighUpDownDeviationAngle"/>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Inclination</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//HLA/RightLampHighUpDownDeviationAngle/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//HLA/RightLampHighUpDownDeviationAngle"/>
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Direction</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//HLA/LeftLampHighLeftRightDeviationAngle/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//HLA/LeftLampHighLeftRightDeviationAngle"/>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Direction</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//HLA/RightLampHighLeftRightDeviationAngle/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//HLA/RightLampHighLeftRightDeviationAngle"/>
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Blinding</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//HLA/LeftLampHighHeight/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//HLA/LeftLampHighHeight"/>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Blinding</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//HLA/RightLampHighHeight/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//HLA/RightLampHighHeight"/>
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Overall Result</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="5">
                                             <fo:block text-align="center" font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//HLAOverallHeadlightTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                              </fo:table-body>
                         </fo:table>
                         <!-- Base Brake Test Results -->
                         <fo:table border-collapse="collapse" table-layout="fixed" text-align="center" width="100%">
                              <fo:table-column column-width="1.6in" />
                              <fo:table-column column-width="0.5in" />
                              <fo:table-column column-width="1in" />
                              <fo:table-column column-width="0.5in" />
                              <fo:table-column column-width="1in" />
                              <fo:table-column column-width="0.5in" />
                              <fo:table-column column-width="1in" />
                              <fo:table-column column-width="0.5in" />
                              <fo:table-column column-width="1in" />
                              <fo:table-body text-align="center">
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="9" display-align="center" padding-left="2pt">
                                             <fo:block font-weight="bold" text-align="left">4. BASE BRAKE TEST</fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block/>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="2">
                                             <fo:block>Axle 1 (Front 1)</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="2">
                                             <fo:block>Axle 2 (Front 2)</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="2">
                                             <fo:block>Axle 3 (Rear 1)</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="2">
                                             <fo:block>Axle 4 (Rear 2)</fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Max Brake Force L</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeTandemAxleLeftAxle1ForceTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="convertForce">
                                                       <xsl:with-param name="FORCE" select="//BrakeTandemAxleLeftAxle1ForceTest/BrakeForce"/>
                                                       <xsl:with-param name="UNITS" select="//BrakeTandemAxleLeftAxle1ForceTest/BrakeForce/@Units"/>
                                                  </xsl:call-template>
                                                  KGF
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeTandemAxleLeftAxle2ForceTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="convertForce">
                                                       <xsl:with-param name="FORCE" select="//BrakeTandemAxleLeftAxle2ForceTest/BrakeForce"/>
                                                       <xsl:with-param name="UNITS" select="//BrakeTandemAxleLeftAxle2ForceTest/BrakeForce/@Units"/>
                                                  </xsl:call-template>
                                                  KGF
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeTandemAxleLeftFrontForceTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="convertForce">
                                                       <xsl:with-param name="FORCE" select="//BrakeTandemAxleLeftFrontForceTest/BrakeForce"/>
                                                       <xsl:with-param name="UNITS" select="//BrakeTandemAxleLeftFrontForceTest/BrakeForce/@Units"/>
                                                  </xsl:call-template>
                                                  KGF
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeTandemAxleLeftRearForceTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="convertForce">
                                                       <xsl:with-param name="FORCE" select="//BrakeTandemAxleLeftRearForceTest/BrakeForce"/>
                                                       <xsl:with-param name="UNITS" select="//BrakeTandemAxleLeftRearForceTest/BrakeForce/@Units"/>
                                                  </xsl:call-template>
                                                  KGF
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Max Brake Force R</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeTandemAxleRightAxle1ForceTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="convertForce">
                                                       <xsl:with-param name="FORCE" select="//BrakeTandemAxleRightAxle1ForceTest/BrakeForce"/>
                                                       <xsl:with-param name="UNITS" select="//BrakeTandemAxleRightAxle1ForceTest/BrakeForce/@Units"/>
                                                  </xsl:call-template>
                                                  KGF
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeTandemAxleRightAxle2ForceTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="convertForce">
                                                       <xsl:with-param name="FORCE" select="//BrakeTandemAxleRightAxle2ForceTest/BrakeForce"/>
                                                       <xsl:with-param name="UNITS" select="//BrakeTandemAxleRightAxle2ForceTest/BrakeForce/@Units"/>
                                                  </xsl:call-template>
                                                  KGF
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeTandemAxleRightFrontForceTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="convertForce">
                                                       <xsl:with-param name="FORCE" select="//BrakeTandemAxleRightFrontForceTest/BrakeForce"/>
                                                       <xsl:with-param name="UNITS" select="//BrakeTandemAxleRightFrontForceTest/BrakeForce/@Units"/>
                                                  </xsl:call-template>
                                                  KGF
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeTandemAxleRightRearForceTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="convertForce">
                                                       <xsl:with-param name="FORCE" select="//BrakeTandemAxleRightRearForceTest/BrakeForce"/>
                                                       <xsl:with-param name="UNITS" select="//BrakeTandemAxleRightRearForceTest/BrakeForce/@Units"/>
                                                  </xsl:call-template>
                                                  KGF
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Drag Force L</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeTandemAxleLeftAxle1DragTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="convertForce">
                                                       <xsl:with-param name="FORCE" select="//BrakeTandemAxleLeftAxle1DragTest/DragForce"/>
                                                       <xsl:with-param name="UNITS" select="//BrakeTandemAxleLeftAxle1DragTest/DragForce/@Units"/>
                                                  </xsl:call-template>
                                                  KGF
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeTandemAxleLeftAxle2DragTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="convertForce">
                                                       <xsl:with-param name="FORCE" select="//BrakeTandemAxleLeftAxle2DragTest/DragForce"/>
                                                       <xsl:with-param name="UNITS" select="//BrakeTandemAxleLeftAxle2DragTest/DragForce/@Units"/>
                                                  </xsl:call-template>
                                                  KGF
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeTandemAxleLeftFrontDragTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="convertForce">
                                                       <xsl:with-param name="FORCE" select="//BrakeTandemAxleLeftFrontDragTest/DragForce"/>
                                                       <xsl:with-param name="UNITS" select="//BrakeTandemAxleLeftFrontDragTest/DragForce/@Units"/>
                                                  </xsl:call-template>
                                                  KGF
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeTandemAxleLeftRearDragTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="convertForce">
                                                       <xsl:with-param name="FORCE" select="//BrakeTandemAxleLeftRearDragTest/DragForce"/>
                                                       <xsl:with-param name="UNITS" select="//BrakeTandemAxleLeftRearDragTest/DragForce/@Units"/>
                                                  </xsl:call-template>
                                                  KGF
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Drag Force R</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeTandemAxleRightAxle1DragTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="convertForce">
                                                       <xsl:with-param name="FORCE" select="//BrakeTandemAxleRightAxle1DragTest/DragForce"/>
                                                       <xsl:with-param name="UNITS" select="//BrakeTandemAxleRightAxle1DragTest/DragForce/@Units"/>
                                                  </xsl:call-template>
                                                  KGF
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeTandemAxleRightAxle2DragTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="convertForce">
                                                       <xsl:with-param name="FORCE" select="//BrakeTandemAxleRightAxle2DragTest/DragForce"/>
                                                       <xsl:with-param name="UNITS" select="//BrakeTandemAxleRightAxle2DragTest/DragForce/@Units"/>
                                                  </xsl:call-template>
                                                  KGF
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeTandemAxleRightFrontDragTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="convertForce">
                                                       <xsl:with-param name="FORCE" select="//BrakeTandemAxleRightFrontDragTest/DragForce"/>
                                                       <xsl:with-param name="UNITS" select="//BrakeTandemAxleRightFrontDragTest/DragForce/@Units"/>
                                                  </xsl:call-template>
                                                  KGF
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeTandemAxleRightRearDragTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="convertForce">
                                                       <xsl:with-param name="FORCE" select="//BrakeTandemAxleRightRearDragTest/DragForce"/>
                                                       <xsl:with-param name="UNITS" select="//BrakeTandemAxleRightRearDragTest/DragForce/@Units"/>
                                                  </xsl:call-template>
                                                  KGF
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Block Difference</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="2">
                                             <fo:block>
                                                  <xsl:call-template name="showBlockDifference">
                                                       <xsl:with-param name="WHEEL_NAME1" select="//BrakeTandemAxleLeftAxle1ForceTest/BrakeForce"/>
                                                       <xsl:with-param name="WHEEL_NAME2" select="//BrakeTandemAxleRightAxle1ForceTest/BrakeForce"/>
                                                  </xsl:call-template>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="2">
                                             <fo:block>
                                                  <xsl:call-template name="showBlockDifference">
                                                       <xsl:with-param name="WHEEL_NAME1" select="//BrakeTandemAxleLeftAxle2ForceTest/BrakeForce"/>
                                                       <xsl:with-param name="WHEEL_NAME2" select="//BrakeTandemAxleRightAxle2ForceTest/BrakeForce"/>
                                                  </xsl:call-template>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="2">
                                             <fo:block>
                                                  <xsl:call-template name="showBlockDifference">
                                                       <xsl:with-param name="WHEEL_NAME1" select="//BrakeTandemAxleLeftFrontForceTest/BrakeForce"/>
                                                       <xsl:with-param name="WHEEL_NAME2" select="//BrakeTandemAxleRightFrontForceTest/BrakeForce"/>
                                                  </xsl:call-template>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="2">
                                             <fo:block>
                                                  <xsl:call-template name="showBlockDifference">
                                                       <xsl:with-param name="WHEEL_NAME1" select="//BrakeTandemAxleLeftRearForceTest/BrakeForce"/>
                                                       <xsl:with-param name="WHEEL_NAME2" select="//BrakeTandemAxleRightRearForceTest/BrakeForce"/>
                                                  </xsl:call-template>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>L/R Balance</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" font-style="italic">
                                             <fo:block>
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeTandemAxleAxle1BalanceTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//BrakeTandemAxleAxle1BalanceTest/Value"/>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" font-style="italic">
                                             <fo:block>
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeTandemAxleAxle2BalanceTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//BrakeTandemAxleAxle2BalanceTest/Value"/>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeTandemAxleFrontBalanceTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//BrakeTandemAxleFrontBalanceTest/Value"/>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeRearBalanceTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//BrakeRearBalanceTest/Value"/>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="9">
                                             <fo:block>-</fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Axle 3/4 Balance</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeFrontToRearBalanceTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//BrakeFrontToRearBalanceTest/Value"/>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="6">
                                             <fo:block/>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Total Brake Force</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="2">
                                             <fo:block>
                                                  <xsl:call-template name="addForces">
                                                       <xsl:with-param name="FORCE1" select="//BrakeTandemAxleLeftFrontForceTest/BrakeForce"/>
                                                       <xsl:with-param name="FORCE2" select="//BrakeTandemAxleRightFrontForceTest/BrakeForce"/>
                                                       <xsl:with-param name="FORCE3" select="//BrakeTandemAxleLeftRearForceTest/BrakeForce"/>
                                                       <xsl:with-param name="FORCE4" select="//BrakeTandemAxleRightRearForceTest/BrakeForce"/>
                                                       <xsl:with-param name="FORCE5" select="//BrakeTandemAxleLeftAxle1ForceTest/BrakeForce"/>
                                                       <xsl:with-param name="FORCE6" select="//BrakeTandemAxleRightAxle1ForceTest/BrakeForce"/>
                                                       <xsl:with-param name="FORCE7" select="//BrakeTandemAxleLeftAxle2ForceTest/BrakeForce"/>
                                                       <xsl:with-param name="FORCE8" select="//BrakeTandemAxleRightAxle2ForceTest/BrakeForce"/>
                                                       <xsl:with-param name="UNITS" select="//BrakeTandemAxleRightRearForceTest/BrakeForce/@Units"/>
                                                  </xsl:call-template>
                                                  KGF
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="2">
                                             <fo:block/>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="2">
                                             <fo:block>Brake Efficiency</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="2">
                                             <fo:block>
                                                  <xsl:call-template name="showBrakeEfficiency">
                                                       <xsl:with-param name="UNLADEN_CHASSIS_WEIGHT" select="//VehicleBuild/UnladenChassisWeight"/>
                                                  </xsl:call-template>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Overall Result</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="8">
                                             <fo:block text-align="center" font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeBrakeTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="9" display-align="center" padding-left="2pt">
                                             <fo:block font-weight="bold" text-align="left">5. ABS TEST</fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block/>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="2">
                                             <fo:block>Axle 1 Left</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="2">
                                             <fo:block>Axle 1 Right</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="2">
                                             <fo:block>Axle 2 Left</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="2">
                                             <fo:block>Axle 2 Right</fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Reduction</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeAxle1LeftReductionTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//BrakeAxle1LeftReductionTest/Value"/>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeAxle1RightReductionTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//BrakeAxle1RightReductionTest/Value"/>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeAxle2LeftReductionTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//BrakeAxle2LeftReductionTest/Value"/>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeAxle2RightReductionTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//BrakeAxle2RightReductionTest/Value"/>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Recovery</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeAxle1LeftRecoveryTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//BrakeAxle1LeftRecoveryTest/Value"/>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeAxle1RightRecoveryTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//BrakeAxle1RightRecoveryTest/Value"/>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeAxle2LeftRecoveryTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//BrakeAxle2LeftRecoveryTest/Value"/>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeAxle2RightRecoveryTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//BrakeAxle2RightRecoveryTest/Value"/>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="9">
                                             <fo:block>-</fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block/>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="2">
                                             <fo:block>Axle 3 Left</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="2">
                                             <fo:block>Axle 3 Right</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="2">
                                             <fo:block>Axle 4 Left</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="2">
                                             <fo:block>Axle 4 Right</fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Reduction</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeAxle3LeftReductionTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//BrakeAxle3LeftReductionTest/Value"/>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeAxle3RightReductionTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//BrakeAxle3RightReductionTest/Value"/>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeAxle4LeftReductionTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//BrakeAxle4LeftReductionTest/Value"/>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeAxle4RightReductionTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//BrakeAxle4RightReductionTest/Value"/>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Recovery</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeAxle3LeftRecoveryTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//BrakeAxle3LeftRecoveryTest/Value"/>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeAxle3RightRecoveryTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//BrakeAxle3RightRecoveryTest/Value"/>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeAxle4LeftRecoveryTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//BrakeAxle4LeftRecoveryTest/Value"/>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeAxle4RightRecoveryTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//BrakeAxle4RightRecoveryTest/Value"/>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Overall Result</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="8">
                                             <fo:block text-align="center" font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeABSTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                              </fo:table-body>
                         </fo:table>
                         <!-- Parking Brake results -->
                         <fo:table border-collapse="collapse" table-layout="fixed" text-align="center" width="100%">
                              <fo:table-column column-width="1.8in" />
                              <fo:table-column column-width="1.2in" />
                              <fo:table-column column-width="0.6in" />
                              <fo:table-column column-width="1.2in" />
                              <fo:table-column column-width="1.1in" />
                              <fo:table-column column-width="0.6in" />
                              <fo:table-column column-width="1.2in" />
                              <fo:table-body>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black" number-rows-spanned="2" display-align="center">
                                             <fo:block font-weight="bold" text-align="center">6. PARKING BRAKE TEST</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Axle 3 Left</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeLeftFrontParkBrakeForceTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="convertForce">
                                                       <xsl:with-param name="FORCE" select="//BrakeLeftFrontParkBrakeForceTest/ParkBrakeForce"/>
                                                       <xsl:with-param name="UNITS" select="//BrakeLeftFrontParkBrakeForceTest/ParkBrakeForce/@Units"/>
                                                  </xsl:call-template>
                                                  KGF

                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Axle 3 Right</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeRightFrontParkBrakeForceTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="convertForce">
                                                       <xsl:with-param name="FORCE" select="//BrakeRightFrontParkBrakeForceTest/ParkBrakeForce"/>
                                                       <xsl:with-param name="UNITS" select="//BrakeRightFrontParkBrakeForceTest/ParkBrakeForce/@Units"/>
                                                  </xsl:call-template>
                                                  KGF
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Overall Result</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="5">
                                             <fo:block text-align="center" font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//BrakeDynamicParkBrakeTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <!-- Speedometer Test -->
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black" number-rows-spanned="4" display-align="center">
                                             <fo:block font-weight="bold" text-align="center">7. SPEEDOMETER TEST</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="3">
                                             <fo:block font-weight="bold">Speed 1</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="3">
                                             <fo:block font-weight="bold">Speed 2</fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Speed</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//SpeedometerSpeedometerTest1/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="convertSpeed">
                                                       <xsl:with-param name="SPEED" select="//SpeedometerSpeedometerTest1/ActualSpeed"/>
                                                       <xsl:with-param name="UNITS" select="//SpeedometerSpeedometerTest1/ActualSpeed/@Units"/>
                                                  </xsl:call-template>
                                                  KPH
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Speed</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//SpeedometerSpeedometerTest2/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="convertSpeed">
                                                       <xsl:with-param name="SPEED" select="//SpeedometerSpeedometerTest2/ActualSpeed"/>
                                                       <xsl:with-param name="UNITS" select="//SpeedometerSpeedometerTest2/ActualSpeed/@Units"/>
                                                  </xsl:call-template>
                                                  KPH
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Accuracy</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//SpeedometerSpeedometerTest1/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//SpeedometerSpeedometerTest1/Accuracy"/>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Accuracy</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//SpeedometerSpeedometerTest2/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//SpeedometerSpeedometerTest2/Accuracy"/>
                                                  %
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Overall Result</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="5">
                                             <fo:block text-align="center" font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//SpeedometerOverallTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <!-- Acceleration Test -->
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black" number-rows-spanned="7" display-align="center">
                                             <fo:block font-weight="bold" text-align="center">8. ACCELERATION TEST</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="3">
                                             <fo:block font-weight="bold">Stage 1</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="3">
                                             <fo:block font-weight="bold">Stage 2</fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Max Speed</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//TransmissionAccelerationTest1/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="convertSpeed">
                                                       <xsl:with-param name="SPEED" select="//TransmissionAccelerationTest1/TopSpeed"/>
                                                       <xsl:with-param name="UNITS" select="//TransmissionAccelerationTest1/TopSpeed/@Units"/>
                                                  </xsl:call-template>
                                                  KPH

                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Max Speed</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//TransmissionAccelerationTest2/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="convertSpeed">
                                                       <xsl:with-param name="SPEED" select="//TransmissionAccelerationTest2/TopSpeed"/>
                                                       <xsl:with-param name="UNITS" select="//TransmissionAccelerationTest2/TopSpeed/@Units"/>
                                                  </xsl:call-template>
                                                  KPH

                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Time</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//TransmissionAccelerationTest1/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//TransmissionAccelerationTest1/ElapsedTime"/>
                                                  sec

                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Time</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//TransmissionAccelerationTest2/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//TransmissionAccelerationTest2/ElapsedTime"/>
                                                  sec
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="3">
                                             <fo:block font-weight="bold">Stage 3</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="3">
                                             <fo:block font-weight="bold">Reverse</fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Max Speed</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//TransmissionAccelerationTest3/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="convertSpeed">
                                                       <xsl:with-param name="SPEED" select="//TransmissionAccelerationTest3/TopSpeed"/>
                                                       <xsl:with-param name="UNITS" select="//TransmissionAccelerationTest3/TopSpeed/@Units"/>
                                                  </xsl:call-template>
                                                  KPH

                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Speed</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//TransmissionAccelerateInReverse/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:call-template name="convertSpeed">
                                                       <xsl:with-param name="SPEED" select="//TransmissionAccelerateInReverse/ReverseSpeed"/>
                                                       <xsl:with-param name="UNITS" select="//TransmissionAccelerateInReverse/ReverseSpeed/@Units"/>
                                                  </xsl:call-template>
                                                  KPH


                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Time</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//TransmissionAccelerationTest3/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//TransmissionAccelerationTest3/ElapsedTime"/>
                                                  sec

                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="3">
                                             <fo:block/>
                                        </fo:table-cell>
                                        
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Overall Result</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="5">
                                             <fo:block text-align="center" font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//SpeedometerOverallTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <!-- Horn Test -->
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black" number-rows-spanned="2" display-align="center">
                                             <fo:block font-weight="bold" text-align="center">9. HORN TEST</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Idle Level</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//SoundLevelRunningHornTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//SoundLevelRunningHornTest/SoundPressureLevel"/>
                                                  db
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Running Level</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//SoundLevelIdleHornTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//SoundLevelIdleHornTest/SoundPressureLevel"/>
                                                  db
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Overall Result</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="5">
                                             <fo:block text-align="center" font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//SoundLevelOverallTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <!-- Opacity Test -->
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black" number-rows-spanned="2" display-align="center">
                                             <fo:block font-weight="bold" text-align="center">10. OPACITY TEST</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Opacity</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//ExhaustOverallTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>
                                                  <xsl:value-of select="//ExhaustExhaustLevelTest/ExhaustSaturationLevel"/>
                                             </fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="3">
                                             <fo:block/>
                                        </fo:table-cell>
                                        
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block>Overall Result</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="5">
                                             <fo:block text-align="center" font-style="italic">
                                                  <xsl:call-template name="showResult">
                                                       <xsl:with-param name="RESULT" select="//ExhaustOverallTest/@Result"/>
                                                  </xsl:call-template>
                                             </fo:block>
                                        </fo:table-cell>
                                   </fo:table-row>
                                   <fo:table-row>
                                        <fo:table-cell border="1pt solid black">
                                             <fo:block font-weight="bold">11. FINAL TEST RESULT</fo:block>
                                        </fo:table-cell>
                                        <fo:table-cell border="1pt solid black" number-columns-spanned="6" display-align="center">
                                             <fo:block text-align="center" font-style="italic" >
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
     <!-- Show brake efficiency -->
     <xsl:template name="showBrakeEfficiency">
          <xsl:param name="UNLADEN_CHASSIS_WEIGHT"/>
          <xsl:variable name="totalBrakeForce">
               <xsl:call-template name="addForces">
                    <xsl:with-param name="FORCE1" select="//BrakeTandemAxleLeftFrontForceTest/BrakeForce"/>
                    <xsl:with-param name="FORCE2" select="//BrakeTandemAxleRightFrontForceTest/BrakeForce"/>
                    <xsl:with-param name="FORCE3" select="//BrakeTandemAxleLeftRearForceTest/BrakeForce"/>
                    <xsl:with-param name="FORCE4" select="//BrakeTandemAxleRightRearForceTest/BrakeForce"/>
                    <xsl:with-param name="FORCE5" select="//BrakeTandemAxleLeftAxle1ForceTest/BrakeForce"/>
                    <xsl:with-param name="FORCE6" select="//BrakeTandemAxleRightAxle1ForceTest/BrakeForce"/>
                    <xsl:with-param name="FORCE7" select="//BrakeTandemAxleLeftAxle2ForceTest/BrakeForce"/>
                    <xsl:with-param name="FORCE8" select="//BrakeTandemAxleRightAxle2ForceTest/BrakeForce"/>
                    <xsl:with-param name="UNITS" select="//BrakeTandemAxleRightRearForceTest/BrakeForce/@Units"/>
               </xsl:call-template>
          </xsl:variable>
          <!-- make sure we have good numbers -->
          <xsl:choose>
               <xsl:when test="(string(number($UNLADEN_CHASSIS_WEIGHT)) != 'NaN') and
                              (string(number($totalBrakeForce)) != 'NaN')">
                    <xsl:value-of select="format-number(($totalBrakeForce * 10) div $UNLADEN_CHASSIS_WEIGHT, '#.##')"/>
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
          <xsl:param name="FORCE5"/>
          <xsl:param name="FORCE6"/>
          <xsl:param name="FORCE7"/>
          <xsl:param name="FORCE8"/>
          <xsl:param name="UNITS"/>
          <xsl:choose>
               <!-- Four axle truck -->
               <xsl:when test="(string(number($FORCE1)) != 'NaN') and 
                               (string(number($FORCE2)) != 'NaN') and
                               (string(number($FORCE3)) != 'NaN') and
                               (string(number($FORCE4)) != 'NaN') and
                               (string(number($FORCE5)) != 'NaN') and 
                               (string(number($FORCE6)) != 'NaN') and
                               (string(number($FORCE7)) != 'NaN') and
                               (string(number($FORCE8)) != 'NaN')">
                    <xsl:choose>
                         <!-- Check if units are lbf -->
                         <xsl:when test="translate($UNITS,'LBF','lbf')='lbf'">
                              <xsl:value-of select="format-number((number($FORCE1) + number($FORCE2) + number($FORCE3) + number($FORCE4) +
                              number($FORCE5) + number($FORCE6) + number($FORCE7) + number($FORCE8))*0.45359237,'#.##')"/>
                         </xsl:when>
                         <xsl:otherwise>
                              <xsl:value-of select="format-number((number($FORCE1) + number($FORCE2) + number($FORCE3) + number($FORCE4) +
                              number($FORCE5) + number($FORCE6) + number($FORCE7) + number($FORCE8)),'#.##')"/>
                         </xsl:otherwise>
                    </xsl:choose>
               </xsl:when>
               <xsl:otherwise>
                    <!-- Three Axle Truck -->
                    <xsl:choose>
                         <xsl:when test="(string(number($FORCE1)) != 'NaN') and 
                               (string(number($FORCE2)) != 'NaN') and
                               (string(number($FORCE3)) != 'NaN') and
                               (string(number($FORCE4)) != 'NaN') and
                               (string(number($FORCE5)) != 'NaN') and 
                               (string(number($FORCE6)) != 'NaN')">
                              <xsl:choose>
                                   <!-- Check if units are lbf -->
                                   <xsl:when test="translate($UNITS,'LBF','lbf')='lbf'">
                                        <xsl:value-of select="format-number((number($FORCE1) + number($FORCE2) + number($FORCE3) + number($FORCE4) +
                                             number($FORCE5) + number($FORCE6))*0.45359237,'#.##')"/>
                                   </xsl:when>
                                   <xsl:otherwise>
                                        <xsl:value-of select="format-number((number($FORCE1) + number($FORCE2) + number($FORCE3) + number($FORCE4) +
                                             number($FORCE5) + number($FORCE6)),'#.##')"/>
                                   </xsl:otherwise>
                              </xsl:choose>
                         </xsl:when>
                         <xsl:otherwise>
                              <!-- Two Axle Test -->
                              <xsl:choose>
                                   <xsl:when test="(string(number($FORCE1)) != 'NaN') and 
                                        (string(number($FORCE2)) != 'NaN') and
                                        (string(number($FORCE3)) != 'NaN') and
                                        (string(number($FORCE4)) != 'NaN')">
                                        <xsl:choose>
                                             <!-- Check if units are lbf -->
                                             <xsl:when test="translate($UNITS,'LBF','lbf')='lbf'">
                                                  <xsl:value-of select="format-number((number($FORCE1) + number($FORCE2) + number($FORCE3) + number($FORCE4))*0.45359237,'#.##')"/>
                                             </xsl:when>
                                             <xsl:otherwise>
                                                  <xsl:value-of select="format-number((number($FORCE1) + number($FORCE2) + number($FORCE3) + number($FORCE4)),'#.##')"/>
                                             </xsl:otherwise>
                                        </xsl:choose>
                                   </xsl:when>
                                   <xsl:otherwise>
                                        <!-- No data -->
                                        <xsl:text>----</xsl:text>
                                   </xsl:otherwise>
                              </xsl:choose>
                         </xsl:otherwise>
                    </xsl:choose>
               </xsl:otherwise>
          </xsl:choose>
     </xsl:template>
     <!-- Finds Block Difference for given wheels -->
     <xsl:template name="showBlockDifference">
          <xsl:param name="WHEEL_NAME1" />
          <xsl:param name="WHEEL_NAME2" />
          <xsl:variable name="brakeForce1">
               <xsl:value-of select="$WHEEL_NAME1" />
          </xsl:variable>
          <xsl:variable name="brakeForce2">
               <xsl:value-of select="$WHEEL_NAME2" />
          </xsl:variable>
          <xsl:choose>
               <xsl:when test="(string(number($brakeForce1)) != 'NaN') and (string(number($brakeForce2)) != 'NaN')">
                    <xsl:choose>
                         <xsl:when test="$brakeForce1 > $brakeForce2">
                              <xsl:variable name="maxForce" select="$brakeForce1" />
                              <xsl:variable name="minForce" select="$brakeForce2" />
                              <xsl:value-of select="format-number( (($maxForce - $minForce) * 100) div $maxForce, '#.##')" />
                         </xsl:when>
                         <xsl:otherwise>
                              <xsl:variable name="maxForce" select="$brakeForce2" />
                              <xsl:variable name="minForce" select="$brakeForce1" />
                              <xsl:value-of select="format-number( (($maxForce - $minForce) * 100) div $maxForce, '#.##')" />
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
     
</xsl:stylesheet>

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
               <u>Bolero Roller and Brake Testing Report</u>
          </h3>
          <table border="1" rules="all" cellpadding="1">
              <tr>
                   <td align="left">VIN:</td>
                   <td align="left">
                        <xsl:value-of select="//VIN"/>
                   </td>
                   <td align="left">Date:</td>
                   <td>
                        <xsl:value-of select="//TestResult/MachineInitialize/Date"/>
                   </td>
              </tr>
              <tr>
                  <td align="left"/>
                  <td align="left"/>
                   <td align="left">Time:</td>
                   <td>
                        <xsl:value-of select="//TestResult/MachineInitialize/Time"/>
                   </td>
              </tr>
          </table>
     </xsl:template>

     <!-- Test Results -->
     <xsl:template name="TestResults" match="/BepVehicleTestResult/TestResult">

          <h3/>
          <table border="1" rules="all" title="Test Results" cellpadding="1">
              <tr>
                  <td align="left">Gear 1:</td>
                  <td align="left">
                      <xsl:call-template name="showResult">
                           <xsl:with-param name="RESULT" select="//TransmissionSpeedBasedGearTest_1/@Result"/>
                      </xsl:call-template>
                  </td>
                  <td align="right">LHFR:</td>
                  <td align="center">
                       <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//BrakeLeftFrontForceTest/@Result"/>
                       </xsl:call-template>
                  </td>
                  <td align="right">4WDH:</td>
                  <td align="center">
                       <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//TransferCaseCheck4HIAxleBalance/@Result"/>
                       </xsl:call-template>
                  </td>
              </tr>

              <tr>
                  <td align="left">Gear 2:</td>
                  <td align="left">
                      <xsl:call-template name="showResult">
                           <xsl:with-param name="RESULT" select="//TransmissionSpeedBasedGearTest_2/@Result"/>
                      </xsl:call-template>
                  </td>
                  <td align="right">LHRR:</td>
                  <td align="center">
                       <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//BrakeLeftRearForceTest/@Result"/>
                       </xsl:call-template>
                  </td>
                  <td align="right">4WDL:</td>
                  <td align="center">
                       <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//TransferCaseCheckAxleRatio/@Result"/>
                       </xsl:call-template>
                  </td>
              </tr>

              <tr>
                  <td align="left">Gear 3:</td>
                  <td align="left">
                      <xsl:call-template name="showResult">
                           <xsl:with-param name="RESULT" select="//TransmissionSpeedBasedGearTest_3/@Result"/>
                      </xsl:call-template>
                  </td>
                  <td align="right">RHFR:</td>
                  <td align="center">
                       <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//BrakeRightFrontForceTest/@Result"/>
                       </xsl:call-template>
                  </td>
                  <td align="right">RWAL:</td>
                  <td align="center">
                       <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//BrakeRwalSensorTest/@Result"/>
                       </xsl:call-template>
                  </td>
              </tr>

              <tr>
                  <td align="left">Gear 4:</td>
                  <td align="left">
                      <xsl:call-template name="showResult">
                           <xsl:with-param name="RESULT" select="//TransmissionSpeedBasedGearTest_4/@Result"/>
                      </xsl:call-template>
                  </td>
                  <td align="right">RHRR:</td>
                  <td align="center">
                       <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//BrakeRightRearForceTest/@Result"/>
                       </xsl:call-template>
                  </td>
                  <td align="right">Axle Ratio:</td>
                  <td align="center">
                       <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//TransferCaseCheckAxleRatio/@Result"/>
                       </xsl:call-template>
                  </td>
              </tr>

              <tr>
                  <td align="left">Gear 5:</td>
                  <td align="left">
                      <xsl:call-template name="showResult">
                           <xsl:with-param name="RESULT" select="//TransmissionSpeedBasedGearTest_5/@Result"/>
                      </xsl:call-template>
                  </td>
                  <td align="right">FR Balance:</td>
                  <td align="center">
                       <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//BrakeFrontBalanceTest/@Result"/>
                       </xsl:call-template>
                  </td>
                  <td align="right">Park Brake:</td>
                  <td align="center">
                       <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//BrakeAnalyzeDynamicParkBrake/@Result"/>
                       </xsl:call-template>
                  </td>
              </tr>

              <tr>
                  <td align="left"/>
                  <td align="left"/>
                  <td align="right">RR Balance:</td>
                  <td align="center">
                       <xsl:call-template name="showResult">
                            <xsl:with-param name="RESULT" select="//BrakeRearBalanceTest/@Result"/>
                       </xsl:call-template>
                  </td>
                  <td align="right"/>
                  <td align="center"/>
              </tr>

              <tr>
                  <td align="left">Test Result:</td>
                  <td align="left">
                      <xsl:call-template name="showResult">
                           <xsl:with-param name="RESULT" select="//OverallTest/@Result"/>
                      </xsl:call-template>
                  </td>
                  <td align="right"/>
                  <td align="center"/>
                  <td align="right"/>
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

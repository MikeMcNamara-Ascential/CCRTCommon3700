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
                    <td align="left" width="7">Date:</td>
                    <td align="left" width="20">
                         <xsl:value-of select="//TestResult/MachineInitialize/Date"/>
                    </td>
                    <td align="left" width="7">Time:</td>
                    <td align="left">
                         <xsl:value-of select="//TestResult/MachineInitialize/Time"/>
                    </td>
               </tr>
               <tr>
                    <td align="left" width="7">VIN:</td>
                    <td align="left">
                         <u>
                              <xsl:value-of select="//VIN"/>
                         </u>
                    </td>
               </tr>
               <tr>
                    <td align="left" width="7">VC:</td>
                    <td align="left">
                         <u>
                              <xsl:value-of select="//VC"/>
                         </u>
                    </td>
               </tr>
          </table>
     </xsl:template>
     <!-- Test Results -->
     <xsl:template name="TestResults" match="/BepVehicleTestResult/TestResult">
          <h3/>
          <table border="1" rules="all" title="Test Results" cellpadding="1">
               <!-- Front Axle Brake Test Results -->
               <tr bgcolor="white">
                    <th align="left" width="15">Front Axle</th>
                    <td align="left" width="8">Min</td>
                    <td align="left" width="8">Max</td>
                    <td align="left" width="8">Actual</td>
                    <td align="left" width="8">Units</td>
                    <td align="left" width="15"></td>
               </tr>
               <tr>
                    <td align="left" width="15">LF Drag</td>
                    <td width="8">0</td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertForce">
                              <xsl:with-param name="FORCE" select="//BrakeFrontAxleLeftFrontDragTest/MaxDragForce"/>
                              <xsl:with-param name="UNITS" select="//BrakeFrontAxleLeftFrontDragTest/MaxDragForce/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertForce">
                              <xsl:with-param name="FORCE" select="//BrakeFrontAxleLeftFrontDragTest/DragForce"/>
                              <xsl:with-param name="UNITS" select="//BrakeFrontAxleLeftFrontDragTest/DragForce/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">KGF</td>
                    <td align="left" width="15">
                         <xsl:value-of select="//BrakeFrontAxleLeftFrontDragTest/@Result"/>
                    </td>
               </tr>
               <tr>
                    <td align="left" width="15">RF Drag</td>
                    <td width="8">0</td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertForce">
                              <xsl:with-param name="FORCE" select="//BrakeFrontAxleRightFrontDragTest/MaxDragForce"/>
                              <xsl:with-param name="UNITS" select="//BrakeFrontAxleRightFrontDragTest/MaxDragForce/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertForce">
                              <xsl:with-param name="FORCE" select="//BrakeFrontAxleRightFrontDragTest/DragForce"/>
                              <xsl:with-param name="UNITS" select="//BrakeFrontAxleRightFrontDragTest/DragForce/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">KGF</td>
                    <td align="left" width="15">
                         <xsl:value-of select="//BrakeFrontAxleRightFrontDragTest/@Result"/>
                    </td>
               </tr>
               <!-- If this vehicle has two front axles -->
               <xsl:if test="//BrakeFrontAxleLeftRearDragTest/DragForce != 0">
                    <tr>
                         <td align="left" width="15">LR Drag</td>
                         <td width="8">0</td>
                         <td align="left" width="8">
                              <xsl:call-template name="convertForce">
                                   <xsl:with-param name="FORCE" select="//BrakeFrontAxleLeftRearDragTest/MaxDragForce"/>
                                   <xsl:with-param name="UNITS" select="//BrakeFrontAxleLeftRearDragTest/MaxDragForce/@Units"/>
                              </xsl:call-template>
                         </td>
                         <td align="left" width="8">
                              <xsl:call-template name="convertForce">
                                   <xsl:with-param name="FORCE" select="//BrakeFrontAxleLeftRearDragTest/DragForce"/>
                                   <xsl:with-param name="UNITS" select="//BrakeFrontAxleLeftRearDragTest/DragForce/@Units"/>
                              </xsl:call-template>
                         </td>
                         <td align="left" width="8">KGF</td>
                         <td align="left" width="15">
                              <xsl:value-of select="//BrakeFrontAxleLeftRearDragTest/@Result"/>
                         </td>
                    </tr>
                    <tr>
                         <td align="left" width="15">RR Drag</td>
                         <td width="8">0</td>
                         <td align="left" width="8">
                              <xsl:call-template name="convertForce">
                                   <xsl:with-param name="FORCE" select="//BrakeFrontAxleRightRearDragTest/MaxDragForce"/>
                                   <xsl:with-param name="UNITS" select="//BrakeFrontAxleRightRearDragTest/MaxDragForce/@Units"/>
                              </xsl:call-template>
                         </td>
                         <td align="left" width="8">
                              <xsl:call-template name="convertForce">
                                   <xsl:with-param name="FORCE" select="//BrakeFrontAxleRightRearDragTest/DragForce"/>
                                   <xsl:with-param name="UNITS" select="//BrakeFrontAxleRightRearDragTest/DragForce/@Units"/>
                              </xsl:call-template>
                         </td>
                         <td align="left" width="8">KGF</td>
                         <td align="left" width="15">
                              <xsl:value-of select="//BrakeFrontAxleRightRearDragTest/@Result"/>
                         </td>
                    </tr>
               </xsl:if>
               <tr>
                    <td align="left" width="15">LF Brake</td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertForce">
                              <xsl:with-param name="FORCE" select="//BrakeFrontAxleLeftFrontForceTest/MinBrakeForce"/>
                              <xsl:with-param name="UNITS" select="//BrakeFrontAxleLeftFrontForceTest/MinBrakeForce/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertForce">
                              <xsl:with-param name="FORCE" select="//BrakeFrontAxleLeftFrontForceTest/MaxBrakeForce"/>
                              <xsl:with-param name="UNITS" select="//BrakeFrontAxleLeftFrontForceTest/MaxBrakeForce/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertForce">
                              <xsl:with-param name="FORCE" select="//BrakeFrontAxleLeftFrontForceTest/BrakeForce"/>
                              <xsl:with-param name="UNITS" select="//BrakeFrontAxleLeftFrontForceTest/BrakeForce/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">KGF</td>
                    <td align="left">
                         <xsl:value-of select="//BrakeFrontAxleLeftFrontForceTest/@Result"/>
                    </td>
               </tr>
               <tr>
                    <td align="left" width="15">RF Brake</td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertForce">
                              <xsl:with-param name="FORCE" select="//BrakeFrontAxleRightFrontForceTest/MinBrakeForce"/>
                              <xsl:with-param name="UNITS" select="//BrakeFrontAxleRightFrontForceTest/MinBrakeForce/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertForce">
                              <xsl:with-param name="FORCE" select="//BrakeFrontAxleRightFrontForceTest/MaxBrakeForce"/>
                              <xsl:with-param name="UNITS" select="//BrakeFrontAxleRightFrontForceTest/MaxBrakeForce/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertForce">
                              <xsl:with-param name="FORCE" select="//BrakeFrontAxleRightFrontForceTest/BrakeForce"/>
                              <xsl:with-param name="UNITS" select="//BrakeFrontAxleRightFrontForceTest/BrakeForce/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">KGF</td>
                    <td align="left" width="15">
                         <xsl:value-of select="//BrakeFrontAxleRightFrontForceTest/@Result"/>
                    </td>
               </tr>
      
               <!-- If this vehicle has two front axles -->
               <xsl:if test="//BrakeFrontAxleLeftRearDragTest/DragForce != 0">
                    <tr>
                         <td align="left" width="15">LR Brake</td>
                         <td align="left" width="8">
                              <xsl:call-template name="convertForce">
                                   <xsl:with-param name="FORCE" select="//BrakeFrontAxleLeftRearForceTest/MinBrakeForce"/>
                                   <xsl:with-param name="UNITS" select="//BrakeFrontAxleLeftRearForceTest/MinBrakeForce/@Units"/>
                              </xsl:call-template>
                         </td>
                         <td align="left" width="8">
                              <xsl:call-template name="convertForce">
                                   <xsl:with-param name="FORCE" select="//BrakeFrontAxleLeftRearForceTest/MaxBrakeForce"/>
                                   <xsl:with-param name="UNITS" select="//BrakeFrontAxleLeftRearForceTest/MaxBrakeForce/@Units"/>
                              </xsl:call-template>
                         </td>
                         <td align="left" width="8">
                              <xsl:call-template name="convertForce">
                                   <xsl:with-param name="FORCE" select="//BrakeFrontAxleLeftRearForceTest/BrakeForce"/>
                                   <xsl:with-param name="UNITS" select="//BrakeFrontAxleLeftRearForceTest/BrakeForce/@Units"/>
                              </xsl:call-template>
                         </td>
                         <td align="left" width="8">KGF</td>
                         <td align="left" width="15">
                              <xsl:value-of select="//BrakeFrontAxleLeftRearForceTest/@Result"/>
                         </td>
                    </tr>
                    <tr>
                         <td align="left" width="15">RR Brake</td>
                         <td align="left" width="8">
                              <xsl:call-template name="convertForce">
                                   <xsl:with-param name="FORCE" select="//BrakeFrontAxleRightRearForceTest/MinBrakeForce"/>
                                   <xsl:with-param name="UNITS" select="//BrakeFrontAxleRightRearForceTest/MinBrakeForce/@Units"/>
                              </xsl:call-template>
                         </td>
                         <td align="left" width="8">
                              <xsl:call-template name="convertForce">
                                   <xsl:with-param name="FORCE" select="//BrakeFrontAxleRightRearForceTest/MaxBrakeForce"/>
                                   <xsl:with-param name="UNITS" select="//BrakeFrontAxleRightRearForceTest/MaxBrakeForce/@Units"/>
                              </xsl:call-template>
                         </td>
                         <td align="left" width="8">
                              <xsl:call-template name="convertForce">
                                   <xsl:with-param name="FORCE" select="//BrakeFrontAxleRightRearForceTest/BrakeForce"/>
                                   <xsl:with-param name="UNITS" select="//BrakeFrontAxleRightRearForceTest/BrakeForce/@Units"/>
                              </xsl:call-template>
                         </td>
                         <td align="left" width="8">KGF</td>
                         <td align="left" width="15">
                              <xsl:value-of select="//BrakeFrontAxleRightRearForceTest/@Result"/>
                         </td>
                    </tr>
               </xsl:if>
               
               <tr>
                    <td align="left" width="15">Front Bal</td>
                    <td align="left" width="8">
                         <xsl:value-of select="//BrakeFrontAxleFrontBalanceTest/MinPercentBalance"/>
                    </td>
                    <td align="left" width="8">
                         <xsl:value-of select="//BrakeFrontAxleFrontBalanceTest/MaxPercentBalance"/>
                    </td>
                    <td align="left" width="8">
                         <xsl:value-of select="//BrakeFrontAxleFrontBalanceTest/Value"/>
                    </td>
                    <td align="left" width="8">%</td>
                    <td align="left">
                         <xsl:value-of select="//BrakeFrontAxleFrontBalanceTest/@Result"/>
                    </td>
               </tr>
               <xsl:if test="//BrakeFrontAxleLeftRearDragTest/DragForce != 0">
                    <tr>
                         <td align="left" width="15">Rear Bal</td>
                         <td align="left" width="8">
                              <xsl:value-of select="//BrakeRearBalanceTest/MinPercentBalance"/>
                         </td>
                         <td align="left" width="8">
                              <xsl:value-of select="//BrakeRearBalanceTest/MaxPercentBalance"/>
                         </td>
                         <td align="left" width="8">
                              <xsl:value-of select="//BrakeRearBalanceTest/Value"/>
                         </td>
                         <td align="left" width="8">%</td>
                         <td align="left">
                              <xsl:value-of select="//BrakeRearBalanceTest/@Result"/>
                         </td>
                    </tr>
               </xsl:if>
               
               <!-- Front ABS Test Results -->
               <xsl:if test="//BrakeLeftFrontABSReduction">
                    <xsl:call-template name="sectionSpacer"/>
                    <tr>
                         <th align="left" width="31">LF ABS</th>
                         <td align="left">
                              <xsl:choose>
                                   <xsl:when test="//BrakeLfSensorCross/@Result='Fail' or //BrakeLfSensorSpeed/@Result='Fail' or 
                                 //BrakeLeftFrontABSReduction/@Result='Fail' or //BrakeLeftFrontABSRecovery/@Result='Fail' or 
                                 //BrakeLeftFrontABSInitialConditions/@Result='Fail'">
                                        <xsl:text>Fail</xsl:text>
                                   </xsl:when>
                                   <xsl:otherwise>
                                        <xsl:text>Pass</xsl:text>
                                   </xsl:otherwise>
                              </xsl:choose>
                         </td>
                    </tr>
                    <tr>
                         <th align="left" width="31">RF ABS</th>
                         <td align="left">
                              <xsl:choose>
                                   <xsl:when test="//BrakeRfSensorCross/@Result='Fail' or //BrakeRfSensorSpeed/@Result='Fail' or 
                                 //BrakeRightFrontABSReduction/@Result='Fail' or //BrakeRightFrontABSRecovery/@Result='Fail' or 
                                 //BrakeRightFrontABSInitialConditions/@Result='Fail'">
                                        <xsl:text>Fail</xsl:text>
                                   </xsl:when>
                                   <xsl:otherwise>
                                        <xsl:text>Pass</xsl:text>
                                   </xsl:otherwise>
                              </xsl:choose>
                         </td>
                    </tr>
               </xsl:if>
               <!-- Tandem Axle Brake Test Results -->
               <xsl:call-template name="sectionSpacer"/>
               <tr bgcolor="white">
                    <th align="left" width="15">Rear Axle</th>
                    <td align="left" width="8">Min</td>
                    <td align="left" width="8">Max</td>
                    <td align="left" width="8">Actual</td>
                    <td align="left" width="8">Units</td>
                    <td align="left" width="15"></td>
               </tr>
               <tr>
                    <td align="left" width="15">LF Drag</td>
                    <td width="8">0</td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertForce">
                              <xsl:with-param name="FORCE" select="//BrakeTandemAxleLeftFrontDragTest/MaxDragForce"/>
                              <xsl:with-param name="UNITS" select="//BrakeTandemAxleLeftFrontDragTest/MaxDragForce/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertForce">
                              <xsl:with-param name="FORCE" select="//BrakeTandemAxleLeftFrontDragTest/DragForce"/>
                              <xsl:with-param name="UNITS" select="//BrakeTandemAxleLeftFrontDragTest/DragForce/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">KGF</td>
                    <td align="left" width="15">
                         <xsl:value-of select="//BrakeTandemAxleLeftFrontDragTest/@Result"/>
                    </td>
               </tr>
               <tr>
                    <td align="left" width="15">RF Drag</td>
                    <td width="8">0</td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertForce">
                              <xsl:with-param name="FORCE" select="//BrakeTandemAxleRightFrontDragTest/MaxDragForce"/>
                              <xsl:with-param name="UNITS" select="//BrakeTandemAxleRightFrontDragTest/MaxDragForce/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertForce">
                              <xsl:with-param name="FORCE" select="//BrakeTandemAxleRightFrontDragTest/DragForce"/>
                              <xsl:with-param name="UNITS" select="//BrakeTandemAxleRightFrontDragTest/DragForce/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">KGF</td>
                    <td align="left" width="15">
                         <xsl:value-of select="//BrakeTandemAxleRightFrontDragTest/@Result"/>
                    </td>
               </tr>
               <!-- If this vehicle has two rear axles -->
               <xsl:if test="//BrakeTandemAxleLeftRearForceTest">
                    <tr>
                         <td align="left" width="15">LR Drag</td>
                         <td width="8">0</td>
                         <td align="left" width="8">
                              <xsl:call-template name="convertForce">
                                   <xsl:with-param name="FORCE" select="//BrakeTandemAxleLeftRearDragTest/MaxDragForce"/>
                                   <xsl:with-param name="UNITS" select="//BrakeTandemAxleLeftRearDragTest/MaxDragForce/@Units"/>
                              </xsl:call-template>
                         </td>
                         <td align="left" width="8">
                              <xsl:call-template name="convertForce">
                                   <xsl:with-param name="FORCE" select="//BrakeTandemAxleLeftRearDragTest/DragForce"/>
                                   <xsl:with-param name="UNITS" select="//BrakeTandemAxleLeftRearDragTest/DragForce/@Units"/>
                              </xsl:call-template>
                         </td>
                         <td align="left" width="8">KGF</td>
                         <td align="left" width="15">
                              <xsl:value-of select="//BrakeTandemAxleLeftRearDragTest/@Result"/>
                         </td>
                    </tr>
                    <tr>
                         <td align="left" width="15">RR Drag</td>
                         <td width="8">0</td>
                         <td align="left" width="8">
                              <xsl:call-template name="convertForce">
                                   <xsl:with-param name="FORCE" select="//BrakeTandemAxleRightRearDragTest/MaxDragForce"/>
                                   <xsl:with-param name="UNITS" select="//BrakeTandemAxleRightRearDragTest/MaxDragForce/@Units"/>
                              </xsl:call-template>
                         </td>
                         <td align="left" width="8">
                              <xsl:call-template name="convertForce">
                                   <xsl:with-param name="FORCE" select="//BrakeTandemAxleRightRearDragTest/DragForce"/>
                                   <xsl:with-param name="UNITS" select="//BrakeTandemAxleRightRearDragTest/DragForce/@Units"/>
                              </xsl:call-template>
                         </td>
                         <td align="left" width="8">KGF</td>
                         <td align="left" width="15">
                              <xsl:value-of select="//BrakeTandemAxleRightRearDragTest/@Result"/>
                         </td>
                    </tr>
               </xsl:if>
               <tr>
                    <td align="left" width="15">LF Brake</td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertForce">
                              <xsl:with-param name="FORCE" select="//BrakeTandemAxleLeftFrontForceTest/MinBrakeForce"/>
                              <xsl:with-param name="UNITS" select="//BrakeTandemAxleLeftFrontForceTest/MinBrakeForce/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertForce">
                              <xsl:with-param name="FORCE" select="//BrakeTandemAxleLeftFrontForceTest/MaxBrakeForce"/>
                              <xsl:with-param name="UNITS" select="//BrakeTandemAxleLeftFrontForceTest/MaxBrakeForce/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertForce">
                              <xsl:with-param name="FORCE" select="//BrakeTandemAxleLeftFrontForceTest/BrakeForce"/>
                              <xsl:with-param name="UNITS" select="//BrakeTandemAxleLeftFrontForceTest/BrakeForce/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">KGF</td>
                    <td align="left" width="15">
                         <xsl:value-of select="//BrakeTandemAxleLeftFrontForceTest/@Result"/>
                    </td>
               </tr>
               <tr>
                    <td align="left" width="15">RF Brake</td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertForce">
                              <xsl:with-param name="FORCE" select="//BrakeTandemAxleRightFrontForceTest/MinBrakeForce"/>
                              <xsl:with-param name="UNITS" select="//BrakeTandemAxleRightFrontForceTest/MinBrakeForce/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertForce">
                              <xsl:with-param name="FORCE" select="//BrakeTandemAxleRightFrontForceTest/MaxBrakeForce"/>
                              <xsl:with-param name="UNITS" select="//BrakeTandemAxleRightFrontForceTest/MaxBrakeForce/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertForce">
                              <xsl:with-param name="FORCE" select="//BrakeTandemAxleRightFrontForceTest/BrakeForce"/>
                              <xsl:with-param name="UNITS" select="//BrakeTandemAxleRightFrontForceTest/BrakeForce/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">KGF</td>
                    <td align="left" width="15">
                         <xsl:value-of select="//BrakeTandemAxleRightFrontForceTest/@Result"/>
                    </td>
               </tr>
               <!-- If this vehicle has two rear axles -->
               <xsl:if test="//BrakeTandemAxleLeftRearForceTest">
                    <tr>
                         <td align="left" width="15">LR Brake</td>
                         <td align="left" width="8">
                              <xsl:call-template name="convertForce">
                                   <xsl:with-param name="FORCE" select="//BrakeTandemAxleLeftRearForceTest/MinBrakeForce"/>
                                   <xsl:with-param name="UNITS" select="//BrakeTandemAxleLeftRearForceTest/MinBrakeForce/@Units"/>
                              </xsl:call-template>
                         </td>
                         <td align="left" width="8">
                              <xsl:call-template name="convertForce">
                                   <xsl:with-param name="FORCE" select="//BrakeTandemAxleLeftRearForceTest/MaxBrakeForce"/>
                                   <xsl:with-param name="UNITS" select="//BrakeTandemAxleLeftRearForceTest/MaxBrakeForce/@Units"/>
                              </xsl:call-template>
                         </td>
                         <td align="left" width="8">
                              <xsl:call-template name="convertForce">
                                   <xsl:with-param name="FORCE" select="//BrakeTandemAxleLeftRearForceTest/BrakeForce"/>
                                   <xsl:with-param name="UNITS" select="//BrakeTandemAxleLeftRearForceTest/BrakeForce/@Units"/>
                              </xsl:call-template>
                         </td>
                         <td align="left" width="8">KGF</td>
                         <td align="left" width="15">
                              <xsl:value-of select="//BrakeTandemAxleLeftRearForceTest/@Result"/>
                         </td>
                    </tr>
                    <tr>
                         <td align="left" width="15">RR Brake</td>
                         <td align="left" width="8">
                              <xsl:call-template name="convertForce">
                                   <xsl:with-param name="FORCE" select="//BrakeTandemAxleRightRearForceTest/MinBrakeForce"/>
                                   <xsl:with-param name="UNITS" select="//BrakeTandemAxleRightRearForceTest/MinBrakeForce/@Units"/>
                              </xsl:call-template>
                         </td>
                         <td align="left" width="8">
                              <xsl:call-template name="convertForce">
                                   <xsl:with-param name="FORCE" select="//BrakeTandemAxleRightRearForceTest/MaxBrakeForce"/>
                                   <xsl:with-param name="UNITS" select="//BrakeTandemAxleRightRearForceTest/MaxBrakeForce/@Units"/>
                              </xsl:call-template>
                         </td>
                         <td align="left" width="8">
                              <xsl:call-template name="convertForce">
                                   <xsl:with-param name="FORCE" select="//BrakeTandemAxleRightRearForceTest/BrakeForce"/>
                                   <xsl:with-param name="UNITS" select="//BrakeTandemAxleRightRearForceTest/BrakeForce/@Units"/>
                              </xsl:call-template>
                         </td>
                         <td align="left" width="8">KGF</td>
                         <td align="left" width="15">
                              <xsl:value-of select="//BrakeTandemAxleRightRearForceTest/@Result"/>
                         </td>
                    </tr>
               </xsl:if>
               <tr>
                    <td align="left" width="15">Rear Bal</td>
                    <td align="left" width="8">
                         <xsl:value-of select="//BrakeTandemAxleFrontBalanceTest/MinPercentBalance"/>
                    </td>
                    <td align="left" width="8">
                         <xsl:value-of select="//BrakeTandemAxleFrontBalanceTest/MaxPercentBalance"/>
                    </td>
                    <td align="left" width="8">
                         <xsl:value-of select="//BrakeTandemAxleFrontBalanceTest/Value"/>
                    </td>
                    <td align="left" width="8">%</td>
                    <td align="left" width="15">
                         <xsl:value-of select="//BrakeTandemAxleFrontBalanceTest/@Result"/>
                    </td>
               </tr>
               <!-- If this vehicle has two rear axles -->
               









               <xsl:if test="//BrakeTandemAxleLeftRearForceTest">
                    <tr>
                         <td align="left" width="15">Rear Bal</td>
                         <td align="left" width="8">
                              <xsl:value-of select="//BrakeRearBalanceTest/MinPercentBalance"/>
                         </td>
                         <td align="left" width="8">
                              <xsl:value-of select="//BrakeRearBalanceTest/MaxPercentBalance"/>
                         </td>
                         <td align="left" width="8">
                              <xsl:value-of select="//BrakeRearBalanceTest/Value"/>
                         </td>
                         <td align="left" width="8">%</td>
                         <td align="left" width="15">
                              <xsl:value-of select="//BrakeRearBalanceTest/@Result"/>
                         </td>
                    </tr>
                    <!--
                    <tr>
                         <td align="left" width="15">F/R Bal</td>
                         <td align="left" width="8">
                              <xsl:value-of select="//BrakeFrontToRearBalanceTest/MinPercentBalance"/>
                         </td>
                         <td align="left" width="8">
                              <xsl:value-of select="//BrakeFrontToRearBalanceTest/MaxPercentBalance"/>
                         </td>
                         <td align="left" width="8">
                              <xsl:value-of select="//BrakeFrontToRearBalanceTest/Value"/>
                         </td>
                         <td align="left" width="8">%</td>
                         <td align="left">
                              <xsl:value-of select="//BrakeFrontToRearBalanceTest/@Result"/>
                         </td>
                    </tr>
                    -->
               </xsl:if>
               
               <!-- Rear ABS Test Results -->
               <xsl:if test="//BrakeLeftTandemABSReduction">
                    <xsl:call-template name="sectionSpacer"/>
                    <tr>
                         <th align="left" width="47">LR ABS</th>
                         <td align="left">
                              <xsl:choose>
                                   <xsl:when test="//BrakeLtSensorCross/@Result='Fail' or //BrakeLtSensorSpeed/@Result='Fail' or
                                 //BrakeLeftTandemABSReduction/@Result='Fail' or //BrakeLeftTandemABSRecovery/@Result='Fail' or
                                 //BrakeLeftTandemABSInitialConditions/@Result='Fail'">
                                        <xsl:text>Fail</xsl:text>
                                   </xsl:when>
                                   <xsl:otherwise>
                                        <xsl:text>Pass</xsl:text>
                                   </xsl:otherwise>
                              </xsl:choose>
                         </td>
                    </tr>
                    <tr>
                         <th align="left" width="47">RR ABS</th>
                         <td align="left">
                              <xsl:choose>
                                   <xsl:when test="//BrakeRtSensorSpeed/@Result='Fail' or //BrakeRightTandemABSReduction/@Result='Fail' or 
                                 //BrakeRightTandemABSRecovery/@Result='Fail' or //BrakeRightTandemABSInitialConditions/@Result='Fail'">
                                        <xsl:text>Fail</xsl:text>
                                   </xsl:when>
                                   <xsl:otherwise>
                                        <xsl:text>Pass</xsl:text>
                                   </xsl:otherwise>
                              </xsl:choose>
                         </td>
                    </tr>
               </xsl:if>

               <xsl:call-template name="sectionSpacer"/>
               <tr>
                    <td align="left" width="15">Brake Pedal Force</td>
                    <td align="left" width="8" />
                    <td align="left" width="8" />
                    <td align="left" width="8">
                         <xsl:call-template name="convertForce">
                              <xsl:with-param name="FORCE" select="//BrakePedalForce"/>
                              <xsl:with-param name="UNITS" select="//BrakePedalForce/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">KGF</td>
                    <td align="left" width="15" />
               </tr>

               <!-- Parking Brake Test Results -->
               <xsl:call-template name="sectionSpacer"/>
               <tr bgcolor="white">
                    <th align="left" width="15">Park Brake</th>
                    <td align="left" width="8">Min</td>
                    <td align="left" width="8">Max</td>
                    <td align="left" width="8">Actual</td>
                    <td align="left" width="8">Units</td>
                    <td align="left">
                         <xsl:value-of select="//BrakeAnalyzeDynamicParkBrake/@Result"/>
                    </td>
               </tr>
               <tr>
                    <td align="left" width="15">LF</td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertForce">
                              <xsl:with-param name="FORCE" select="//BrakeLeftFrontParkBrakeForceTest/MinParkBrakeForce"/>
                              <xsl:with-param name="UNITS" select="//BrakeLeftFrontParkBrakeForceTest/MinParkBrakeForce/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertForce">
                              <xsl:with-param name="FORCE" select="//BrakeLeftFrontParkBrakeForceTest/MaxParkBrakeForce"/>
                              <xsl:with-param name="UNITS" select="//BrakeLeftFrontParkBrakeForceTest/MaxParkBrakeForce/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertForce">
                              <xsl:with-param name="FORCE" select="//BrakeLeftFrontParkBrakeForceTest/ParkBrakeForce"/>
                              <xsl:with-param name="UNITS" select="//BrakeLeftFrontParkBrakeForceTest/ParkBrakeForce/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">KGF</td>
                    <td align="left"></td>
               </tr>
               <tr>
                    <td align="left" width="15">RF</td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertForce">
                              <xsl:with-param name="FORCE" select="//BrakeRightFrontParkBrakeForceTest/MinParkBrakeForce"/>
                              <xsl:with-param name="UNITS" select="//BrakeRightFrontParkBrakeForceTest/MinParkBrakeForce/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertForce">
                              <xsl:with-param name="FORCE" select="//BrakeRightFrontParkBrakeForceTest/MaxParkBrakeForce"/>
                              <xsl:with-param name="UNITS" select="//BrakeRightFrontParkBrakeForceTest/MaxParkBrakeForce/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertForce">
                              <xsl:with-param name="FORCE" select="//BrakeRightFrontParkBrakeForceTest/ParkBrakeForce"/>
                              <xsl:with-param name="UNITS" select="//BrakeRightFrontParkBrakeForceTest/ParkBrakeForce/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">KGF</td>
                    <td align="left"></td>
               </tr>
               <!-- Speedo test -->
               <xsl:call-template name="sectionSpacer"/>
               <tr bgcolor="white">
                    <th align="left" width="15">Speedometer</th>
                    <td align="left" width="8">Min</td>
                    <td align="left" width="8">Max</td>
                    <td align="left" width="8">Actual</td>
                    <td align="left" width="8">Units</td>
                    <td align="left" width="15">
                         <xsl:value-of select="//SpeedometerOverallTest/@Result"/>
                    </td>
               </tr>
               <tr>
                    <td align="left" width="15">Speed</td>
                    <td width="8">63</td>
                    <td width="8">70</td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertSpeed">
                              <xsl:with-param name="SPEED" select="//SpeedometerSpeedometerTest/ActualSpeed"/>
                              <xsl:with-param name="UNITS" select="//SpeedometerSpeedometerTest/ActualSpeed/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">KPH</td>
                    <td align="left" width="15">
                         <xsl:value-of select="//SpeedometerOverallTest/@Result"/>
                    </td>
               </tr>

               <!-- transmission overall test -->
               <xsl:call-template name="sectionSpacer"/>
               <tr bgcolor="white">
                    <th align="left" width="15">Transmission</th>
                    <td align="left" width="8"></td>
                    <td align="left" width="8"></td>
                    <td align="left" width="8"></td>
                    <td width="8"/>
                    <td align="left">
                         <xsl:value-of select="//TransmissionOverallTest/@Result"/>
                    </td>
               </tr>
               
               <!-- Accel/Transmission test -->
               <xsl:call-template name="sectionSpacer"/>
               <tr bgcolor="white">
                    <th align="left" width="15">Max Spd Test</th>
                    <td align="left" width="8">Target</td>
                    <td align="left" width="8">Actual</td>
                    <td align="left" width="8">Units</td>
                    <td width="8"/>
                    <td align="left">
                         <xsl:value-of select="//TransmissionAccelerationTest/@Result"/>
                    </td>
               </tr>
               <tr>
                    <td align="left" width="15">Max Speed</td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertSpeed">
                              <xsl:with-param name="SPEED" select="//TransmissionAccelerationTest/TargetSpeed"/>
                              <xsl:with-param name="UNITS" select="//TransmissionAccelerationTest/TargetSpeed/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">
                         <xsl:call-template name="convertSpeed">
                              <xsl:with-param name="SPEED" select="//TransmissionAccelerationTest/TopSpeed"/>
                              <xsl:with-param name="UNITS" select="//TransmissionAccelerationTest/TopSpeed/@Units"/>
                         </xsl:call-template>
                    </td>
                    <td align="left" width="8">KPH</td>
               </tr>
               <tr>
                    <td align="left" width="15">Time</td>
                    <td align="left" width="8">
                         <xsl:value-of select="//TransmissionAccelerationTest/MaxTime"/>
                    </td>
                    <td align="left" width="8">
                         <xsl:value-of select="//TransmissionAccelerationTest/ElapsedTime"/>
                    </td>
                    <td align="left" width="8">sec</td>
               </tr>
               <xsl:call-template name="sectionSpacer"/>
               <tr>
                    <th align="left" width="15">Reverse</th>
                    <td width="8"/>
                    <td width="8"/>
                    <td width="8"/>
                    <td width="8"/>
                    <td align="left" width="15">
                         <xsl:value-of select="//TransmissionAccelerateInReverse/@Result"/>
                    </td>
               </tr>
               <xsl:call-template name="sectionSpacer"/>
               <!-- Exhaust test -->
               <xsl:if test="//ExhaustOverallTest">
                    <tr bgcolor="white">
                         <th align="left" width="15">Opacity Test</th>
                         <td align="left" width="8">Min</td>
                         <td align="left" width="8">Max</td>
                         <td align="left" width="8">Actual</td>
                         <td align="left" width="8">Units</td>
                         <td align="left" width="15">
                              <xsl:value-of select="//ExhaustOverallTest/@Result"/>
                         </td>
                    </tr>
                    <tr>
                         <td align="left" width="15">Opacity</td>
                         <td width="8">0</td>
                         <td align="left" width="8">
                              <xsl:value-of select="//ExhaustExhaustLevelTest/MaxExhaustSaturationLevel"/>
                         </td>
                         <td align="left" width="8">
                              <xsl:value-of select="//ExhaustExhaustLevelTest/ExhaustSaturationLevel"/>
                         </td>
                         <td width="8">%</td>
                    </tr>
               </xsl:if>
               <!-- Gas Analyzer test -->
               <xsl:if test="//GasAnalyzerOverallTest">
                    <tr bgcolor="white">
                         <th align="left" width="15">Gas Analyzer Test</th>
                         <td align="left" width="8" />
                         <td align="left" width="8" />
                         <td align="left" width="8" >Actual</td>
                         <td align="left" width="8">Units</td>
                         <td align="left" width="15">
                              <xsl:value-of select="//GasAnalyzerOverallTest/@Result"/>
                         </td>
                    </tr>
                    <tr>
                         <td align="left" width="8">CO2</td>
                         <td align="left" width="8" />
                         <td align="left" width="8" />
                         <td align="left" width="8">
                              <xsl:value-of select="//GasAnalyzerCO2/CO2"/>
                         </td>
                         <td align="left" width="8" >%</td>
                         <td align="left" width="15" />
                    </tr>
                    <tr>
                         <td align="left" width="8">CO</td>
                         <td align="left" width="8" />
                         <td align="left" width="8" />
                         <td align="left" width="8">
                              <xsl:value-of select="//GasAnalyzerCOpercent/COpercent"/>
                         </td>
                         <td align="left" width="8" >%</td>
                         <td align="left" width="15" />
                    </tr>
                    <tr>
                         <td align="left" width="8">HC</td>
                         <td align="left" width="8" />
                         <td align="left" width="8" />
                         <td align="left" width="8">
                              <xsl:value-of select="//GasAnalyzerHCppm/HCppm"/>
                         </td>
                         <td align="left" width="8" >ppm</td>
                         <td align="left" width="15" />
                    </tr>
               </xsl:if>
               <xsl:call-template name="sectionSpacer"/>
               <tr>
                    <th align="left" width="15">Overall Test</th>
                    <td width="8"/>
                    <td width="8"/>
                    <td width="8"/>
                    <td width="8"/>
                    <td align="left" width="15">
                         <xsl:choose>
                              <xsl:when test="//OverallTest/@Result='Pass' and //FrontAxleOverallTest/@Result='Pass'">
                                   <xsl:text>Pass</xsl:text>
                              </xsl:when>
                              <xsl:otherwise>
                                   <xsl:text>Fail</xsl:text>
                              </xsl:otherwise>
                         </xsl:choose>
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
               <th align="left"/>
               <th align="left"/>
               <th align="left">Result</th>
               <th align="left">Measured</th>
               <th align="left"/>
               <th align="left">Result</th>
               <th align="left">Measured</th>
          </tr>
     </xsl:template>
     <xsl:template name="fixedHeaderRow">
          <tr>
               <th align="left" width="50"/>
               <th align="left" width="175"/>
               <th align="left" width="50">Result</th>
               <th align="left" width="100">Measured</th>
               <th align="left" width="175"/>
               <th align="left" width="50">Result</th>
               <th align="left" width="100">Measured</th>
          </tr>
     </xsl:template>
</xsl:stylesheet>

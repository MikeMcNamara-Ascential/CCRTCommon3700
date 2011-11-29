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
    <table border="1" rules="all" cellpadding="1">
      <tr>
        <td align="left" width="17">Date:</td>
        <td width="20">
          <xsl:value-of select="//TestResult/MachineInitialize/Date"/>
        </td>
      </tr>
      <tr>
        <td align="left" width="17">Time:</td>
        <td width="20">
          <xsl:value-of select="//TestResult/MachineInitialize/Time"/>
        </td>
      </tr>
      <tr>
        <td align="left" width="17">Brake Type:</td>
        <td width="20">
          <xsl:value-of select="//BrakeType"/>
        </td>
      </tr>
      <xsl:if test="//TestResult/EmissionsOverallTest">
        <tr>
          <td align="left" width="17">Emissions Type:</td>
          <td width="20">
            <xsl:value-of select="//EmissionsType"/>
          </td>
        </tr>
      </xsl:if>
      <tr>
        <td align="left" width="17">VIN:</td>
        <td width="20">
          <xsl:value-of select="//VIN"/>
        </td>
      </tr>
    </table>
  </xsl:template>

  <!-- Test Results -->
  <xsl:template name="TestResults" match="/BepVehicleTestResult/TestResult">

    <table border="1" rules="all" title="Test Results" cellpadding="1">
      <!-- Brake Test Results -->
      <xsl:call-template name="sectionSpacer"/>
      
      <tr bgcolor="white">
        <th align="center" width="17">Brake Test</th>
        <td align="left">
          <xsl:call-template name="showResult">
            <xsl:with-param name="RESULT" select="//BrakeBrakeTest/@Result"/>
          </xsl:call-template>
        </td>
      </tr>
      <xsl:call-template name="sectionSpacer"/>
      <tr>
        <td align="right" width="17">LF Drag</td>
        <td align="center" width="7">
          <xsl:call-template name="showResult">
            <xsl:with-param name="RESULT" select="//BrakeLeftFrontDragTest/@Result"/>
          </xsl:call-template>
        </td>
        <td align="center" width="15">
          <u>
            <xsl:call-template name="convertForce">
              <xsl:with-param name="FORCE" select="//BrakeLeftFrontDragTest/DragForce"/>
              <xsl:with-param name="UNITS" select="//BrakeLeftFrontDragTest/DragForce/@Units"/>
            </xsl:call-template>
                  KGF
          </u>
        </td>
      </tr>
      <tr>
        <td align="right" width="17">RF Drag</td>
        <td align="center" width="7">
          <xsl:call-template name="showResult">
            <xsl:with-param name="RESULT" select="//BrakeRightFrontDragTest/@Result"/>
          </xsl:call-template>
        </td>
        <td align="center" width="15">
          <u>
            <xsl:call-template name="convertForce">
              <xsl:with-param name="FORCE" select="//BrakeRightFrontDragTest/DragForce"/>
              <xsl:with-param name="UNITS" select="//BrakeRightFrontDragTest/DragForce/@Units"/>
            </xsl:call-template>
                  KGF
          </u>
        </td>
      </tr>
      <tr>
        <td align="right" width="17">LR Drag</td>
        <td align="center" width="7">
          <xsl:call-template name="showResult">
            <xsl:with-param name="RESULT" select="//BrakeLeftRearDragTest/@Result"/>
          </xsl:call-template>
        </td>
        <td align="center" width="15">
          <u>
            <xsl:call-template name="convertForce">
              <xsl:with-param name="FORCE" select="//BrakeLeftRearDragTest/DragForce"/>
              <xsl:with-param name="UNITS" select="//BrakeLeftRearDragTest/DragForce/@Units"/>
            </xsl:call-template>
                  KGF
          </u>
        </td>
      </tr>
      <tr>
        <td align="right" width="17">RR Drag</td>
        <td align="center" width="7">
          <xsl:call-template name="showResult">
            <xsl:with-param name="RESULT" select="//BrakeRightRearDragTest/@Result"/>
          </xsl:call-template>
        </td>
        <td align="center" width="15">
          <u>
            <xsl:call-template name="convertForce">
              <xsl:with-param name="FORCE" select="//BrakeRightRearDragTest/DragForce"/>
              <xsl:with-param name="UNITS" select="//BrakeRightRearDragTest/DragForce/@Units"/>
            </xsl:call-template>
                  KGF
          </u>
        </td>
      </tr>
      <xsl:call-template name="sectionSpacer"/>
      <tr>
        <td align="right" width="17">LF Brake</td>
        <td align="center" width="7">
          <xsl:call-template name="showResult">
            <xsl:with-param name="RESULT" select="//BrakeLeftFrontForceTest/@Result"/>
          </xsl:call-template>
        </td>
        <td align="center" width="15">
          <u>
            <xsl:call-template name="convertForce">
              <xsl:with-param name="FORCE" select="//BrakeLeftFrontForceTest/BrakeForce"/>
              <xsl:with-param name="UNITS" select="//BrakeLeftFrontForceTest/BrakeForce/@Units"/>
            </xsl:call-template>
                  KGF
          </u>
        </td>
      </tr>
      <tr>
        <td align="right" width="17">RF Brake</td>
        <td align="center" width="7">
          <xsl:call-template name="showResult">
            <xsl:with-param name="RESULT" select="//BrakeRightFrontForceTest/@Result"/>
          </xsl:call-template>
        </td>
        <td align="center" width="15">
          <u>
            <xsl:call-template name="convertForce">
              <xsl:with-param name="FORCE" select="//BrakeRightFrontForceTest/BrakeForce"/>
              <xsl:with-param name="UNITS" select="//BrakeRightFrontForceTest/BrakeForce/@Units"/>
            </xsl:call-template>
                  KGF
          </u>
        </td>
      </tr>
      <tr>
        <td align="right" width="17">LR Brake</td>
        <td align="center" width="7">
          <xsl:call-template name="showResult">
            <xsl:with-param name="RESULT" select="//BrakeLeftRearForceTest/@Result"/>
          </xsl:call-template>
        </td>
        <td align="center" width="15">
          <u>
            <xsl:call-template name="convertForce">
              <xsl:with-param name="FORCE" select="//BrakeLeftRearForceTest/BrakeForce"/>
              <xsl:with-param name="UNITS" select="//BrakeLeftRearForceTest/BrakeForce/@Units"/>
            </xsl:call-template>
                  KGF
          </u>
        </td>
      </tr>
      <tr>
        <td align="right" width="17">RR Brake</td>
        <td align="center" width="7">
          <xsl:call-template name="showResult">
            <xsl:with-param name="RESULT" select="//BrakeRightRearForceTest/@Result"/>
          </xsl:call-template>
        </td>
        <td align="center" width="15">
          <u>
            <xsl:call-template name="convertForce">
              <xsl:with-param name="FORCE" select="//BrakeRightRearForceTest/BrakeForce"/>
              <xsl:with-param name="UNITS" select="//BrakeRightRearForceTest/BrakeForce/@Units"/>
            </xsl:call-template>
                  KGF
          </u>
        </td>
      </tr>
      <xsl:call-template name="sectionSpacer"/>
      <tr>
        <td align="right" width="17">Front Bal</td>
        <td align="center" width="7">
          <xsl:call-template name="showResult">
            <xsl:with-param name="RESULT" select="//BrakeFrontBalanceTest/@Result"/>
          </xsl:call-template>
        </td>
        <td align="center" width="15">
          <u>
            <xsl:value-of select="//BrakeFrontBalanceTest/Value"/>
                  %
          </u>
        </td>
      </tr>
      <tr>
        <td align="right" width="17">Rear Bal</td>
        <td align="center" width="7">
          <xsl:call-template name="showResult">
            <xsl:with-param name="RESULT" select="//BrakeRearBalanceTest/@Result"/>
          </xsl:call-template>
        </td>
        <td align="center" width="15">
          <u>
            <xsl:value-of select="//BrakeRearBalanceTest/Value"/>
                  %
          </u>
        </td>
      </tr>
      <tr>
        <td align="right" width="17">Frt/Rr Bal</td>
        <td align="center" width="7">
          <xsl:call-template name="showResult">
            <xsl:with-param name="RESULT" select="//BrakeFrontToRearBalanceTest/@Result"/>
          </xsl:call-template>
        </td>
        <td align="center" width="15">
          <u>
            <xsl:value-of select="//BrakeFrontToRearBalanceTest/Value"/>
                  %
          </u>
        </td>
      </tr>

      <xsl:call-template name="sectionSpacer"/>

      <!-- If there are ABS results, add them to the printout -->
      <xsl:if test="//BrakeLeftFrontABSReduction">
        <tr>
          <td align="right">LF Red</td>
          <td align="center">
            <xsl:call-template name="showResult">
              <xsl:with-param name="RESULT" select="//BrakeLeftFrontABSReduction/@Result"/>
            </xsl:call-template>
          </td>
          <td align="center">
            <u>
              <xsl:value-of select="//BrakeLeftFrontABSReduction/LeftFrontABSReductionPercent"/>
                    %
            </u>
          </td>
          <td align="right">RF Red</td>
          <td align="center">
            <xsl:call-template name="showResult">
              <xsl:with-param name="RESULT" select="//BrakeRightFrontABSReduction/@Result"/>
            </xsl:call-template>
          </td>
          <td align="center">
            <u>
              <xsl:value-of select="//BrakeRightFrontABSReduction/RightFrontABSReductionPercent"/>
                    %
            </u>
          </td>
        </tr>
        <tr>
          <td align="right">LR Red</td>
          <td align="center">
            <xsl:call-template name="showResult">
              <xsl:with-param name="RESULT" select="//BrakeLeftRearABSReduction/@Result"/>
            </xsl:call-template>
          </td>
          <td align="center">
            <u>
              <xsl:value-of select="//BrakeLeftRearABSReduction/LeftRearABSReductionPercent"/>
                    %
            </u>
          </td>
          <td align="right">RR Red</td>
          <td align="center">
            <xsl:call-template name="showResult">
              <xsl:with-param name="RESULT" select="//BrakeRightRearABSReduction/@Result"/>
            </xsl:call-template>
          </td>
          <td align="center">
            <u>
              <xsl:value-of select="//BrakeRightRearABSReduction/RightRearABSReductionPercent"/>
                    %
            </u>
          </td>
        </tr>
  
        <xsl:call-template name="sectionSpacer"/>
  
        <tr>
          <td align="right">LF Rec</td>
          <td align="center">
            <xsl:call-template name="showResult">
              <xsl:with-param name="RESULT" select="//BrakeLeftFrontABSRecovery/@Result"/>
            </xsl:call-template>
          </td>
          <td align="center">
            <u>
              <xsl:value-of select="//BrakeLeftFrontABSRecovery/LeftFrontABSRecoveryValue"/>
                    %
            </u>
          </td>
          <td align="right">RF Rec</td>
          <td align="center">
            <xsl:call-template name="showResult">
              <xsl:with-param name="RESULT" select="//BrakeRightFrontABSRecovery/@Result"/>
            </xsl:call-template>
          </td>
          <td align="center">
            <u>
              <xsl:value-of select="//BrakeRightFrontABSRecovery/RightFrontABSRecoveryValue"/>
                    %
            </u>
          </td>
        </tr>
        <tr>
          <td align="right">LR Rec</td>
          <td align="center">
            <xsl:call-template name="showResult">
              <xsl:with-param name="RESULT" select="//BrakeLeftRearABSRecovery/@Result"/>
            </xsl:call-template>
          </td>
          <td align="center">
            <u>
              <xsl:value-of select="//BrakeLeftRearABSRecovery/LeftRearABSRecoveryValue"/>
                    %
            </u>
          </td>
          <td align="right">RR Rec</td>
          <td align="center">
            <xsl:call-template name="showResult">
              <xsl:with-param name="RESULT" select="//BrakeRightRearABSRecovery/@Result"/>
            </xsl:call-template>
          </td>
          <td align="center">
            <u>
              <xsl:value-of select="//BrakeRightRearABSRecovery/RightRearABSRecoveryValue"/>
                    %
            </u>
          </td>
        </tr>
      </xsl:if>

      <!-- Emission Test Results -->
      <xsl:if test="//EmissionsOverallTest">
        <xsl:call-template name="sectionSpacer"/>
        <xsl:call-template name="sectionSpacer"/>
        <tr bgcolor="white">
          <th align="center" rowspan="6">Emissions Test</th>
          <td align="left" colspan="6">
            <xsl:call-template name="showResult">
              <xsl:with-param name="RESULT" select="//EmissionsOverallTest/@Result"/>
            </xsl:call-template>
          </td>
        </tr>
      </xsl:if>
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


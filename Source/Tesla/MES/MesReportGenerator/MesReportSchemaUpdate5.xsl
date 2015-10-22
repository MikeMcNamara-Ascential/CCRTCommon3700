<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="2.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xalan="http://xml.apache.org/xalan">
    <xsl:output method="xml" version="1.0" indent="yes" xalan:indent-amount="2" cdata-section-elements="BrakeForceData SpeedData"/>
    <xsl:strip-space elements="*"/>

    <xsl:template match="/BepVehicleTestResult">
        <xsl:element name="QualityTest">
            <xsl:element name="QualityTestNumber">
                <xsl:value-of select="//VehicleBuild/VIN"/>
            </xsl:element>
            <xsl:element name="TestDate">
                <xsl:value-of select="//TestResult/MachineInitialize/Date"/>
            </xsl:element>
            <xsl:element name="TestTime">
                <xsl:value-of select="//TestResult/MachineInitialize/Time"/>
            </xsl:element>
            <xsl:call-template name="ChooseMachine">
                <xsl:with-param name="VALUE" select="//TestResult/MachineInitialize/MachineNumber"/>
            </xsl:call-template>
            <xsl:element name="CcrtVersion">
                <xsl:value-of select="//TestResult/MachineInitialize/Version"/>
            </xsl:element>
            <xsl:call-template name="BrakeModule"/>
            <!--
                        <xsl:call-template name="AirbagModule"/>
            -->
            <xsl:call-template name="OverallResult"/>
            <xsl:element name="BrakeForceData">
                <xsl:value-of select="//TestResult/BrakeForceData"/>
            </xsl:element>
            <xsl:element name="SpeedData">
                <xsl:value-of select="//TestResult/SpeedData"/>
            </xsl:element>
        </xsl:element>
    </xsl:template>




    <xsl:template name="BrakeModule" match="//TestResult">
        <xsl:element name="Module">
            <xsl:call-template name="BrakeModuleInfo">
                <xsl:with-param name="NAME">BrakeControlModule</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeLeftFrontDragTest">
                <xsl:with-param name="NAME">BrakeLeftFrontDrag</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeRightFrontDragTest">
                <xsl:with-param name="NAME">BrakeRightFrontDrag</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeLeftRearDragTest">
                <xsl:with-param name="NAME">BrakeLeftRearDrag</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeRightRearDragTest">
                <xsl:with-param name="NAME">BrakeRightRearDrag</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeLeftFrontForceTest">
                <xsl:with-param name="NAME">BrakeLeftFrontForce</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeRightFrontForceTest">
                <xsl:with-param name="NAME">BrakeRightFrontForce</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeLeftRearForceTest">
                <xsl:with-param name="NAME">BrakeLeftRearForce</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeRightRearForceTest">
                <xsl:with-param name="NAME">BrakeRightRearForce</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="ValveFiringTest">
                <xsl:with-param name="NAME">BrakeValveFiring</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeLeftFrontReduction">
                <xsl:with-param name="NAME">BrakeLeftFrontReduction</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeRightFrontReduction">
                <xsl:with-param name="NAME">BrakeRightFrontReduction</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeLeftRearReduction">
                <xsl:with-param name="NAME">BrakeLeftRearReduction</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeRightRearReduction">
                <xsl:with-param name="NAME">BrakeRightRearReduction</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeLeftFrontBuild">
                <xsl:with-param name="NAME">BrakeLeftFrontBuild</xsl:with-param>
                <xsl:with-param name="NAME2">BrakeLeftFrontRecovery</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeRightFrontBuild">
                <xsl:with-param name="NAME">BrakeRightFrontBuild</xsl:with-param>
                <xsl:with-param name="NAME2">BrakeRightFrontRecovery</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeLeftRearBuild">
                <xsl:with-param name="NAME">BrakeLeftRearBuild</xsl:with-param>
                <xsl:with-param name="NAME2">BrakeLeftRearRecovery</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeRightRearBuild">
                <xsl:with-param name="NAME">BrakeRightRearBuild</xsl:with-param>
                <xsl:with-param name="NAME2">BrakeRightRearRecovery</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeAnalyzeBalance">
                <xsl:with-param name="NAME">BrakeAnalyzeBalance</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeFrontToRearBalanceTest">
                <xsl:with-param name="NAME">BrakeFrontToRearBalanceTest</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeFrontBalanceTest">
                <xsl:with-param name="NAME">BrakeFrontBalanceTest</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeRearBalanceTest">
                <xsl:with-param name="NAME">BrakeRearBalanceTest</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeLeftFrontSensor">
                <xsl:with-param name="NAME">BrakeLeftFrontSensor</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeRightFrontSensor">
                <xsl:with-param name="NAME">BrakeRightFrontSensor</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeLeftRearSensor">
                <xsl:with-param name="NAME">BrakeLeftRearSensor</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeRightRearSensor">
                <xsl:with-param name="NAME">BrakeRightRearSensor</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeSensorQualityTest">
                <xsl:with-param name="NAME">BrakeSensorQualityTest</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeESPValveCross">
                <xsl:with-param name="NAME">BrakeValveCross</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeSwitchTest">
                <xsl:with-param name="NAME">BrakeSwitchTest</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeLearnMotorType">
                <xsl:with-param name="NAME">BrakeLearnMotorType</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeBurnish">
                <xsl:with-param name="NAME">BrakeBurnishCycle</xsl:with-param>
            </xsl:call-template>
            <!--
                        <xsl:call-template name="BrakeEPBBurnish">
                            <xsl:with-param name="NAME">BrakeEPBBurnishCycle</xsl:with-param>
                        </xsl:call-template>
            -->
            <xsl:call-template name="BrakeCheckPartNumber">
                <xsl:with-param name="NAME">BrakeCheckPartNumber</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeProgramVIN">
                <xsl:with-param name="NAME">BrakeProgramVIN</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeCheckVIN">
                <xsl:with-param name="NAME">BrakeCheckVIN</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeReadFaults">
                <xsl:with-param name="NAME">BrakeReadFaults</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeDtcs"/>
        </xsl:element>
    </xsl:template>

    <xsl:template name="AirbagModule" match="//TestResult">
        <xsl:element name="Module">
            <xsl:call-template name="AirbagModuleInfo">
                <xsl:with-param name="NAME">AirbagControlModule</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="AirbagProgramVIN">
                <xsl:with-param name="NAME">AirbagProgramVIN</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="AirbagCheckVIN">
                <xsl:with-param name="NAME">AirbagCheckVIN</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="ArmAirbag">
                <xsl:with-param name="NAME">AirbagEnableAirBagModule</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="AirbagDtcCheck">
                <xsl:with-param name="NAME">AirbagReadFaults</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="AirbagDtcs"/>
        </xsl:element>
    </xsl:template>

    <xsl:template name="OverallResult" match="//TestResult">
        <xsl:element name="OverallResult">
            <xsl:value-of select="//TestResult/OverallTest/@Result" />
        </xsl:element>
    </xsl:template>



    <xsl:template name="BrakeModuleInfo">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleName">
            <xsl:value-of select="$NAME"/>
        </xsl:element>
        <xsl:element name="ModulePartNumber">
            <xsl:value-of select="//TestResult/BrakeCheckPartNumber/ModulePartNumber"/>
        </xsl:element>
        <xsl:element name="SerialNumber">
            <xsl:value-of select="//TestResult/BrakeCollectTraceData/SerialNumber"/>
        </xsl:element>
        <xsl:element name="Result">
            <xsl:value-of select="//TestResult/BrakeOverallTest/@Result"/>
        </xsl:element>
    </xsl:template>

    <xsl:template name="AirbagModuleInfo">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleName">
            <xsl:value-of select="$NAME"/>
        </xsl:element>
        <xsl:element name="ModulePartNumber">
            <xsl:value-of select="//TestResult/AirbagCheckP3_ModulePartNumber/ModulePartNumber"/>
        </xsl:element>
        <xsl:element name="SerialNumber">
            <xsl:value-of select="//TestResult/AirbagCollectTraceData/SerialNumber"/>
        </xsl:element>

        <xsl:element name="OemPartNumber">
            <xsl:value-of select="//TestResult/AirbagCheckP1_OEMPartNumber/ModulePartNumber"/>
        </xsl:element>
        <xsl:element name="SoftwareVersion">
            <xsl:value-of select="//TestResult/AirbagCheckP2_FWChecksum/ModulePartNumber"/>
        </xsl:element>

        <xsl:element name="Result">
            <xsl:value-of select="//TestResult/AirbagOverallTest/@Result"/>
        </xsl:element>
    </xsl:template>

    <xsl:template name="AirbagCheckVIN">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/AirbagCheckVIN/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/AirbagCheckVIN/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/AirbagCheckVIN/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/AirbagCheckVIN/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units"/>
                <xsl:attribute name="Max"/>
                <xsl:attribute name="Min"/>
            </xsl:element>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/AirbagCheckVIN/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:call-template name="ShowUnits">
                                    <xsl:with-param name="UNITS" select="@Units"/>
                                </xsl:call-template>
                            </xsl:attribute>
                            <xsl:value-of select="."/>
                        </xsl:element>
                    </xsl:element>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <xsl:template name="AirbagProgramVIN">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/AirbagProgramVIN/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/AirbagProgramVIN/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/AirbagProgramVIN/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/AirbagProgramVIN/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units"/>
                <xsl:attribute name="Max"/>
                <xsl:attribute name="Min"/>
            </xsl:element>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/AirbagProgramVIN/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:call-template name="ShowUnits">
                                    <xsl:with-param name="UNITS" select="@Units"/>
                                </xsl:call-template>
                            </xsl:attribute>
                            <xsl:value-of select="."/>
                        </xsl:element>
                    </xsl:element>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <xsl:template name="ArmAirbag">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/AirbagEnableAirBagModule/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/AirbagEnableAirBagModule/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/AirbagEnableAirBagModule/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/AirbagEnableAirBagModule/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units"/>
                <xsl:attribute name="Max"/>
                <xsl:attribute name="Min"/>
            </xsl:element>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/AirbagEnableAirBagModule/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:call-template name="ShowUnits">
                                    <xsl:with-param name="UNITS" select="@Units"/>
                                </xsl:call-template>
                            </xsl:attribute>
                            <xsl:value-of select="."/>
                        </xsl:element>
                    </xsl:element>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <xsl:template name="AirbagDtcCheck">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/AirbagReadFaults/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/AirbagReadFaults/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/AirbagReadFaults/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/AirbagReadFaults/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units"/>
                <xsl:attribute name="Max"/>
                <xsl:attribute name="Min"/>
            </xsl:element>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/AirbagReadFaults/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:call-template name="ShowUnits">
                                    <xsl:with-param name="UNITS" select="@Units"/>
                                </xsl:call-template>
                            </xsl:attribute>
                            <xsl:value-of select="."/>
                        </xsl:element>
                    </xsl:element>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>


    <xsl:template name="ValveFiringTest">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:choose>
                <xsl:when test="string-length(//TestResult/BrakeValveFiringTest/@Result) != 0">
                    <xsl:element name="Process">
                        <xsl:value-of select="//TestResult/BrakeValveFiringTest/@Process"/>
                    </xsl:element>
                    <xsl:element name="Result">
                        <xsl:value-of select="//TestResult/BrakeValveFiringTest/@Result"/>
                    </xsl:element>
                    <xsl:element name="ResultDescription">
                        <xsl:value-of select="//TestResult/BrakeValveFiringTest/@Description"/>
                    </xsl:element>
                    <xsl:element name="TestResultCode">
                        <xsl:value-of select="//TestResult/BrakeValveFiringTest/@TestResultCode"/>
                    </xsl:element>
                    <xsl:element name="TestValue">
                        <xsl:attribute name="Units"/>
                        <xsl:attribute name="Max"/>
                        <xsl:attribute name="Min"/>
                    </xsl:element>
                    <xsl:element name="TestAttributes">
                        <xsl:for-each select="//TestResult/BrakeValveFiringTest/child::*">
                            <xsl:element name="Attribute">
                                <xsl:element name="AttributeName">
                                    <xsl:value-of select="name()"/>
                                </xsl:element>
                                <xsl:element name="AttributeValue">
                                    <xsl:attribute name="Units">
                                        <xsl:call-template name="ShowUnits">
                                            <xsl:with-param name="UNITS" select="@Units"/>
                                        </xsl:call-template>
                                    </xsl:attribute>
                                    <xsl:value-of select="."/>
                                </xsl:element>
                            </xsl:element>
                        </xsl:for-each>
                    </xsl:element>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:element name="Process">
                        <xsl:value-of select="//TestResult/BrakeESPValveFiringTest/@Process"/>
                    </xsl:element>
                    <xsl:element name="Result">
                        <xsl:value-of select="//TestResult/BrakeESPValveFiringTest/@Result"/>
                    </xsl:element>
                    <xsl:element name="ResultDescription">
                        <xsl:value-of select="//TestResult/BrakeESPValveFiringTest/@Description"/>
                    </xsl:element>
                    <xsl:element name="TestResultCode">
                        <xsl:value-of select="//TestResult/BrakeESPValveFiringTest/@TestResultCode"/>
                    </xsl:element>
                    <xsl:element name="TestValue">
                        <xsl:attribute name="Units"/>
                        <xsl:attribute name="Max"/>
                        <xsl:attribute name="Min"/>
                    </xsl:element>
                    <xsl:element name="TestAttributes">
                        <xsl:for-each select="//TestResult/BrakeESPValveFiringTest/child::*">
                            <xsl:element name="Attribute">
                                <xsl:element name="AttributeName">
                                    <xsl:value-of select="name()"/>
                                </xsl:element>
                                <xsl:element name="AttributeValue">
                                    <xsl:attribute name="Units">
                                        <xsl:call-template name="ShowUnits">
                                            <xsl:with-param name="UNITS" select="@Units"/>
                                        </xsl:call-template>
                                    </xsl:attribute>
                                    <xsl:value-of select="."/>
                                </xsl:element>
                            </xsl:element>
                        </xsl:for-each>
                    </xsl:element>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeLeftFrontReduction">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:choose>
                <xsl:when test="string-length(//TestResult/BrakeLeftFrontABSReduction/@Result) != 0">
                    <xsl:element name="Process">
                        <xsl:value-of select="//TestResult/BrakeLeftFrontABSReduction/@Process"/>
                    </xsl:element>
                    <xsl:element name="Result">
                        <xsl:value-of select="//TestResult/BrakeLeftFrontABSReduction/@Result"/>
                    </xsl:element>
                    <xsl:element name="ResultDescription">
                        <xsl:value-of select="//TestResult/BrakeLeftFrontABSReduction/@Description"/>
                    </xsl:element>
                    <xsl:element name="TestResultCode">
                        <xsl:value-of select="//TestResult/BrakeLeftFrontABSReduction/@TestResultCode"/>
                    </xsl:element>
                    <xsl:element name="TestValue">
                        <xsl:attribute name="Units">
                            <xsl:call-template name="ShowUnits">
                                <xsl:with-param name="UNITS" select="//TestResult/BrakeLeftFrontABSReduction/LeftFrontABSReductionPercent/@Units"/>
                            </xsl:call-template>
                        </xsl:attribute>
                        <xsl:attribute name="Max">
                            <xsl:value-of select="normalize-space(//TestResult/BrakeLeftFrontABSReduction/LeftFrontReductionMinParameter)"/>
                        </xsl:attribute>
                        <xsl:attribute name="Min"/>
                        <xsl:value-of select="normalize-space(//TestResult/BrakeLeftFrontABSReduction/LeftFrontABSReductionPercent)"/>
                    </xsl:element>
                    <xsl:element name="TestAttributes"/>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:element name="Process">
                        <xsl:value-of select="//TestResult/BrakeLeftFrontESPReduction/@Process"/>
                    </xsl:element>
                    <xsl:element name="Result">
                        <xsl:value-of select="//TestResult/BrakeLeftFrontESPReduction/@Result"/>
                    </xsl:element>
                    <xsl:element name="ResultDescription">
                        <xsl:value-of select="//TestResult/BrakeLeftFrontESPReduction/@Description"/>
                    </xsl:element>
                    <xsl:element name="TestResultCode">
                        <xsl:value-of select="//TestResult/BrakeLeftFrontESPReduction/@TestResultCode"/>
                    </xsl:element>
                    <xsl:element name="TestValue">
                        <xsl:attribute name="Units">
                            <xsl:call-template name="ShowUnits">
                                <xsl:with-param name="UNITS" select="//TestResult/BrakeLeftFrontESPReduction/LeftFrontESPReductionValue/@Units"/>
                            </xsl:call-template>
                        </xsl:attribute>
                        <xsl:attribute name="Max">
                            <xsl:value-of select="normalize-space(//TestResult/BrakeLeftFrontESPReduction/LeftFrontESPReductionMaxForce)"/>
                        </xsl:attribute>
                        <xsl:attribute name="Min"/>
                        <xsl:value-of select="normalize-space(//TestResult/BrakeLeftFrontESPReduction/LeftFrontESPReductionValue)"/>
                    </xsl:element>
                    <xsl:element name="TestAttributes"/>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeRightFrontReduction">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:choose>
                <xsl:when test="string-length(//TestResult/BrakeRightFrontABSReduction/@Result) != 0">
                    <xsl:element name="Process">
                        <xsl:value-of select="//TestResult/BrakeRightFrontABSReduction/@Process"/>
                    </xsl:element>
                    <xsl:element name="Result">
                        <xsl:value-of select="//TestResult/BrakeRightFrontABSReduction/@Result"/>
                    </xsl:element>
                    <xsl:element name="ResultDescription">
                        <xsl:value-of select="//TestResult/BrakeRightFrontABSReduction/@Description"/>
                    </xsl:element>
                    <xsl:element name="TestResultCode">
                        <xsl:value-of select="//TestResult/BrakeRightFrontABSReduction/@TestResultCode"/>
                    </xsl:element>
                    <xsl:element name="TestValue">
                        <xsl:attribute name="Units">
                            <xsl:call-template name="ShowUnits">
                                <xsl:with-param name="UNITS" select="//TestResult/BrakeRightFrontABSReduction/RightFrontABSReductionPercent/@Units"/>
                            </xsl:call-template>
                        </xsl:attribute>
                        <xsl:attribute name="Max">
                            <xsl:value-of select="normalize-space(//TestResult/BrakeRightFrontABSReduction/RightFrontReductionMinParameter)"/>
                        </xsl:attribute>
                        <xsl:attribute name="Min"/>
                        <xsl:value-of select="normalize-space(//TestResult/BrakeRightFrontABSReduction/RightFrontABSReductionPercent)"/>
                    </xsl:element>
                    <xsl:element name="TestAttributes"/>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:element name="Process">
                        <xsl:value-of select="//TestResult/BrakeRightFrontESPReduction/@Process"/>
                    </xsl:element>
                    <xsl:element name="Result">
                        <xsl:value-of select="//TestResult/BrakeRightFrontESPReduction/@Result"/>
                    </xsl:element>
                    <xsl:element name="ResultDescription">
                        <xsl:value-of select="//TestResult/BrakeRightFrontESPReduction/@Description"/>
                    </xsl:element>
                    <xsl:element name="TestResultCode">
                        <xsl:value-of select="//TestResult/BrakeRightFrontESPReduction/@TestResultCode"/>
                    </xsl:element>
                    <xsl:element name="TestValue">
                        <xsl:attribute name="Units">
                            <xsl:call-template name="ShowUnits">
                                <xsl:with-param name="UNITS" select="//TestResult/BrakeRightFrontESPReduction/RightFrontESPReductionValue/@Units"/>
                            </xsl:call-template>
                        </xsl:attribute>
                        <xsl:attribute name="Max">
                            <xsl:value-of select="normalize-space(//TestResult/BrakeRightFrontESPReduction/RightFrontESPReductionMaxForce)"/>
                        </xsl:attribute>
                        <xsl:attribute name="Min"/>
                        <xsl:value-of select="normalize-space(//TestResult/BrakeRightFrontESPReduction/RightFrontESPReductionValue)"/>
                    </xsl:element>
                    <xsl:element name="TestAttributes"/>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeLeftRearReduction">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:choose>
                <xsl:when test="string-length(//TestResult/BrakeLeftRearABSReduction/@Result) != 0">
                    <xsl:element name="Process">
                        <xsl:value-of select="//TestResult/BrakeLeftRearABSReduction/@Process"/>
                    </xsl:element>
                    <xsl:element name="Result">
                        <xsl:value-of select="//TestResult/BrakeLeftRearABSReduction/@Result"/>
                    </xsl:element>
                    <xsl:element name="ResultDescription">
                        <xsl:value-of select="//TestResult/BrakeLeftRearABSReduction/@Description"/>
                    </xsl:element>
                    <xsl:element name="TestResultCode">
                        <xsl:value-of select="//TestResult/BrakeLeftRearABSReduction/@TestResultCode"/>
                    </xsl:element>
                    <xsl:element name="TestValue">
                        <xsl:attribute name="Units">
                            <xsl:call-template name="ShowUnits">
                                <xsl:with-param name="UNITS" select="//TestResult/BrakeLeftRearABSReduction/LeftRearABSReductionPercent/@Units"/>
                            </xsl:call-template>
                        </xsl:attribute>
                        <xsl:attribute name="Max">
                            <xsl:value-of select="normalize-space(//TestResult/BrakeLeftRearABSReduction/LeftRearReductionMinParameter)"/>
                        </xsl:attribute>
                        <xsl:attribute name="Min"/>
                        <xsl:value-of select="normalize-space(//TestResult/BrakeLeftRearABSReduction/LeftRearABSReductionPercent)"/>
                    </xsl:element>
                    <xsl:element name="TestAttributes"/>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:element name="Process">
                        <xsl:value-of select="//TestResult/BrakeLeftRearESPReduction/@Process"/>
                    </xsl:element>
                    <xsl:element name="Result">
                        <xsl:value-of select="//TestResult/BrakeLeftRearESPReduction/@Result"/>
                    </xsl:element>
                    <xsl:element name="ResultDescription">
                        <xsl:value-of select="//TestResult/BrakeLeftRearESPReduction/@Description"/>
                    </xsl:element>
                    <xsl:element name="TestResultCode">
                        <xsl:value-of select="//TestResult/BrakeLeftRearESPReduction/@TestResultCode"/>
                    </xsl:element>
                    <xsl:element name="TestValue">
                        <xsl:attribute name="Units">
                            <xsl:call-template name="ShowUnits">
                                <xsl:with-param name="UNITS" select="//TestResult/BrakeLeftRearESPReduction/LeftRearESPReductionValue/@Units"/>
                            </xsl:call-template>
                        </xsl:attribute>
                        <xsl:attribute name="Max">
                            <xsl:value-of select="normalize-space(//TestResult/BrakeLeftRearESPReduction/LeftRearESPReductionMaxForce)"/>
                        </xsl:attribute>
                        <xsl:attribute name="Min"/>
                        <xsl:value-of select="normalize-space(//TestResult/BrakeLeftRearESPReduction/LeftRearESPReductionValue)"/>
                    </xsl:element>
                    <xsl:element name="TestAttributes"/>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeRightRearReduction">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:choose>
                <xsl:when test="string-length(//TestResult/BrakeRightRearABSReduction/@Result) != 0">
                    <xsl:element name="Process">
                        <xsl:value-of select="//TestResult/BrakeRightRearABSReduction/@Process"/>
                    </xsl:element>
                    <xsl:element name="Result">
                        <xsl:value-of select="//TestResult/BrakeRightRearABSReduction/@Result"/>
                    </xsl:element>
                    <xsl:element name="ResultDescription">
                        <xsl:value-of select="//TestResult/BrakeRightRearABSReduction/@Description"/>
                    </xsl:element>
                    <xsl:element name="TestResultCode">
                        <xsl:value-of select="//TestResult/BrakeRightRearABSReduction/@TestResultCode"/>
                    </xsl:element>
                    <xsl:element name="TestValue">
                        <xsl:attribute name="Units">
                            <xsl:call-template name="ShowUnits">
                                <xsl:with-param name="UNITS" select="//TestResult/BrakeRightRearABSReduction/RightRearABSReductionPercent/@Units"/>
                            </xsl:call-template>
                        </xsl:attribute>
                        <xsl:attribute name="Max">
                            <xsl:value-of select="normalize-space(//TestResult/BrakeRightRearABSReduction/RightRearReductionMinParameter)"/>
                        </xsl:attribute>
                        <xsl:attribute name="Min"/>
                        <xsl:value-of select="normalize-space(//TestResult/BrakeRightRearABSReduction/RightRearABSReductionPercent)"/>
                    </xsl:element>
                    <xsl:element name="TestAttributes"/>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:element name="Process">
                        <xsl:value-of select="//TestResult/BrakeRightRearESPReduction/@Process"/>
                    </xsl:element>
                    <xsl:element name="Result">
                        <xsl:value-of select="//TestResult/BrakeRightRearESPReduction/@Result"/>
                    </xsl:element>
                    <xsl:element name="ResultDescription">
                        <xsl:value-of select="//TestResult/BrakeRightRearESPReduction/@Description"/>
                    </xsl:element>
                    <xsl:element name="TestResultCode">
                        <xsl:value-of select="//TestResult/BrakeRightRearESPReduction/@TestResultCode"/>
                    </xsl:element>
                    <xsl:element name="TestValue">
                        <xsl:attribute name="Units">
                            <xsl:call-template name="ShowUnits">
                                <xsl:with-param name="UNITS" select="//TestResult/BrakeRightRearESPReduction/RightRearESPReductionValue/@Units"/>
                            </xsl:call-template>
                        </xsl:attribute>
                        <xsl:attribute name="Max">
                            <xsl:value-of select="normalize-space(//TestResult/BrakeRightRearESPReduction/RightRearESPReductionMaxForce)"/>
                        </xsl:attribute>
                        <xsl:attribute name="Min"/>
                        <xsl:value-of select="normalize-space(//TestResult/BrakeRightRearESPReduction/RightRearESPReductionValue)"/>
                    </xsl:element>
                    <xsl:element name="TestAttributes"/>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeLeftFrontBuild">
        <xsl:param name="NAME"/>
        <xsl:param name="NAME2"/>
        <xsl:element name="ModuleTest">
            <xsl:choose>
                <xsl:when test="string-length(//TestResult/BrakeLeftFrontABSRecovery/@Result) != 0">
                    <xsl:element name="TestName">
                        <xsl:value-of select="$NAME2"/>
                    </xsl:element>
                    <xsl:element name="Process">
                        <xsl:value-of select="//TestResult/BrakeLeftFrontABSRecovery/@Process"/>
                    </xsl:element>
                    <xsl:element name="Result">
                        <xsl:value-of select="//TestResult/BrakeLeftFrontABSRecovery/@Result"/>
                    </xsl:element>
                    <xsl:element name="ResultDescription">
                        <xsl:value-of select="//TestResult/BrakeLeftFrontABSRecovery/@Description"/>
                    </xsl:element>
                    <xsl:element name="TestResultCode">
                        <xsl:value-of select="//TestResult/BrakeLeftFrontABSRecovery/@TestResultCode"/>
                    </xsl:element>
                    <xsl:element name="TestValue">
                        <xsl:attribute name="Units">
                            <xsl:call-template name="ShowUnits">
                                <xsl:with-param name="UNITS" select="//TestResult/BrakeLeftFrontABSRecovery/LeftFrontABSRecoveryValue/@Units"/>
                            </xsl:call-template>
                        </xsl:attribute>
                        <xsl:attribute name="Max"/>
                        <xsl:attribute name="Min">
                            <xsl:value-of select="normalize-space(//TestResult/BrakeLeftFrontABSRecovery/LeftFrontRecoveryMinPercent)"/>
                        </xsl:attribute>
                        <xsl:value-of select="normalize-space(//TestResult/BrakeLeftFrontABSRecovery/LeftFrontABSRecoveryValue)"/>
                    </xsl:element>
                    <xsl:element name="TestAttributes"/>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:element name="TestName">
                        <xsl:value-of select="$NAME"/>
                    </xsl:element>
                    <xsl:element name="Process">
                        <xsl:value-of select="//TestResult/BrakeLeftFrontESPBuild/@Process"/>
                    </xsl:element>
                    <xsl:element name="Result">
                        <xsl:value-of select="//TestResult/BrakeLeftFrontESPBuild/@Result"/>
                    </xsl:element>
                    <xsl:element name="ResultDescription">
                        <xsl:value-of select="//TestResult/BrakeLeftFrontESPBuild/@Description"/>
                    </xsl:element>
                    <xsl:element name="TestResultCode">
                        <xsl:value-of select="//TestResult/BrakeLeftFrontESPBuild/@TestResultCode"/>
                    </xsl:element>
                    <xsl:element name="TestValue">
                        <xsl:attribute name="Units">
                            <xsl:call-template name="ShowUnits">
                                <xsl:with-param name="UNITS" select="//TestResult/BrakeLeftFrontESPBuild/LeftFrontESPBuildValue/@Units"/>
                            </xsl:call-template>
                        </xsl:attribute>
                        <xsl:attribute name="Max"/>
                        <xsl:attribute name="Min">
                            <xsl:value-of select="normalize-space(//TestResult/BrakeLeftFrontESPBuild/LeftFrontESPBuildMinForce)"/>
                        </xsl:attribute>
                        <xsl:value-of select="normalize-space(//TestResult/BrakeLeftFrontESPBuild/LeftFrontESPBuildValue)"/>
                    </xsl:element>
                    <xsl:element name="TestAttributes"/>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeRightFrontBuild">
        <xsl:param name="NAME"/>
        <xsl:param name="NAME2"/>
        <xsl:element name="ModuleTest">
            <xsl:choose>
                <xsl:when test="string-length(//TestResult/BrakeRightFrontABSRecovery/@Result) != 0">
                    <xsl:element name="TestName">
                        <xsl:value-of select="$NAME2"/>
                    </xsl:element>
                    <xsl:element name="Process">
                        <xsl:value-of select="//TestResult/BrakeRightFrontABSRecovery/@Process"/>
                    </xsl:element>
                    <xsl:element name="Result">
                        <xsl:value-of select="//TestResult/BrakeRightFrontABSRecovery/@Result"/>
                    </xsl:element>
                    <xsl:element name="ResultDescription">
                        <xsl:value-of select="//TestResult/BrakeRightFrontABSRecovery/@Description"/>
                    </xsl:element>
                    <xsl:element name="TestResultCode">
                        <xsl:value-of select="//TestResult/BrakeRightFrontABSRecovery/@TestResultCode"/>
                    </xsl:element>
                    <xsl:element name="TestValue">
                        <xsl:attribute name="Units">
                            <xsl:call-template name="ShowUnits">
                                <xsl:with-param name="UNITS" select="//TestResult/BrakeRightFrontABSRecovery/RightFrontABSRecoveryValue/@Units"/>
                            </xsl:call-template>
                        </xsl:attribute>
                        <xsl:attribute name="Max"/>
                        <xsl:attribute name="Min">
                            <xsl:value-of select="normalize-space(//TestResult/BrakeRightFrontABSRecovery/RightFrontRecoveryMinParameter)"/>
                        </xsl:attribute>
                        <xsl:value-of select="normalize-space(//TestResult/BrakeRightFrontABSRecovery/RightFrontABSRecoveryValue)"/>
                    </xsl:element>
                    <xsl:element name="TestAttributes"/>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:element name="TestName">
                        <xsl:value-of select="$NAME"/>
                    </xsl:element>
                    <xsl:element name="Process">
                        <xsl:value-of select="//TestResult/BrakeRightFrontESPBuild/@Process"/>
                    </xsl:element>
                    <xsl:element name="Result">
                        <xsl:value-of select="//TestResult/BrakeRightFrontESPBuild/@Result"/>
                    </xsl:element>
                    <xsl:element name="ResultDescription">
                        <xsl:value-of select="//TestResult/BrakeRightFrontESPBuild/@Description"/>
                    </xsl:element>
                    <xsl:element name="TestResultCode">
                        <xsl:value-of select="//TestResult/BrakeRightFrontESPBuild/@TestResultCode"/>
                    </xsl:element>
                    <xsl:element name="TestValue">
                        <xsl:attribute name="Units">
                            <xsl:call-template name="ShowUnits">
                                <xsl:with-param name="UNITS" select="//TestResult/BrakeRightFrontESPBuild/RightFrontESPBuildValue/@Units"/>
                            </xsl:call-template>
                        </xsl:attribute>
                        <xsl:attribute name="Max"/>
                        <xsl:attribute name="Min">
                            <xsl:value-of select="normalize-space(//TestResult/BrakeRightFrontESPBuild/RightFrontESPBuildMinForce)"/>
                        </xsl:attribute>
                        <xsl:value-of select="normalize-space(//TestResult/BrakeRightFrontESPBuild/RightFrontESPBuildValue)"/>
                    </xsl:element>
                    <xsl:element name="TestAttributes"/>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeLeftRearBuild">
        <xsl:param name="NAME"/>
        <xsl:param name="NAME2"/>
        <xsl:element name="ModuleTest">
            <xsl:choose>
                <xsl:when test="string-length(//TestResult/BrakeLeftRearABSRecovery/@Result) != 0">
                    <xsl:element name="TestName">
                        <xsl:value-of select="$NAME2"/>
                    </xsl:element>
                    <xsl:element name="Process">
                        <xsl:value-of select="//TestResult/BrakeLeftRearABSRecovery/@Process"/>
                    </xsl:element>
                    <xsl:element name="Result">
                        <xsl:value-of select="//TestResult/BrakeLeftRearABSRecovery/@Result"/>
                    </xsl:element>
                    <xsl:element name="ResultDescription">
                        <xsl:value-of select="//TestResult/BrakeLeftRearABSRecovery/@Description"/>
                    </xsl:element>
                    <xsl:element name="TestResultCode">
                        <xsl:value-of select="//TestResult/BrakeLeftRearABSRecovery/@TestResultCode"/>
                    </xsl:element>
                    <xsl:element name="TestValue">
                        <xsl:attribute name="Units">
                            <xsl:call-template name="ShowUnits">
                                <xsl:with-param name="UNITS" select="//TestResult/BrakeLeftRearABSRecovery/LeftRearABSRecoveryValue/@Units"/>
                            </xsl:call-template>
                        </xsl:attribute>
                        <xsl:attribute name="Max"/>
                        <xsl:attribute name="Min">
                            <xsl:value-of select="normalize-space(//TestResult/BrakeLeftRearABSRecovery/LeftRearRecoveryMinParameter)"/>
                        </xsl:attribute>
                        <xsl:value-of select="normalize-space(//TestResult/BrakeLeftRearABSRecovery/LeftRearABSRecoveryValue)"/>
                    </xsl:element>
                    <xsl:element name="TestAttributes"/>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:element name="TestName">
                        <xsl:value-of select="$NAME"/>
                    </xsl:element>
                    <xsl:element name="Process">
                        <xsl:value-of select="//TestResult/BrakeLeftRearESPBuild/@Process"/>
                    </xsl:element>
                    <xsl:element name="Result">
                        <xsl:value-of select="//TestResult/BrakeLeftRearESPBuild/@Result"/>
                    </xsl:element>
                    <xsl:element name="ResultDescription">
                        <xsl:value-of select="//TestResult/BrakeLeftRearESPBuild/@Description"/>
                    </xsl:element>
                    <xsl:element name="TestResultCode">
                        <xsl:value-of select="//TestResult/BrakeLeftRearESPBuild/@TestResultCode"/>
                    </xsl:element>
                    <xsl:element name="TestValue">
                        <xsl:attribute name="Units">
                            <xsl:call-template name="ShowUnits">
                                <xsl:with-param name="UNITS" select="//TestResult/BrakeLeftRearESPBuild/LeftRearESPBuildValue/@Units"/>
                            </xsl:call-template>
                        </xsl:attribute>
                        <xsl:attribute name="Max"/>
                        <xsl:attribute name="Min">
                            <xsl:value-of select="normalize-space(//TestResult/BrakeLeftRearESPBuild/LeftRearESPBuildMinForce)"/>
                        </xsl:attribute>
                        <xsl:value-of select="normalize-space(//TestResult/BrakeLeftRearESPBuild/LeftRearESPBuildValue)"/>
                    </xsl:element>
                    <xsl:element name="TestAttributes"/>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeRightRearBuild">
        <xsl:param name="NAME"/>
        <xsl:param name="NAME2"/>
        <xsl:element name="ModuleTest">
            <xsl:choose>
                <xsl:when test="string-length(//TestResult/BrakeRightRearABSRecovery/@Result) != 0">
                    <xsl:element name="TestName">
                        <xsl:value-of select="$NAME2"/>
                    </xsl:element>
                    <xsl:element name="Process">
                        <xsl:value-of select="//TestResult/BrakeRightRearABSRecovery/@Process"/>
                    </xsl:element>
                    <xsl:element name="Result">
                        <xsl:value-of select="//TestResult/BrakeRightRearABSRecovery/@Result"/>
                    </xsl:element>
                    <xsl:element name="ResultDescription">
                        <xsl:value-of select="//TestResult/BrakeRightRearABSRecovery/@Description"/>
                    </xsl:element>
                    <xsl:element name="TestResultCode">
                        <xsl:value-of select="//TestResult/BrakeRightRearABSRecovery/@TestResultCode"/>
                    </xsl:element>
                    <xsl:element name="TestValue">
                        <xsl:attribute name="Units">
                            <xsl:call-template name="ShowUnits">
                                <xsl:with-param name="UNITS" select="//TestResult/BrakeRightRearABSRecovery/RightRearABSRecoveryValue/@Units"/>
                            </xsl:call-template>
                        </xsl:attribute>
                        <xsl:attribute name="Max"/>
                        <xsl:attribute name="Min">
                            <xsl:value-of select="normalize-space(//TestResult/BrakeRightRearABSRecovery/RightRearRecoveryMinParameter)"/>
                        </xsl:attribute>
                        <xsl:value-of select="normalize-space(//TestResult/BrakeRightRearABSRecovery/RightRearABSRecoveryValue)"/>
                    </xsl:element>
                    <xsl:element name="TestAttributes"/>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:element name="TestName">
                        <xsl:value-of select="$NAME"/>
                    </xsl:element>
                    <xsl:element name="Process">
                        <xsl:value-of select="//TestResult/BrakeRightRearESPBuild/@Process"/>
                    </xsl:element>
                    <xsl:element name="Result">
                        <xsl:value-of select="//TestResult/BrakeRightRearESPBuild/@Result"/>
                    </xsl:element>
                    <xsl:element name="ResultDescription">
                        <xsl:value-of select="//TestResult/BrakeRightRearESPBuild/@Description"/>
                    </xsl:element>
                    <xsl:element name="TestResultCode">
                        <xsl:value-of select="//TestResult/BrakeRightRearESPBuild/@TestResultCode"/>
                    </xsl:element>
                    <xsl:element name="TestValue">
                        <xsl:attribute name="Units">
                            <xsl:call-template name="ShowUnits">
                                <xsl:with-param name="UNITS" select="//TestResult/BrakeRightRearESPBuild/RightRearESPBuildValue/@Units"/>
                            </xsl:call-template>
                        </xsl:attribute>
                        <xsl:attribute name="Max"/>
                        <xsl:attribute name="Min">
                            <xsl:value-of select="normalize-space(//TestResult/BrakeRightRearESPBuild/RightRearESPBuildMinForce)"/>
                        </xsl:attribute>
                        <xsl:value-of select="normalize-space(//TestResult/BrakeRightRearESPBuild/RightRearESPBuildValue)"/>
                    </xsl:element>
                    <xsl:element name="TestAttributes"/>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeLeftFrontDragTest">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeLeftFrontDragTest/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeLeftFrontDragTest/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeLeftFrontDragTest/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeLeftFrontDragTest/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units">
                    <xsl:call-template name="ShowUnits">
                        <xsl:with-param name="UNITS" select="//TestResult/BrakeLeftFrontDragTest/DragForce/@Units"/>
                    </xsl:call-template>
                </xsl:attribute>
                <xsl:attribute name="Max">
                    <xsl:value-of select="//TestResult/BrakeLeftFrontDragTest/MaxDragForce"/>
                </xsl:attribute>
                <xsl:attribute name="Min"/>
                <xsl:value-of select="//TestResult/BrakeLeftFrontDragTest/DragForce"/>
            </xsl:element>
            <xsl:element name="TestAttributes"/>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeRightFrontDragTest">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeRightFrontDragTest/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeRightFrontDragTest/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeRightFrontDragTest/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeRightFrontDragTest/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units">
                    <xsl:call-template name="ShowUnits">
                        <xsl:with-param name="UNITS" select="//TestResult/BrakeRightFrontDragTest/DragForce/@Units"/>
                    </xsl:call-template>
                </xsl:attribute>
                <xsl:attribute name="Max">
                    <xsl:value-of select="//TestResult/BrakeRightFrontDragTest/MaxDragForce"/>
                </xsl:attribute>
                <xsl:attribute name="Min"/>
                <xsl:value-of select="//TestResult/BrakeRightFrontDragTest/DragForce"/>
            </xsl:element>
            <xsl:element name="TestAttributes"/>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeLeftRearDragTest">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeLeftRearDragTest/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeLeftRearDragTest/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeLeftRearDragTest/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeLeftRearDragTest/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units">
                    <xsl:call-template name="ShowUnits">
                        <xsl:with-param name="UNITS" select="//TestResult/BrakeLeftRearDragTest/DragForce/@Units"/>
                    </xsl:call-template>
                </xsl:attribute>
                <xsl:attribute name="Max">
                    <xsl:value-of select="//TestResult/BrakeLeftRearDragTest/MaxDragForce"/>
                </xsl:attribute>
                <xsl:attribute name="Min"/>
                <xsl:value-of select="//TestResult/BrakeLeftRearDragTest/DragForce"/>
            </xsl:element>
            <xsl:element name="TestAttributes"/>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeRightRearDragTest">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeRightRearDragTest/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeRightRearDragTest/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeRightRearDragTest/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeRightRearDragTest/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units">
                    <xsl:call-template name="ShowUnits">
                        <xsl:with-param name="UNITS" select="//TestResult/BrakeRightRearDragTest/DragForce/@Units"/>
                    </xsl:call-template>
                </xsl:attribute>
                <xsl:attribute name="Max">
                    <xsl:value-of select="//TestResult/BrakeRightRearDragTest/MaxDragForce"/>
                </xsl:attribute>
                <xsl:attribute name="Min"/>
                <xsl:value-of select="//TestResult/BrakeRightRearDragTest/DragForce"/>
            </xsl:element>
            <xsl:element name="TestAttributes"/>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeLeftFrontForceTest">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeLeftFrontForceTest/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeLeftFrontForceTest/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeLeftFrontForceTest/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeLeftFrontForceTest/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units">
                    <xsl:call-template name="ShowUnits">
                        <xsl:with-param name="UNITS" select="//TestResult/BrakeLeftFrontForceTest/BrakeForce/@Units"/>
                    </xsl:call-template>
                </xsl:attribute>
                <xsl:attribute name="Max">
                    <xsl:value-of select="//TestResult/BrakeLeftFrontForceTest/MaxBrakeForce"/>
                </xsl:attribute>
                <xsl:attribute name="Min">
                    <xsl:value-of select="//TestResult/BrakeLeftFrontForceTest/MinBrakeForce"/>
                </xsl:attribute>
                <xsl:value-of select="//TestResult/BrakeLeftFrontForceTest/BrakeForce"/>
            </xsl:element>
            <xsl:element name="TestAttributes"/>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeRightFrontForceTest">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeRightFrontForceTest/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeRightFrontForceTest/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeRightFrontForceTest/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeRightFrontForceTest/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units">
                    <xsl:call-template name="ShowUnits">
                        <xsl:with-param name="UNITS" select="//TestResult/BrakeRightFrontForceTest/BrakeForce/@Units"/>
                    </xsl:call-template>
                </xsl:attribute>
                <xsl:attribute name="Max">
                    <xsl:value-of select="//TestResult/BrakeRightFrontForceTest/MaxBrakeForce"/>
                </xsl:attribute>
                <xsl:attribute name="Min">
                    <xsl:value-of select="//TestResult/BrakeRightFrontForceTest/MinBrakeForce"/>
                </xsl:attribute>
                <xsl:value-of select="//TestResult/BrakeRightFrontForceTest/BrakeForce"/>
            </xsl:element>
            <xsl:element name="TestAttributes"/>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeLeftRearForceTest">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeLeftRearForceTest/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeLeftRearForceTest/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeLeftRearForceTest/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeLeftRearForceTest/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units">
                    <xsl:call-template name="ShowUnits">
                        <xsl:with-param name="UNITS" select="//TestResult/BrakeLeftRearForceTest/BrakeForce/@Units"/>
                    </xsl:call-template>
                </xsl:attribute>
                <xsl:attribute name="Max">
                    <xsl:value-of select="//TestResult/BrakeLeftRearForceTest/MaxBrakeForce"/>
                </xsl:attribute>
                <xsl:attribute name="Min">
                    <xsl:value-of select="//TestResult/BrakeLeftRearForceTest/MinBrakeForce"/>
                </xsl:attribute>
                <xsl:value-of select="//TestResult/BrakeLeftRearForceTest/BrakeForce"/>
            </xsl:element>
            <xsl:element name="TestAttributes"/>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeRightRearForceTest">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeRightRearForceTest/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeRightRearForceTest/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeRightRearForceTest/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeRightRearForceTest/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units">
                    <xsl:call-template name="ShowUnits">
                        <xsl:with-param name="UNITS" select="//TestResult/BrakeRightRearForceTest/BrakeForce/@Units"/>
                    </xsl:call-template>
                </xsl:attribute>
                <xsl:attribute name="Max">
                    <xsl:value-of select="//TestResult/BrakeRightRearForceTest/MaxBrakeForce"/>
                </xsl:attribute>
                <xsl:attribute name="Min">
                    <xsl:value-of select="//TestResult/BrakeRightRearForceTest/MinBrakeForce"/>
                </xsl:attribute>
                <xsl:value-of select="//TestResult/BrakeRightRearForceTest/BrakeForce"/>
            </xsl:element>
            <xsl:element name="TestAttributes"/>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeAnalyzeBalance">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeAnalyzeBalance/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeAnalyzeBalance/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeAnalyzeBalance/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeAnalyzeBalance/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units"/>
                <xsl:attribute name="Max"/>
                <xsl:attribute name="Min"/>
            </xsl:element>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeAnalyzeBalance/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:call-template name="ShowUnits">
                                    <xsl:with-param name="UNITS" select="@Units"/>
                                </xsl:call-template>
                            </xsl:attribute>
                            <xsl:value-of select="."/>
                        </xsl:element>
                    </xsl:element>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeFrontToRearBalanceTest">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeFrontToRearBalanceTest/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeFrontToRearBalanceTest/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeFrontToRearBalanceTest/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeFrontToRearBalanceTest/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units">
                    <xsl:call-template name="ShowUnits">
                        <xsl:with-param name="UNITS" select="//TestResult/BrakeFrontToRearBalanceTest/Value/@Units"/>
                    </xsl:call-template>
                </xsl:attribute>
                <xsl:attribute name="Max">
                    <xsl:value-of select="//TestResult/BrakeFrontToRearBalanceTest/MaxPercentBalance"/>
                </xsl:attribute>
                <xsl:attribute name="Min">
                    <xsl:value-of select="//TestResult/BrakeFrontToRearBalanceTest/MinPercentBalance"/>
                </xsl:attribute>
                <xsl:value-of select="//TestResult/BrakeFrontToRearBalanceTest/Value"/>
            </xsl:element>
            <xsl:element name="TestAttributes"/>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeFrontBalanceTest">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeFrontBalanceTest/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeFrontBalanceTest/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeFrontBalanceTest/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeFrontBalanceTest/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units">
                    <xsl:call-template name="ShowUnits">
                        <xsl:with-param name="UNITS" select="//TestResult/BrakeFrontBalanceTest/Value/@Units"/>
                    </xsl:call-template>
                </xsl:attribute>
                <xsl:attribute name="Max">
                    <xsl:value-of select="//TestResult/BrakeFrontBalanceTest/MaxPercentBalance"/>
                </xsl:attribute>
                <xsl:attribute name="Min">
                    <xsl:value-of select="//TestResult/BrakeFrontBalanceTest/MinPercentBalance"/>
                </xsl:attribute>
                <xsl:value-of select="//TestResult/BrakeFrontBalanceTest/Value"/>
            </xsl:element>
            <xsl:element name="TestAttributes"/>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeRearBalanceTest">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeRearBalanceTest/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeRearBalanceTest/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeRearBalanceTest/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeRearBalanceTest/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units">
                    <xsl:call-template name="ShowUnits">
                        <xsl:with-param name="UNITS" select="//TestResult/BrakeRearBalanceTest/Value/@Units"/>
                    </xsl:call-template>
                </xsl:attribute>
                <xsl:attribute name="Max">
                    <xsl:value-of select="//TestResult/BrakeRearBalanceTest/MaxPercentBalance"/>
                </xsl:attribute>
                <xsl:attribute name="Min">
                    <xsl:value-of select="//TestResult/BrakeRearBalanceTest/MinPercentBalance"/>
                </xsl:attribute>
                <xsl:value-of select="//TestResult/BrakeRearBalanceTest/Value"/>
            </xsl:element>
            <xsl:element name="TestAttributes"/>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeCheckPartNumber">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeCheckPartNumber/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeCheckPartNumber/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeCheckPartNumber/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeCheckPartNumber/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units"/>
                <xsl:attribute name="Max"/>
                <xsl:attribute name="Min"/>
            </xsl:element>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeCheckPartNumber/child::*">
                    <xsl:if test="name()!='Value'">
                        <xsl:element name="Attribute">
                            <xsl:element name="AttributeName">
                                <xsl:value-of select="name()"/>
                            </xsl:element>
                            <xsl:element name="AttributeValue">
                                <xsl:attribute name="Units">
                                    <xsl:call-template name="ShowUnits">
                                        <xsl:with-param name="UNITS" select="@Units"/>
                                    </xsl:call-template>
                                </xsl:attribute>
                                <xsl:value-of select="."/>
                            </xsl:element>
                        </xsl:element>
                    </xsl:if>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeProgramVIN">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeProgramVIN/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeProgramVIN/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeProgramVIN/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeProgramVIN/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units"/>
                <xsl:attribute name="Max"/>
                <xsl:attribute name="Min"/>
            </xsl:element>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeProgramVIN/child::*">
                    <xsl:if test="name()!='Value'">
                        <xsl:element name="Attribute">
                            <xsl:element name="AttributeName">
                                <xsl:value-of select="name()"/>
                            </xsl:element>
                            <xsl:element name="AttributeValue">
                                <xsl:attribute name="Units">
                                    <xsl:call-template name="ShowUnits">
                                        <xsl:with-param name="UNITS" select="@Units"/>
                                    </xsl:call-template>
                                </xsl:attribute>
                                <xsl:value-of select="."/>
                            </xsl:element>
                        </xsl:element>
                    </xsl:if>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeCheckVIN">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeCheckVIN/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeCheckVIN/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeCheckVIN/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeCheckVIN/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units"/>
                <xsl:attribute name="Max"/>
                <xsl:attribute name="Min"/>
            </xsl:element>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeCheckVIN/child::*">
                    <xsl:if test="name()!='Value'">
                        <xsl:element name="Attribute">
                            <xsl:element name="AttributeName">
                                <xsl:value-of select="name()"/>
                            </xsl:element>
                            <xsl:element name="AttributeValue">
                                <xsl:attribute name="Units">
                                    <xsl:call-template name="ShowUnits">
                                        <xsl:with-param name="UNITS" select="@Units"/>
                                    </xsl:call-template>
                                </xsl:attribute>
                                <xsl:value-of select="."/>
                            </xsl:element>
                        </xsl:element>
                    </xsl:if>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeReadFaults">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeReadFaults/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeReadFaults/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeReadFaults/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeReadFaults/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units"/>
                <xsl:attribute name="Max"/>
                <xsl:attribute name="Min"/>
            </xsl:element>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeReadFaults/child::*">
                    <xsl:if test="name()!='Value'">
                        <xsl:element name="Attribute">
                            <xsl:element name="AttributeName">
                                <xsl:value-of select="name()"/>
                            </xsl:element>
                            <xsl:element name="AttributeValue">
                                <xsl:attribute name="Units">
                                    <xsl:call-template name="ShowUnits">
                                        <xsl:with-param name="UNITS" select="@Units"/>
                                    </xsl:call-template>
                                </xsl:attribute>
                                <xsl:value-of select="."/>
                            </xsl:element>
                        </xsl:element>
                    </xsl:if>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeESPValveCross">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:choose>
                <xsl:when test="string-length(//TestResult/BrakeABSValveCrossCheck/@Result) != 0">
                    <xsl:element name="Process">
                        <xsl:value-of select="//TestResult/BrakeABSValveCrossCheck/@Process"/>
                    </xsl:element>
                    <xsl:element name="Result">
                        <xsl:value-of select="//TestResult/BrakeABSValveCrossCheck/@Result"/>
                    </xsl:element>
                    <xsl:element name="ResultDescription">
                        <xsl:value-of select="//TestResult/BrakeABSValveCrossCheck/@Description"/>
                    </xsl:element>
                    <xsl:element name="TestResultCode">
                        <xsl:value-of select="//TestResult/BrakeABSValveCrossCheck/@TestResultCode"/>
                    </xsl:element>
                    <xsl:element name="TestValue">
                        <xsl:attribute name="Units"/>
                        <xsl:attribute name="Max"/>
                        <xsl:attribute name="Min"/>
                    </xsl:element>
                    <xsl:element name="TestAttributes">
                        <xsl:for-each select="//TestResult/BrakeABSValveCrossCheck/child::*">
                            <xsl:element name="Attribute">
                                <xsl:element name="AttributeName">
                                    <xsl:value-of select="name()"/>
                                </xsl:element>
                                <xsl:element name="AttributeValue">
                                    <xsl:attribute name="Units">
                                        <xsl:call-template name="ShowUnits">
                                            <xsl:with-param name="UNITS" select="@Units"/>
                                        </xsl:call-template>
                                    </xsl:attribute>
                                    <xsl:value-of select="."/>
                                </xsl:element>
                            </xsl:element>
                        </xsl:for-each>
                    </xsl:element>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:element name="Process">
                        <xsl:value-of select="//TestResult/BrakeESPValveCross/@Process"/>
                    </xsl:element>
                    <xsl:element name="Result">
                        <xsl:value-of select="//TestResult/BrakeESPValveCross/@Result"/>
                    </xsl:element>
                    <xsl:element name="ResultDescription">
                        <xsl:value-of select="//TestResult/BrakeESPValveCross/@Description"/>
                    </xsl:element>
                    <xsl:element name="TestResultCode">
                        <xsl:value-of select="//TestResult/BrakeESPValveCross/@TestResultCode"/>
                    </xsl:element>
                    <xsl:element name="TestValue">
                        <xsl:attribute name="Units"/>
                        <xsl:attribute name="Max"/>
                        <xsl:attribute name="Min"/>
                    </xsl:element>
                    <xsl:element name="TestAttributes">
                        <xsl:for-each select="//TestResult/BrakeESPValveCross/child::*">
                            <xsl:element name="Attribute">
                                <xsl:element name="AttributeName">
                                    <xsl:value-of select="name()"/>
                                </xsl:element>
                                <xsl:element name="AttributeValue">
                                    <xsl:attribute name="Units">
                                        <xsl:call-template name="ShowUnits">
                                            <xsl:with-param name="UNITS" select="@Units"/>
                                        </xsl:call-template>
                                    </xsl:attribute>
                                    <xsl:value-of select="."/>
                                </xsl:element>
                            </xsl:element>
                        </xsl:for-each>
                    </xsl:element>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeBurnish">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeBrakeBurnishCycle/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:call-template name="BurnishResult">
                    <xsl:with-param name="RESULT" select="//TestResult/BrakeBrakeBurnishCycle/@Result"/>
                </xsl:call-template>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeBrakeBurnishCycle/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeBrakeBurnishCycle/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units"/>
                <xsl:attribute name="Max"/>
                <xsl:attribute name="Min"/>
            </xsl:element>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeBrakeBurnishCycle/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:call-template name="ShowUnits">
                                    <xsl:with-param name="UNITS" select="@Units"/>
                                </xsl:call-template>
                            </xsl:attribute>
                            <xsl:value-of select="."/>
                        </xsl:element>
                    </xsl:element>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <!--
    <xsl:template name="BrakeEPBBurnish">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeElectricParkBrakeBurnishCycle/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:call-template name="BurnishResult">
                    <xsl:with-param name="RESULT" select="//TestResult/BrakeElectricParkBrakeBurnishCycle/@Result"/>
                </xsl:call-template>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeElectricParkBrakeBurnishCycle/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeElectricParkBrakeBurnishCycle/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units"/>
                <xsl:attribute name="Max"/>
                <xsl:attribute name="Min"/>
            </xsl:element>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeElectricParkBrakeBurnishCycle/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:call-template name="ShowUnits">
                                    <xsl:with-param name="UNITS" select="@Units"/>
                                </xsl:call-template>
                            </xsl:attribute>
                            <xsl:value-of select="."/>
                        </xsl:element>
                    </xsl:element>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>
    -->

    <xsl:template name="BrakeSwitchTest">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeBrakeSwitchTest/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeBrakeSwitchTest/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeBrakeSwitchTest/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeBrakeSwitchTest/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units"/>
                <xsl:attribute name="Max"/>
                <xsl:attribute name="Min"/>
            </xsl:element>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeBrakeSwitchTest/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:call-template name="ShowUnits">
                                    <xsl:with-param name="UNITS" select="@Units"/>
                                </xsl:call-template>
                            </xsl:attribute>
                            <xsl:value-of select="."/>
                        </xsl:element>
                    </xsl:element>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeLearnMotorType">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeLearnPerformanceType/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeLearnPerformanceType/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeLearnPerformanceType/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeLearnPerformanceType/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units"/>
                <xsl:attribute name="Max"/>
                <xsl:attribute name="Min"/>
            </xsl:element>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeLearnPerformanceType/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:call-template name="ShowUnits">
                                    <xsl:with-param name="UNITS" select="@Units"/>
                                </xsl:call-template>
                            </xsl:attribute>
                            <xsl:value-of select="."/>
                        </xsl:element>
                    </xsl:element>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeLeftFrontSensor">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeLFSensorTest/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeLFSensorTest/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeLFSensorTest/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeLFSensorTest/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units"/>
                <xsl:attribute name="Max"/>
                <xsl:attribute name="Min"/>
            </xsl:element>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeLFSensorTest/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:call-template name="ShowUnits">
                                    <xsl:with-param name="UNITS" select="@Units"/>
                                </xsl:call-template>
                            </xsl:attribute>
                            <xsl:value-of select="."/>
                        </xsl:element>
                    </xsl:element>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeRightFrontSensor">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeRFSensorTest/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeRFSensorTest/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeRFSensorTest/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeRFSensorTest/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units"/>
                <xsl:attribute name="Max"/>
                <xsl:attribute name="Min"/>
            </xsl:element>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeRFSensorTest/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:call-template name="ShowUnits">
                                    <xsl:with-param name="UNITS" select="@Units"/>
                                </xsl:call-template>
                            </xsl:attribute>
                            <xsl:value-of select="."/>
                        </xsl:element>
                    </xsl:element>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeLeftRearSensor">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeLRSensorTest/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeLFSensorTest/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeLRSensorTest/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeLRSensorTest/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units"/>
                <xsl:attribute name="Max"/>
                <xsl:attribute name="Min"/>
            </xsl:element>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeLRSensorTest/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:call-template name="ShowUnits">
                                    <xsl:with-param name="UNITS" select="@Units"/>
                                </xsl:call-template>
                            </xsl:attribute>
                            <xsl:value-of select="."/>
                        </xsl:element>
                    </xsl:element>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeRightRearSensor">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeRRSensorTest/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeRRSensorTest/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeRRSensorTest/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeRRSensorTest/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units"/>
                <xsl:attribute name="Max"/>
                <xsl:attribute name="Min"/>
            </xsl:element>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeRRSensorTest/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:call-template name="ShowUnits">
                                    <xsl:with-param name="UNITS" select="@Units"/>
                                </xsl:call-template>
                            </xsl:attribute>
                            <xsl:value-of select="."/>
                        </xsl:element>
                    </xsl:element>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeSensorQualityTest">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeSensorQualityTest/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeSensorQualityTest/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeSensorQualityTest/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeSensorQualityTest/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue">
                <xsl:attribute name="Units"/>
                <xsl:attribute name="Max"/>
                <xsl:attribute name="Min"/>
            </xsl:element>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeSensorQualityTest/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:call-template name="ShowUnits">
                                    <xsl:with-param name="UNITS" select="@Units"/>
                                </xsl:call-template>
                            </xsl:attribute>
                            <xsl:value-of select="."/>
                        </xsl:element>
                    </xsl:element>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeDtcs">
        <xsl:element name="DtcCodes">
            <xsl:for-each select="//TestResult/DTC/Brake/child::*">
                <xsl:element name="Dtc">
                    <xsl:element name="DtcCode">0x
                        <xsl:value-of select="./@DTCCode" />
                    </xsl:element>
                    <xsl:element name="DtcDescription">
                        <xsl:value-of select="." />
                    </xsl:element>
                </xsl:element>
            </xsl:for-each>
        </xsl:element>
    </xsl:template>

    <xsl:template name="AirbagDtcs">
        <xsl:element name="DtcCodes">
            <xsl:for-each select="//TestResult/DTC/Airbag/child::*">
                <xsl:element name="Dtc">
                    <xsl:element name="DtcCode">0x
                        <xsl:value-of select="./@DTCCode" />
                    </xsl:element>
                    <xsl:element name="DtcDescription">
                        <xsl:value-of select="." />
                    </xsl:element>
                </xsl:element>
            </xsl:for-each>
        </xsl:element>
    </xsl:template>

    <xsl:template name="ChooseMachine">
        <xsl:param name="VALUE"/>
        <xsl:choose>
            <xsl:when test="$VALUE = '508180'">
                <xsl:element name="Machine">
                    <xsl:text>Lane 1</xsl:text>
                </xsl:element>
            </xsl:when>
            <xsl:otherwise>
                <xsl:choose>
                    <xsl:when test="$VALUE = '508202'">
                        <xsl:element name="Machine">
                            <xsl:text>Lane 2</xsl:text>
                        </xsl:element>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:element name="Machine">
                            <xsl:text>Lane 3</xsl:text>
                        </xsl:element>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <xsl:template name="BurnishResult">
        <xsl:param name="RESULT"/>
        <xsl:choose>
            <xsl:when test="$RESULT != ''">
                <xsl:value-of select="$RESULT"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>Not Run</xsl:text>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <xsl:template name="ShowUnits">
        <xsl:param name="UNITS"/>
        <xsl:choose>
            <xsl:when test="translate($UNITS,'PERCENT','percent')='percent'">
                <xsl:text>%</xsl:text>
            </xsl:when>
            <xsl:when test="translate($UNITS,'LBF','lbf')='lbf'">
                <xsl:text>lbf</xsl:text>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="$UNITS"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>


</xsl:stylesheet>
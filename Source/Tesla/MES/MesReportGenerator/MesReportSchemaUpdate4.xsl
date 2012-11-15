<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="2.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xalan="http://xml.apache.org/xalan">
    <xsl:output method="xml" version="1.0" indent="yes" xalan:indent-amount="2"/>
    <xsl:strip-space elements="*"/>

    <xsl:template match="/BepVehicleTestResult">
        <xsl:element name="QualityTest">
            <xsl:element name="QualityTestNumber">
                <xsl:value-of select="//VehicleBuild/VIN"/>
            </xsl:element>
            <xsl:call-template name="BrakeModule"/>
            <xsl:call-template name="AirbagModule"/>
        </xsl:element>
    </xsl:template>




    <xsl:template name="BrakeModule" match="//TestResult">
        <xsl:element name="Module">
            <xsl:call-template name="BrakeModuleInfo">
                <xsl:with-param name="NAME">BrakeControlModule</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="ValveFiringTest">
                <xsl:with-param name="NAME">BrakeValveFiring</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeDumpAndIsolateAllWheels">
                <xsl:with-param name="NAME">BrakeDumpAndIsolateAllWheels</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeRecoverAllWheels">
                <xsl:with-param name="NAME">BrakeRecoverAllWheels</xsl:with-param>
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
            <xsl:call-template name="BrakeLeftFrontRecovery">
                <xsl:with-param name="NAME">BrakeLeftFrontRecovery</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeRightFrontRecovery">
                <xsl:with-param name="NAME">BrakeRightFrontRecovery</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeLeftRearRecovery">
                <xsl:with-param name="NAME">BrakeLeftRearRecovery</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="BrakeRightRearRecovery">
                <xsl:with-param name="NAME">BrakeRightRearRecovery</xsl:with-param>
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
            <xsl:element name="TestValue"/>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/AirbagCheckVIN/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:value-of select="@Units"/>
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
            <xsl:element name="TestValue"/>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/AirbagProgramVIN/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:value-of select="@Units"/>
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
            <xsl:element name="TestValue"/>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/AirbagEnableAirBagModule/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:value-of select="@Units"/>
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
            <xsl:element name="TestValue"/>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/AirbagReadFaults/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:value-of select="@Units"/>
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
            <xsl:element name="TestValue"/>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeValveFiringTest/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:value-of select="@Units"/>
                            </xsl:attribute>
                            <xsl:value-of select="."/>
                        </xsl:element>
                    </xsl:element>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeDumpAndIsolateAllWheels">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeDumpAndIsolateAllWheels/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeDumpAndIsolateAllWheels/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeDumpAndIsolateAllWheels/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeDumpAndIsolateAllWheels/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue"/>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeDumpAndIsolateAllWheels/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:value-of select="@Units"/>
                            </xsl:attribute>
                            <xsl:value-of select="."/>
                        </xsl:element>
                    </xsl:element>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeRecoverAllWheels">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeRecoverAllWheels/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeRecoverAllWheels/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeRecoverAllWheels/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeRecoverAllWheels/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue"/>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeRecoverAllWheels/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:value-of select="@Units"/>
                            </xsl:attribute>
                            <xsl:value-of select="."/>
                        </xsl:element>
                    </xsl:element>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeLeftFrontReduction">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeLeftFrontReduction/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeLeftFrontReduction/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeLeftFrontReduction/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeLeftFrontReduction/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue"/>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeLeftFrontReduction/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:value-of select="@Units"/>
                            </xsl:attribute>
                            <xsl:value-of select="."/>
                        </xsl:element>
                    </xsl:element>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeRightFrontReduction">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeRightFrontReduction/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeRightFrontReduction/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeRightFrontReduction/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeRightFrontReduction/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue"/>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeRightFrontReduction/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:value-of select="@Units"/>
                            </xsl:attribute>
                            <xsl:value-of select="."/>
                        </xsl:element>
                    </xsl:element>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeLeftRearReduction">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeLeftRearReduction/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeLeftRearReduction/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeLeftRearReduction/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeLeftRearReduction/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue"/>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeLeftRearReduction/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:value-of select="@Units"/>
                            </xsl:attribute>
                            <xsl:value-of select="."/>
                        </xsl:element>
                    </xsl:element>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeRightRearReduction">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeRightRearReduction/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeRightRearReduction/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeRightRearReduction/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeRightRearReduction/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue"/>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeRightRearReduction/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:value-of select="@Units"/>
                            </xsl:attribute>
                            <xsl:value-of select="."/>
                        </xsl:element>
                    </xsl:element>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeLeftFrontRecovery">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeLeftFrontRecovery/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeLeftFrontRecovery/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeLeftFrontRecovery/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeLeftFrontRecovery/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue"/>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeLeftFrontRecovery/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:value-of select="@Units"/>
                            </xsl:attribute>
                            <xsl:value-of select="."/>
                        </xsl:element>
                    </xsl:element>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeRightFrontRecovery">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeRightFrontRecovery/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeRightFrontRecovery/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeRightFrontRecovery/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeRightFrontRecovery/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue"/>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeRightFrontRecovery/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:value-of select="@Units"/>
                            </xsl:attribute>
                            <xsl:value-of select="."/>
                        </xsl:element>
                    </xsl:element>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeLeftRearRecovery">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeLeftRearRecovery/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeLeftRearRecovery/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeLeftRearRecovery/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeLeftRearRecovery/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue"/>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeLeftRearRecovery/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:value-of select="@Units"/>
                            </xsl:attribute>
                            <xsl:value-of select="."/>
                        </xsl:element>
                    </xsl:element>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <xsl:template name="BrakeRightRearRecovery">
        <xsl:param name="NAME"/>
        <xsl:element name="ModuleTest">
            <xsl:element name="TestName">
                <xsl:value-of select="$NAME"/>
            </xsl:element>
            <xsl:element name="Process">
                <xsl:value-of select="//TestResult/BrakeRightRearRecovery/@Process"/>
            </xsl:element>
            <xsl:element name="Result">
                <xsl:value-of select="//TestResult/BrakeRightRearRecovery/@Result"/>
            </xsl:element>
            <xsl:element name="ResultDescription">
                <xsl:value-of select="//TestResult/BrakeRightRearRecovery/@Description"/>
            </xsl:element>
            <xsl:element name="TestResultCode">
                <xsl:value-of select="//TestResult/BrakeRightRearRecovery/@TestResultCode"/>
            </xsl:element>
            <xsl:element name="TestValue"/>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeRightRearRecovery/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:value-of select="@Units"/>
                            </xsl:attribute>
                            <xsl:value-of select="."/>
                        </xsl:element>
                    </xsl:element>
                </xsl:for-each>
            </xsl:element>
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
            <xsl:element name="TestValue"/>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeAnalyzeBalance/child::*">
                    <xsl:element name="Attribute">
                        <xsl:element name="AttributeName">
                            <xsl:value-of select="name()"/>
                        </xsl:element>
                        <xsl:element name="AttributeValue">
                            <xsl:attribute name="Units">
                                <xsl:value-of select="@Units"/>
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
                    <xsl:value-of select="//TestResult/BrakeFrontToRearBalanceTest/Value/@Units"/>
                </xsl:attribute>
                <xsl:value-of select="//TestResult/BrakeFrontToRearBalanceTest/Value"/>
            </xsl:element>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeFrontToRearBalanceTest/child::*">
                    <xsl:if test="name()!='Value'">
                        <xsl:element name="Attribute">
                            <xsl:element name="AttributeName">
                                <xsl:value-of select="name()"/>
                            </xsl:element>
                            <xsl:element name="AttributeValue">
                                <xsl:attribute name="Units">
                                    <xsl:value-of select="@Units"/>
                                </xsl:attribute>
                                <xsl:value-of select="."/>
                            </xsl:element>
                        </xsl:element>
                    </xsl:if>
                </xsl:for-each>
            </xsl:element>
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
                    <xsl:value-of select="//TestResult/BrakeFrontBalanceTest/Value/@Units"/>
                </xsl:attribute>
                <xsl:value-of select="//TestResult/BrakeFrontBalanceTest/Value"/>
            </xsl:element>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeFrontBalanceTest/child::*">
                    <xsl:if test="name()!='Value'">
                        <xsl:element name="Attribute">
                            <xsl:element name="AttributeName">
                                <xsl:value-of select="name()"/>
                            </xsl:element>
                            <xsl:element name="AttributeValue">
                                <xsl:attribute name="Units">
                                    <xsl:value-of select="@Units"/>
                                </xsl:attribute>
                                <xsl:value-of select="."/>
                            </xsl:element>
                        </xsl:element>
                    </xsl:if>
                </xsl:for-each>
            </xsl:element>
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
                    <xsl:value-of select="//TestResult/BrakeRearBalanceTest/Value/@Units"/>
                </xsl:attribute>
                <xsl:value-of select="//TestResult/BrakeRearBalanceTest/Value"/>
            </xsl:element>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeRearBalanceTest/child::*">
                    <xsl:if test="name()!='Value'">
                        <xsl:element name="Attribute">
                            <xsl:element name="AttributeName">
                                <xsl:value-of select="name()"/>
                            </xsl:element>
                            <xsl:element name="AttributeValue">
                                <xsl:attribute name="Units">
                                    <xsl:value-of select="@Units"/>
                                </xsl:attribute>
                                <xsl:value-of select="."/>
                            </xsl:element>
                        </xsl:element>
                    </xsl:if>
                </xsl:for-each>
            </xsl:element>
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
            <xsl:element name="TestValue"/>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeCheckPartNumber/child::*">
                    <xsl:if test="name()!='Value'">
                        <xsl:element name="Attribute">
                            <xsl:element name="AttributeName">
                                <xsl:value-of select="name()"/>
                            </xsl:element>
                            <xsl:element name="AttributeValue">
                                <xsl:attribute name="Units">
                                    <xsl:value-of select="@Units"/>
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
            <xsl:element name="TestValue"/>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeProgramVIN/child::*">
                    <xsl:if test="name()!='Value'">
                        <xsl:element name="Attribute">
                            <xsl:element name="AttributeName">
                                <xsl:value-of select="name()"/>
                            </xsl:element>
                            <xsl:element name="AttributeValue">
                                <xsl:attribute name="Units">
                                    <xsl:value-of select="@Units"/>
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
            <xsl:element name="TestValue"/>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeCheckVIN/child::*">
                    <xsl:if test="name()!='Value'">
                        <xsl:element name="Attribute">
                            <xsl:element name="AttributeName">
                                <xsl:value-of select="name()"/>
                            </xsl:element>
                            <xsl:element name="AttributeValue">
                                <xsl:attribute name="Units">
                                    <xsl:value-of select="@Units"/>
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
            <xsl:element name="TestValue"/>
            <xsl:element name="TestAttributes">
                <xsl:for-each select="//TestResult/BrakeReadFaults/child::*">
                    <xsl:if test="name()!='Value'">
                        <xsl:element name="Attribute">
                            <xsl:element name="AttributeName">
                                <xsl:value-of select="name()"/>
                            </xsl:element>
                            <xsl:element name="AttributeValue">
                                <xsl:attribute name="Units">
                                    <xsl:value-of select="@Units"/>
                                </xsl:attribute>
                                <xsl:value-of select="."/>
                            </xsl:element>
                        </xsl:element>
                    </xsl:if>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>


</xsl:stylesheet>
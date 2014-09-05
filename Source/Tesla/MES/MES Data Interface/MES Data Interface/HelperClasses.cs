using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Text;
using System.Threading.Tasks;

namespace MES_Data_Interface
{
    //Delegates
    public delegate void LogHandler(string text);


    /// <summary>
    /// Used by Tesla MES GetOptionCodesByVIN(string VIN) method
    /// Called OptionCodesByVinMessage in documentation
    /// </summary>
    public class OptionCodesMessage
    {
        public string Vin { get; set; }
        public string Success { get; set; }
        public string Error { get; set; }
        public List<OptionCodeRow> OptionCodes { get; set; }
    }

    /// <summary>
    /// Used by Tesla MES GetOptionCodesByVIN(string VIN) method
    /// </summary>
    public class OptionCodeRow
    {
        public string Description { get; set; }
        public string Group { get; set; }
        public string OptionCode { get; set; }
    }

    /// <summary>
    /// Used by Tesla MES UpdateDisposition(DispositionMessage Message) method
    /// </summary>
    public class DispositionMessage
    {
        public string Vin { get; set; }
        public string ExternalSystem { get; set; }
        public string Success { get; set; }
        public string Error { get; set; }
        public List<DispositionLine> PitchList { get; set; }

    }

    /// <summary>
    /// Used by Tesla MES UpdateDisposition(DispositionMessage Message) method
    /// </summary>
    public class DispositionLine
    {
        public string PitchName { get; set; }
        public List<DispositionTest> CheckItems { get; set; }
    }

    /// <summary>
    /// Used by Tesla MES UpdateDisposition(DispositionMessage Message) method
    /// </summary>
    public class DispositionTest
    {
        public string Code { get; set; }
        public string Message { get; set; }
        public decimal Status { get; set; }
        public decimal? Value { get; set; }
        public string Attribute { get; set; }
        public string UOM { get; set; }
        public decimal? MaxValue { get; set; }
        public decimal? MinValue { get; set; }
        public DateTime? StartTime { get; set; }
        public DateTime? EndTime { get; set; }
        public decimal? TargetValue { get; set; }
        public string AttributeTarget { get; set; }
    }

    public class TeslaDefines
    {
        public static readonly string VIN = "VIN";
        public static readonly string LogFileDir = "LogFileDir";
        public static readonly string DefaultLogDir = Path.Combine(Application.StartupPath, "Logs");
        public static readonly string VinLength = "VinLength";
        public static readonly string VehCode_Prefix = "VehCode_Prefix";
        public static readonly string HlaCode_Prefix = "HlaCode_Prefix";
        public static readonly string RbCode_Prefix = "RbCode_Prefix";
        public static readonly string HostUrl = "HostUrl";
        public static readonly string TimeoutInMilliseconds = "TimeoutInMilliseconds";
    }

    public enum ApplicationStatus : int
    {
        ExternalAppAlive = 10,
        KillExternalApp = 20,
        KillingExternalApp = 30,
        ExternalAppDead = 40,
    }

    public enum GetBuildDataSignal : int
    {
        CLEAR_GET_BUILD = 0,
        AUTO_GET_BUILD = 1,
        MANUAL_GET_BUILD = 2,
        ERR_VIN_NOT_FOUND = -1,
        ERR_INVALID_VIN = -2,
        ERR_TYPE_MATCH_NOT_FOUND = -3
    };
}

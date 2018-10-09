using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Common.Classes
{
    public class ECUBuildData
    {
        
        public ECUBuildData()
        {
            m_softwareModulePartNumbers = new List<string>();
            m_requestID = new List<byte>();
            m_responseID = new List<byte>();
            m_programmingSuccess = false;
            m_performFlash = true;
            m_vinWriteSuccess = true;
            m_serialNumberWriteSuccess = true;
            m_esnLeadingChars = "";
            m_esnWriteRequired = false;
            //Affects Plant 12 ECM and Mimamori
            m_rearAxel = "";
            m_transmission = "";
            m_tireSize = "";
            m_speedMeter = "";

        }
        public ECUBuildData(string ecuName, string vin, string partNo, string utilFilePartNo, List<String> swmiPartNos, string esnLeadingChars, bool esnWriteRequired)
        {
            m_ecuName = ecuName;
            m_vin = vin;
            m_partNo = partNo;
            m_utilityFilePartNumber = utilFilePartNo;
            m_softwareModulePartNumbers = swmiPartNos;
            m_requestID = new List<byte>();
            m_responseID = new List<byte>();
            m_programmingSuccess = false;
            m_performFlash = true;
            m_vinWriteSuccess = true;
            m_serialNumberWriteSuccess = true;
            m_partNoMatches = false;
            m_esn = "";
            m_esnLeadingChars = esnLeadingChars;
            m_esnWriteRequired = esnWriteRequired;
            //Affects Plant 12 ECM and Mimamori
            m_rearAxel = "";
            m_transmission = "";
            m_tireSize = "";
            m_speedMeter = "";
        }
        private String m_ecuName;
        private String m_vin;
        private String m_partNo;
        private String m_utilityFilePartNumber;
        private List<String> m_softwareModulePartNumbers;
        private List<byte> m_requestID;
        private List<byte> m_responseID;
        private bool m_programmingSuccess;
        private bool m_performFlash;
        private bool m_vinWriteSuccess;
        private bool m_serialNumberWriteSuccess;
        private bool m_partNoMatches;
        private String m_esn;
        private String m_esnLeadingChars;
        private bool m_esnWriteRequired;
        //Added to BOM for Flashing Mimamori and ECM
        private String m_rearAxel;
        private String m_transmission;
        private String m_tireSize;
        private String m_speedMeter;

        public String ECUName
        {
            get { return m_ecuName; }
            set { m_ecuName = value; }
        }
        public String VIN
        {
            get { return m_vin; }
            set { m_vin = value; }
        }

        public String PartNumber
        {
            get { return m_partNo; }
            set { m_partNo = value; }
        }

        public bool PartNumberMatches
        {
            get { return m_partNoMatches; }
            set { m_partNoMatches = value; }
        }

        public String UtilityFilePartNumber
        {
            get { return m_utilityFilePartNumber; }
            set { m_utilityFilePartNumber = value; }
        }

        public List<String> SoftwareModulePartNumbers
        {
            get { return m_softwareModulePartNumbers; }
            set { m_softwareModulePartNumbers = value; }
        }

        public List<byte> RequestID
        {
            get { return m_requestID; }
            set { m_responseID = value; }
        }

        public List<byte> ResponseID
        {
            get { return m_responseID; }
            set { m_responseID = value; }
        }
        public bool ProgrammingSuccess
        {
            get { return m_programmingSuccess; }
            set { m_programmingSuccess = value; }
        }
        public bool PerformFlash
        {
            get { return m_performFlash; }
            set { m_performFlash = value; }
        }
        public bool VinWriteSuccess
        {
            get { return m_vinWriteSuccess; }
            set { m_vinWriteSuccess = value; }
        }
        public bool SerialNumberWriteSuccess
        {
            get { return m_serialNumberWriteSuccess; }
            set { m_serialNumberWriteSuccess = value; }
        }
        public String EngineSerialNumber
        {
            get { return m_esn; }
            set { m_esn = value; }
        }
        public String ESNLeadingCharacters
        {
            get { return m_esnLeadingChars; }
            set { m_esnLeadingChars = value; }
        }
        public bool ESNWriteRequired
        {
            get { return m_esnWriteRequired; }
            set { m_esnWriteRequired = value; }
        }

        //Added RearAxel, Transmission, Tiresize and SpeedMeter so that the ECU  and Mimamori modules can be flashed with different values for different builds. 
        public String RearAxel
        {
            get { return m_rearAxel; }
            set { m_rearAxel = value; }
        }

        public String Transmission
        {
            get { return m_transmission; }
            set { m_transmission = value; }
        }

        public String TireSize
        {
            get { return m_tireSize; }
            set { m_tireSize = value; }
        }

        public String SpeedMeter
        {
            get { return m_speedMeter; }
            set { m_speedMeter = value; }
        }
    }
}

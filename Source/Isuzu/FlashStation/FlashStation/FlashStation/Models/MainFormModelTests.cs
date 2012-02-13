using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Common.Lib.Models
{
    public class MainFormModelTests : MainFormModel
    {
        public MainFormModelTests()
        {
            TestGetESN();
        }
        public string TestGetESN()
        {
            string result = "fail";
            if (GetESN("12345678901234567", "C:\\FlashStation\\TestESN\\") == "1234567890123456")
            {
                result = "pass";
            }
            else
            {
                result = "fail";
            }
            m_logger.Log("INFO:  Result: " + result);
            return result;
        }
    }
}

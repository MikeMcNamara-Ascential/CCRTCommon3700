using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace MaintenanceReminder
{
    public static class IntervalUnits
    {
        //=========================================================================================
        // Member Functions
        //=========================================================================================

        /// <summary>
        /// Get the list of units in the local display language.
        /// </summary>
        /// <returns>List of units in the current display language.</returns>
        public static List<String> GetDisplayUnits(Localizer displayLocalizer)
        {
            List<String> displayUnits = new List<String>();
            foreach (String unit in Units)
            {
                displayUnits.Add(displayLocalizer.GetLocalizedString(unit));
            }
            return displayUnits;
        }

        /// <summary>
        /// Get the specified unit in the current display language.
        /// </summary>
        /// <param name="unitIndex">Index of the unit to retrieve.</param>
        /// <returns>Requested unit in the current display language.</returns>
        public static String GetDisplayUnits(Localizer displayLocalizer, Int32 unitIndex = 0)
        {
            return displayLocalizer.GetLocalizedString(Units[unitIndex]);
        }

        /// <summary>
        /// Get the specified interval unit in English.
        /// </summary>
        /// <param name="unitIndex">Index of the unit to retrieve.</param>
        /// <returns>Requested unit in English.</returns>
        public static String GetUnits(Int32 unitIndex = 0)
        {
            return Units[unitIndex];
        }


        

        //=========================================================================================
        // Member Variables and Properties
        //=========================================================================================

        /// <summary>
        /// List of English units to use.
        /// </summary>
        private static String[] Units = { "Hours", "Days", "Weeks", "Months", "Years"};
    }
}

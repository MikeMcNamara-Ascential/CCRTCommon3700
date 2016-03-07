using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace MaintenanceReminderLibrary
{
    public class ReminderSettings
    {
        //=========================================================================================
        // Member Variables and Properties
        //=========================================================================================

        /// <summary>
        /// Get/Set the current elapsed time.
        /// </summary>
        public Double CurrentElapsedTime { get; set; }

        /// <summary>
        /// Get/Set the message to display when the maintenance interval expires.
        /// </summary>
        public String FaultMessage { get; set; }

        /// <summary>
        /// Get/Set the currently logged maintenance notes for this item.
        /// </summary>
        public String MaintenanceNotes { get; set; }

        /// <summary>
        /// Get/Set the name of the maintenance item.
        /// </summary>
        public String Name { get; set; }

        /// <summary>
        /// Get/Set the maintenance interval
        /// </summary>
        public Int32 Interval { get; set; }

        /// <summary>
        /// Get/Set the units of the maintenance interval
        /// </summary>
        public String IntervalUnits { get; set; }

        /// <summary>
        /// Get/Set the start date and time of the current interval.
        /// </summary>
        public DateTime IntervalStartDate { get; set; }

        /// <summary>
        /// Get/Set the name of the document containing the work instructions for this item.
        /// </summary>
        public String WorkInstructionDocument { get; set; }
    }
}

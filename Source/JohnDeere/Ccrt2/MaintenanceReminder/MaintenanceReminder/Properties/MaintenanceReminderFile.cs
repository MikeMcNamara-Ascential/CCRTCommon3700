using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml;
using Logger;
using MaintenanceReminderLibrary;

namespace MaintenanceReminder
{
    public static class MaintenanceReminderFile
    {
        //=========================================================================================
        // Member Functions
        //=========================================================================================

        /// <summary>
        /// Delete the specified reminders from the list of maintenance items.
        /// </summary>
        /// <param name="reminderstoDelete">List of maintenance items to remove.</param>
        /// <param name="reminderList">List of maintenance items.</param>
        /// <param name="logger">Logger object to log activities.</param>
        public static void DeleteReminder(List<String> reminderstoDelete,
                                          List<MaintenanceReminderItem> reminderList,
                                          CcrtLogger logger)
        {
            // Find the reminder so we can remove it from the list
            XmlDocument reminderDoc = new XmlDocument();
            reminderDoc.Load(MaintenanceReminder.Properties.Settings.Default.MaintenanceSettingsFile);
            foreach (object item in reminderstoDelete)
            {   // First remove the item from the XML

                XmlNode reminderNode = reminderDoc.DocumentElement.SelectSingleNode(item.ToString());
                if (reminderNode != null)
                {
                    reminderDoc.DocumentElement.RemoveChild(reminderNode);
                }
                MaintenanceReminderItem deleteItem = reminderList.Find(delegate(MaintenanceReminderItem reminderItem)
                {
                    return reminderItem.ItemSettings.Name == item.ToString();
                }
                                                                    );
                if (deleteItem != null)
                {
                    logger.Log("Removing reminder for " + deleteItem.ItemSettings.Name);
                    reminderList.Remove(deleteItem);
                }
            }
            // Save the list back to file
            reminderDoc.Save(MaintenanceReminder.Properties.Settings.Default.MaintenanceSettingsFile);
        }

        /// <summary>
        /// Save the reminders to file.
        /// </summary>
        /// <param name="logger">Logger object to use for logging activities.</param>
        /// <param name="reminderList">List of reminders to store in the file.</param>
        public static void SaveReminders(List<MaintenanceReminderItem> reminderList, CcrtLogger logger)
        {   // Create a new XML Document to hold the reminder info
            try
            {
                XmlDocument reminderDoc = new XmlDocument();
                reminderDoc.Load(MaintenanceReminder.Properties.Settings.Default.MaintenanceSettingsFile);
                foreach (MaintenanceReminderItem reminder in reminderList)
                {   // Check if a node already exists for this item
                    XmlNode reminderNode = reminderDoc.DocumentElement.SelectSingleNode(reminder.ItemSettings.Name);
                    if (reminderNode == null)
                    {   // Create a new node to add to the list
                        XmlElement newItem = reminderDoc.CreateElement(reminder.ItemSettings.Name);
                        reminderDoc.DocumentElement.AppendChild(newItem);
                        reminderNode = reminderDoc.DocumentElement.SelectSingleNode(reminder.ItemSettings.Name);
                        XmlAttribute a_faultText = reminderDoc.CreateAttribute("FaultText");
                        XmlAttribute a_faultLevel = reminderDoc.CreateAttribute("FaultLevel");
                        XmlAttribute a_interval = reminderDoc.CreateAttribute("MaintenanceInterval");
                        XmlAttribute a_intervalUnits = reminderDoc.CreateAttribute("IntervalUnits");
                        XmlAttribute a_intervalStartDate = reminderDoc.CreateAttribute("IntervalStartDate");
                        XmlAttribute a_workInstruction = reminderDoc.CreateAttribute("WorkInstructionDocument");
                        // Assign the elements to the correct places
                        reminderNode.Attributes.Append(a_faultText);
                        reminderNode.Attributes.Append(a_faultLevel);
                        reminderNode.Attributes.Append(a_interval);
                        reminderNode.Attributes.Append(a_intervalUnits);
                        reminderNode.Attributes.Append(a_intervalStartDate);
                        reminderNode.Attributes.Append(a_workInstruction);
                        reminderDoc.DocumentElement.AppendChild(reminderNode);
                    }
                    // Assign the data values to the elements
                    reminderNode.Attributes.GetNamedItem("FaultText").Value = reminder.ItemSettings.FaultMessage;
                    reminderNode.Attributes.GetNamedItem("FaultLevel").Value = "1";
                    reminderNode.Attributes.GetNamedItem("MaintenanceInterval").Value = reminder.ItemSettings.Interval.ToString();
                    reminderNode.Attributes.GetNamedItem("IntervalUnits").Value = reminder.ItemSettings.IntervalUnits;
                    Boolean nodeComplete = false;
                    Int16 nodeAttempts = 2;
                    do
                    {
                        try
                        {
                            reminderNode.Attributes.GetNamedItem("IntervalStartDate").Value = reminder.ItemSettings.IntervalStartDate.ToString();
                            nodeComplete = true;
                        }
                        catch (NullReferenceException)
                        {
                            XmlAttribute a_intervalStartDate = reminderDoc.CreateAttribute("IntervalStartDate");
                            reminderNode.Attributes.Append(a_intervalStartDate);
                        }
                    } while (!nodeComplete && (nodeAttempts-- > 0));
                    reminderNode.Attributes.GetNamedItem("WorkInstructionDocument").Value = reminder.ItemSettings.WorkInstructionDocument;
                    // Remove the existing Notes and replace with the new notes
                    XmlNode orgNotesNode = reminderNode.SelectSingleNode("Notes");
                    if (orgNotesNode != null)
                    {
                        reminderNode.RemoveChild(orgNotesNode);
                    }
                    XmlElement notesNode = reminderDoc.CreateElement("Notes");
                    XmlText notes = reminderDoc.CreateTextNode(reminder.ItemSettings.MaintenanceNotes);
                    notesNode.AppendChild(notes);
                    reminderNode.AppendChild(notesNode);
                }
                // Save the updated list to file
                reminderDoc.Save(MaintenanceReminder.Properties.Settings.Default.MaintenanceSettingsFile);
            }
            catch (IOException excpt)
            {
                logger.Log("IOException opening maintenance file during UpdateElapsedTime() - " + excpt.Message);
                logger.Log("- Exception Details: " + Environment.NewLine + excpt.StackTrace);
            }
            catch (Exception excpt)
            {
                logger.Log("Exception opening maintenance file during UpdateElapsedTime() - " + excpt.Message);
                logger.Log("- Exception Details: " + Environment.NewLine + excpt.StackTrace);
            }
        }

    }
}

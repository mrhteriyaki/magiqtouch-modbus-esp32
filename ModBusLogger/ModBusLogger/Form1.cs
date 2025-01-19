using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Globalization;
using System.Net.Configuration;

namespace ModBusLogger
{
    public partial class Form1 : Form
    {
        Thread SerialThread;
        Thread SerialThread2;
        SerialPort serialAPort = new SerialPort();
        SerialPort serialBPort = new SerialPort();


        static string OModeString;
        static bool shutdown = false;
        Thread MessageDisplayThread = new Thread(MessageDisplayProcess);
        static Queue<ModBusMessage> MessageDisplayQueue = new Queue<ModBusMessage>();
        static List<ModBusMessage> DisplayList = new List<ModBusMessage>();

        static List<ModBusMessage> UniMessages = new List<ModBusMessage>();
        static string MonitorMessage = String.Empty;
        static object monitormessagelock = new object(); //thread lock for message string.

        static int BytesToMatch = 2;
        static bool BytesLengthmatch = true;

        static bool ExcludeCRC = false;
        static bool ExcludeTime = false;
        static string DisplayFilter;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            chkHexMode.Checked = true;
            chkExcludeCRC.Checked = true;
            chkTimestamp.Checked = true;
            MessageDisplayThread.Start();
        }

        static void MessageDisplayProcess()
        {
            //Sort messages by SlaveID, Function and specified number of data bytes.
            while (shutdown == false) //Keep running until program shuts down.
            {
                while (MessageDisplayQueue.Count > 0)
                {
                    ModBusMessage newmsg = MessageDisplayQueue.Dequeue();

                    //Check if newmsg matches any in DisplayList.
                    bool nomatch = true;
                    for (int k = 0; k < DisplayList.Count; k++) //Loop DisplayList.
                    {
                        if (DisplayList[k].SlaveID != newmsg.SlaveID)
                        {
                            continue; //Skip if Slave ID not matched.
                        }
                        if (DisplayList[k].FunctionCode != newmsg.FunctionCode)
                        {
                            continue; //Skip if Function Code not matched.
                        }

                        //Match databytes.
                        bool databytesmatch = true;
                        if (BytesToMatch > 0) //Only check if byte match count is above 0.
                        {
                            for (int i = 0; i < BytesToMatch; i++) //check number of starting bytes to match.
                            {
                                if (DisplayList[k].Data[i] != newmsg.Data[i]) //Check if bytes don't match up to specified count.
                                {
                                    databytesmatch = false; //Data bytes failed to match, mark new message.
                                    break; //exit byte loop.
                                }
                            }
                        }

                        //If length match enabled, mark as mismatch if number of data bytes does not match.
                        if (newmsg.Data.Count != DisplayList[k].Data.Count && BytesLengthmatch)
                        {
                            databytesmatch = false;
                        }

                        if (databytesmatch)
                        {
                            //Existing message - update array with new data.
                            nomatch = false;

                            //Update existing displaylist entry.
                            DisplayList[k].Timestamp = newmsg.Timestamp; //Update timestamp to show last change.
                            DisplayList[k].Data = newmsg.Data; // Replace display entry with new data.
                            //future - add checks here to add visual alert for changes to data.

                            //Break loop, match found with existing entry.
                            break;
                        }
                    }

                    //Add to displaylist as new entry.
                    if (nomatch)
                    {
                        DisplayList.Add(newmsg);
                    }


                    //update gui string
                    string tmpMonitorMessage = String.Empty;
                    foreach (ModBusMessage msg in DisplayList.OrderBy(ModBusMessage => ModBusMessage.SlaveID).ThenBy(ModBusMessage => ModBusMessage.FunctionCode).ToList()) //Sort list by SlaveID.
                    {
                        string LineData = msg.SlaveID.ToString(OModeString) + " " + msg.FunctionCode.ToString(OModeString) + " " + msg.GetDataString(ExcludeCRC);
                        bool addline = false;
                        if (String.IsNullOrEmpty(DisplayFilter) || LineData.ToLower().StartsWith(DisplayFilter.ToLower()))
                        {
                            addline = true;
                        }
                        string timestamp = ""; 
                        if (!ExcludeTime)
                        {
                            timestamp = msg.Timestamp.ToString("HH:mm:ss.ffff") + " ";
                        }
                        string snamestring = "";
                        if (!string.IsNullOrEmpty(msg.serialport))
                        {
                            snamestring = msg.serialport + " ";
                        }
                        if (addline)
                        {
                            tmpMonitorMessage = tmpMonitorMessage + timestamp + snamestring + LineData + Environment.NewLine;
                        }
                    }

                    lock (monitormessagelock)
                    {
                        MonitorMessage = tmpMonitorMessage;
                    }
                }
                //Sleep while no messages in queue.
                Thread.Sleep(5);


            }
        }



        static bool stop_monitor = false;
        static void MonitoringThread(SerialPort serialPort, SerialPort relayPort, bool bytemode)
        {
            serialPort.BaudRate = 9600;   // Change this to your actual baud rate
            serialPort.Parity = Parity.None;
            serialPort.DataBits = 8;
            serialPort.StopBits = StopBits.One;
            serialPort.ReadTimeout = 1000;



            long lastbyte_time = DateTimeOffset.Now.ToUnixTimeMilliseconds();
            int seperation_interval = 50; //max ms time gap between packets.
            int packet_byte_number = 0; //byte number in packet.
            ModBusMessage curMsg = new ModBusMessage(); //tmp object for modbus message.
            curMsg.Timestamp = DateTime.Now;
            UniMessages.Clear();

            //start new log file.
            StreamWriter SW = new StreamWriter("modbus_log_" + serialPort.PortName + ".csv");
            StreamWriter SWError = new StreamWriter("modbus_crc_failures_" + serialPort.PortName + ".csv");

            try
            {
                // Open the serial port
                serialPort.Open();
                // Subscribe to the DataReceived event to handle incoming data
                //serialPort.DataReceived += SerialPort_DataReceived;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error: {ex.Message}");
                return;
            }

            while (stop_monitor == false && shutdown == false)
            {
                try
                {

                    //String mode - process string message, skip CRC validation.
                    if (!bytemode)
                    {

                        try
                        {
                            string linedata = serialPort.ReadLine();
                            curMsg = new ModBusMessage();
                            curMsg.Timestamp = DateTime.Now;
                            SW.WriteLine(DateTime.Now.ToString("HH:mm:ss.ffff") + " " + linedata); //Write to logfile.
                            string[] linedatasplit = linedata.Trim().Split(' ');
                            try
                            {
                                curMsg.serialport = linedatasplit[0]; //serial port
                                curMsg.SlaveID = Convert.ToInt32(linedatasplit[1], 16);
                                curMsg.FunctionCode = Convert.ToInt32(linedatasplit[2], 16);
                            }
                            catch (Exception ex)
                            {
                                SWError.WriteLine(linedata); //data that does not parse correctly.
                                continue;
                            }

                            for (int i = 3; i < (linedatasplit.Length); i++)
                            {
                                curMsg.Data.Add(Convert.ToInt32(linedatasplit[i], 16));
                            }
                            UniMessageCheck(curMsg);
                            MessageDisplayQueue.Enqueue(curMsg);
                        }
                        catch (Exception ex)
                        {
                            MessageBox.Show("error parsing serial data: " + ex.ToString());
                        }
                        continue;
                    }


                    int byteValue = serialPort.ReadByte();
                    byte[] bytedata = BitConverter.GetBytes(byteValue);
                    try
                    {
                        relayPort.Write(bytedata, 0, 1);
                    }
                    catch (Exception ex)
                    {

                    }

                    //Caclualate milliseconds since last byte received.
                    long milliseconds_diff = DateTimeOffset.Now.ToUnixTimeMilliseconds() - lastbyte_time;
                    lastbyte_time = DateTimeOffset.Now.ToUnixTimeMilliseconds();

                    //Check time since last packet is larger than interval to indicate packet completed.
                    if (milliseconds_diff > seperation_interval || curMsg.ValidateCRC())
                    {
                        //full message received.
                        string linedata = curMsg.Timestamp.ToString("yyyy.MM.dd.HH.mm.ss.ffffff") + "," + curMsg.SlaveID.ToString(OModeString) + "," + curMsg.FunctionCode.ToString(OModeString) + "," + curMsg.GetDataString(true) + "," + curMsg.GetCRC();
                        if (curMsg.ValidateCRC())
                        {
                            //Valid Message.

                            SW.WriteLine(linedata); //Write to logfile.

                            //uniquie messages list, add if required otherwise increase count.
                            UniMessageCheck(curMsg);

                            //Add to Display.
                            MessageDisplayQueue.Enqueue(curMsg);


                        }
                        else
                        {
                            //Log to invalid crc log.
                            SWError.WriteLine(linedata);
                        }

                        curMsg = new ModBusMessage(); //reset

                        packet_byte_number = 0; //byte seperator

                        /*
                        foreach (string timestamp in timelog)
                        {
                            SW.Write(timestamp + ",");
                        }
                        timelog.Clear();
                        SW.WriteLine();
                        */


                    }

                    //Update packet timestamp.
                    curMsg.Timestamp = DateTime.Now;

                    //Set SlaveID / Function Code.
                    if (packet_byte_number == 0)
                    {
                        curMsg.SlaveID = byteValue;
                        packet_byte_number++;
                    }
                    else if (packet_byte_number == 1)
                    {
                        curMsg.FunctionCode = byteValue;
                        packet_byte_number++;
                    }
                    else
                    {
                        curMsg.Data.Add(byteValue);
                    }

                }
                catch (Exception ex)
                {
                    //Failed to read byte - normal for timeout.
                }




            }
            serialPort.Close();
            SW.Close();
            SWError.Close();

            //Write all unique modbus messages (exclude messages duplicate SlaveID, Function Code and Messsage)
            StreamWriter SW2 = new StreamWriter("modbus_unique_" + serialPort.PortName + ".csv");
            SW2.WriteLine("RepeatCount,SlaveID,FunctionCode,Data");
            foreach (ModBusMessage umodmsg in UniMessages)
            {
                SW2.WriteLine(umodmsg.DetectionCount + "," + umodmsg.SlaveID.ToString("X2") + "," + umodmsg.FunctionCode.ToString("X2") + "," + umodmsg.GetDataString(true));
            }
            SW2.Close();
        }

        static void UniMessageCheck(ModBusMessage curMsg)
        {
            bool existingmsg = false;
            foreach (ModBusMessage ModBusMsg in UniMessages)
            {
                if (ModBusMsg.SlaveID == curMsg.SlaveID)
                {
                    if (ModBusMsg.FunctionCode == curMsg.FunctionCode)
                    {
                        if (ModBusMsg.GetDataString() == curMsg.GetDataString())
                        {
                            //Existing message detected
                            ModBusMsg.DetectionCount++; //Increase detection count.
                            existingmsg = true;
                            break;
                        }
                    }
                }
            }
            if (existingmsg == false)
            {
                UniMessages.Add(curMsg);
            }
        }

        static void SerialWriteQueueProcess(ref SerialPort serialPort, ref Queue<int> SerialWriteQueue)
        {
            while (stop_monitor == false && shutdown == false)
            {
                //Write any queued bytes from other serial port.
                if (SerialWriteQueue.Count > 0)
                {

                }
            }
        }

        private void btnStart_Click(object sender, EventArgs e)
        {
            stop_monitor = false;

            serialAPort.PortName = txtCOMA.Text;
            SerialThread = new Thread(() => MonitoringThread(serialAPort, serialBPort, rbtByteMode.Checked));
            SerialThread.Start();

            if (rbtByteMode.Checked && !string.IsNullOrEmpty(txtCOMB.Text))
            {
                serialBPort.PortName = txtCOMB.Text;
                SerialThread2 = new Thread(() => MonitoringThread(serialBPort, serialAPort, true));
                SerialThread2.Start();
            }


            btnStart.Enabled = false;
            btnStop.Enabled = true;
        }

        private void btnStop_Click(object sender, EventArgs e)
        {
            stop_monitor = true;
            btnStart.Enabled = true;
            btnStop.Enabled = false;
        }

        private void chkHexMode_CheckedChanged(object sender, EventArgs e)
        {
            if (chkHexMode.Checked)
            {
                OModeString = "X2";
            }
            else
            {
                OModeString = "";
            }
        }

        private void txtCRCTest_TextChanged(object sender, EventArgs e)
        {
            try
            {
                lblcrcresult.Text = ModBusMessage.CRCTestString(txtCRCTest.Text);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }

        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            shutdown = true;
        }

        private void tmrUpdate_Tick(object sender, EventArgs e)
        {
            string newdatastr = String.Empty;
            lock (monitormessagelock)
            {
                newdatastr = MonitorMessage;
            }
            txtMonitor.Text = newdatastr;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Clipboard.SetText(txtMonitor.Text);
        }

        private void txtByteMatchCount_TextChanged(object sender, EventArgs e)
        {
            if (String.IsNullOrEmpty(txtByteMatchCount.Text)) return; //no empty
            BytesToMatch = int.Parse(txtByteMatchCount.Text);
        }

        private void chkLength_CheckedChanged(object sender, EventArgs e)
        {
            BytesLengthmatch = chkLength.Checked;
        }



        public class ModBusMessage
        {
            public DateTime Timestamp;
            public int SlaveID;
            public int FunctionCode;
            public List<int> Data = new List<int>();
            public string serialport;

            public int DetectionCount = 1;

            public string GetDataString(bool ExcludeCRC = false)
            {
                string tmp = String.Empty;
                if (ExcludeCRC)
                {
                    for (int i = 0; i < Data.Count - 2; i++)
                    {
                        tmp = tmp + Data[i].ToString(OModeString) + " ";
                    }
                }
                else
                {
                    foreach (int i in Data)
                    {
                        tmp = tmp + i.ToString(OModeString) + " ";
                    }
                }
                
                //tmp = tmp + (ValidateCRC() ? "(VALID)" : "(NOT VALID)");
                
                return tmp;
            }

            public string GetCRC()
            {
                return Data[Data.Count - 2].ToString(OModeString) + Data[Data.Count - 1].ToString(OModeString);
            }


            public bool ValidateCRC()
            {
                if (Data.Count <= 2)
                {
                    //not enough bytes to validate.
                    return false;
                }

                //Get 16 Bit CRC.
                int crc = Data[Data.Count - 2] << 8; //bitshift 8. 0xFF00
                crc = crc + Data[Data.Count - 1];


                int databytes = Data.Count - 2;
                int[] dataonly = new int[Data.Count]; //allow for adding SlaveID,FunctionCode.
                dataonly[0] = SlaveID;
                dataonly[1] = FunctionCode;
                for (int i = 0; i < databytes; i++)
                {
                    dataonly[i + 2] = Data[i];
                }

                if (CRC(dataonly) == crc)
                {
                    return true;
                }

                return false;
            }

            public static ushort CRC(int[] data)
            {
                ushort crc = 0xFFFF;

                for (int pos = 0; pos < data.Length; pos++) //Loop each byte of data.
                {
                    //XOR - Exclusive Or
                    crc ^= (ushort)data[pos];

                    //a = binary 0101
                    //b = binary 0011
                    //a ^= b
                    //  Set a to 0110

                    for (int i = 8; i != 0; i--)      // Loop over each bit
                    {
                        if ((crc & 0x0001) != 0)        // If the LSB is set
                        {
                            crc >>= 1;                    // Shift right and XOR 0xA001
                            crc ^= 0xA001; //Polynomial for Modbus is 0xA001
                        }
                        else                            // Else LSB is not set
                        {
                            crc >>= 1;                    // Just shift right
                        }
                    }
                }

                return (ushort)((crc >> 8) | (crc << 8)); //Reverse Endian (High to Low bytes) using bitshift and combine with bitwise OR.

                //return crc; //Return without reversed endian.


            }

            public static string CRCTestString(string TestData)
            {
                //byte[] data = {0x8d, 0x10, 0 ,0x6e, 0, 1};
                //MessageBox.Show(ModBusMessage.CRC(data).ToString(OModeString));            
                //returns 7f18 for crc.

                /*
                //Test CRC Validation with test data.
                ModBusMessage tmpmsg = new ModBusMessage();
                tmpmsg.SlaveID = 0x8d;
                tmpmsg.FunctionCode = 0x10;
                tmpmsg.Data.Add(0);
                tmpmsg.Data.Add(0x6e);
                tmpmsg.Data.Add(0);
                tmpmsg.Data.Add(0x1);
                tmpmsg.Data.Add(0x2);
                tmpmsg.Data.Add(0x0);
                tmpmsg.Data.Add(0x0);

                tmpmsg.Data.Add(0x9a);
                tmpmsg.Data.Add(0x18);

                MessageBox.Show(tmpmsg.ValidateCRC().ToString());
                */

                if (String.IsNullOrEmpty(TestData) || !(TestData.Contains(" ")))
                {
                    return "Fail";
                }
                TestData = TestData.Replace(",", " "); //replace commas with spaces.

                string[] bytes_str = TestData.Split(' ');
                if (bytes_str.Length < 4) return "Fail";

                ModBusMessage tmpmsg = new ModBusMessage();
                try
                {
                    tmpmsg.SlaveID = int.Parse(bytes_str[0]);
                    tmpmsg.FunctionCode = int.Parse(bytes_str[1]);
                }
                catch (Exception ex)
                {
                    //will fail parse on hex input.
                }

                ModBusMessage tmpmsg_hex = new ModBusMessage();
                tmpmsg_hex.SlaveID = int.Parse(bytes_str[0], NumberStyles.HexNumber);
                tmpmsg_hex.FunctionCode = int.Parse(bytes_str[1], NumberStyles.HexNumber);

                for (int i = 2; i < bytes_str.Length; i++)
                {
                    if (String.IsNullOrEmpty(bytes_str[i])) continue; //skip empty
                    try
                    {
                        tmpmsg.Data.Add(int.Parse(bytes_str[i]));
                    }
                    catch (Exception ex)
                    {

                    }
                    try
                    {
                        tmpmsg_hex.Data.Add(int.Parse(bytes_str[i], NumberStyles.HexNumber));
                    }
                    catch (Exception ex)
                    {

                    }

                }

                if (tmpmsg.ValidateCRC()) return "Pass (Dec)";
                if (tmpmsg_hex.ValidateCRC()) return "Pass (Hex)";
                return "Fail";
            }



        }

        private void txtMonitor_TextChanged(object sender, EventArgs e)
        {

        }

        private void chkShowCRC_CheckedChanged(object sender, EventArgs e)
        {
            ExcludeCRC = chkExcludeCRC.Checked;
        }

        private void txtFilter_TextChanged(object sender, EventArgs e)
        {
            DisplayFilter = txtFilter.Text;
        }

        private void chkTimestamp_CheckedChanged(object sender, EventArgs e)
        {
            ExcludeTime = chkTimestamp.Checked;
        }

        private void rbtStringMode_CheckedChanged(object sender, EventArgs e)
        {
            chkExcludeCRC.Checked = false;
        }

        private void txtCOMA_TextChanged(object sender, EventArgs e)
        {

        }
    }


}

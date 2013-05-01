using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using System.Timers;


namespace wysyłanie
{
    public partial class Form1 : Form
    {
        String ax, ay, az, mx, my, mz, gx, gy, gz;
        private static SerialPort port;
        String text;
        //System.Timers.Timer clock;
        public Form1()
        {
            InitializeComponent();
            //% create an instance of serial port
                port = new SerialPort("COM9", 128000, Parity.None, 8, StopBits.One);
                port.ReadTimeout = 500;
                port.WriteTimeout = 500;
                
                port.Handshake = Handshake.None;

                port.Open();
                if (!port.IsOpen)
                    MessageBox.Show("Error", "Cannot open COM port", MessageBoxButtons.OK, MessageBoxIcon.Error);
                
                port.DataReceived += new SerialDataReceivedEventHandler(Port_DataReceived);
        }


        
        private void richTextBox1_TextChanged(object sender, EventArgs e)
        {
            
             
        }

        private void Port_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            text = port.ReadLine();
            this.Invoke(new EventHandler(displayText));           
        }
        private void displayText(object o, EventArgs e)
        {
            
            if (text.Length > 0)
            {
                String ktory = text.Substring(0, 2);
                switch (ktory)
                {
                    case "ax":
                        {
                            ax = text.Substring(2);
                            richTextBox1.AppendText(ax);
                            richTextBox1.ScrollToCaret();
                            break;
                        }
                    case "ay":
                        {
                            ay = text.Substring(2);
                            richTextBox2.AppendText(ay);
                            richTextBox2.ScrollToCaret();
                            break;
                        }
                    case "az":
                        {
                            az = text.Substring(2);
                            richTextBox3.AppendText(az);
                            richTextBox3.ScrollToCaret();
                            break;
                        }
                    case "mx":
                        {
                            mx = text.Substring(2);
                            richTextBox4.AppendText(mx);
                            richTextBox4.ScrollToCaret();
                            break;
                        }
                    case "my":
                        {
                            my = text.Substring(2);
                            richTextBox5.AppendText(my);
                            richTextBox5.ScrollToCaret();
                            break;
                        }
                    case "mz":
                        {
                            mz = text.Substring(2);
                            richTextBox6.AppendText(mz);
                            richTextBox6.ScrollToCaret();
                            break;
                        }
                    case "gx":
                        {
                            gx = text.Substring(2);
                            richTextBox7.AppendText(gx);
                            richTextBox7.ScrollToCaret();
                            break;
                        }
                    case "gy":
                        {
                            gy = text.Substring(2);
                            richTextBox8.AppendText(gy);
                            richTextBox8.ScrollToCaret();
                            break;
                        }
                    case "gz":
                        {
                            gz = text.Substring(2);
                            richTextBox9.AppendText(gz);
                            richTextBox9.ScrollToCaret();
                            port.Write("1");
                            break;
                        }
                        
                }
            }
        }
        
        private void button5_Click(object sender, EventArgs e)
        {
            port.Close();
            this.Close();
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void label5_Click(object sender, EventArgs e)
        {

        }

        private void label2_Click(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            port.Write("1");
        }
    }
}

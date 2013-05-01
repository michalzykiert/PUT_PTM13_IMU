using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;
using System.Timers;

namespace ConsoleApplication1
{
    class Program
    {




        static void Main(string[] args)
        {
            System.Timers.Timer clock;
            String ax, ay, az, mx, my, mz, gx, gy, gz;
            SerialPort port;
            String text;
           // String wyjscie;
            port = new SerialPort("COM9", 128000, Parity.None, 8, StopBits.One);
            port.ReadTimeout = 5000;
            port.WriteTimeout = 5000;

            port.Handshake = Handshake.RequestToSend;
            port.Open();
            if (!port.IsOpen)
                Console.WriteLine("port nie został otwarty");
            port.Write("1");
            for (; ; )
            {
                //try
                {
                    //System.Threading.Thread.Sleep(5);
                    text = port.ReadLine();
                    //Console.WriteLine(text);
                if (text.Length > 0)
                {
                    String ktory = text.Substring(0, 2);
                    switch (ktory)
                    {
                        case "ax":
                            {
                                ax = text.Substring(2);
                                Console.WriteLine(ax);
                                break;
                            }
                        case "ay":
                            {
                                ay = text.Substring(2);
                                Console.WriteLine(ay);
                                
                                break;
                            }
                        case "az":
                            {
                                az = text.Substring(2);
                                Console.WriteLine(az);
                                break;
                            }
                        case "mx":
                            {
                                mx = text.Substring(2);
                                Console.WriteLine(mx);
                                break;
                            }
                        case "my":
                            {
                                my = text.Substring(2);
                                Console.WriteLine(my);
                                break;
                            }
                        case "mz":
                            {
                                mz = text.Substring(2);
                                Console.WriteLine(mz);
                                break;
                            }
                        case "gx":
                            {
                                gx = text.Substring(2);
                                Console.WriteLine(gx);
                                break;
                            }
                        case "gy":
                            {
                                gy = text.Substring(2);
                                Console.WriteLine(gy);
                                break;
                            }
                        case "gz":
                            {
                                gz = text.Substring(2);
                                Console.WriteLine(gz);
                                Console.WriteLine("-----------------------------");
                                
                                port.Write("1");
                                break;
                            }

                    }
                }
                
                }
               //catch
                {
                   
                   
                    
                    
                }
            }
        }
    }
        
}
        
     


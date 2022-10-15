using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using Kyptron.Utils;

namespace Kyptron.Game
{
    class Client
    {
        public IPEndPoint Endpoint;
        public Client(IPEndPoint endPoint) => this.Endpoint = endPoint;

        public void Send(byte[] bytes)
        {
            Globals.Udp.Send(bytes, bytes.Length, Endpoint);
        }

        public async Task SendAsync(byte[] bytes)
        {
            await Globals.Udp.SendAsync(bytes, bytes.Length, Endpoint);
        }
    }

    class Server
    {
        public static void Start(int Port)
        {
            Console.WriteLine("");
            Console.WriteLine(" ██ ▄█▀▓██   ██▓ ██▓███  ▄▄▄█████▓ ██▀███   ▒█████   ███▄    █ ");
            Console.WriteLine(" ██▄█▒  ▒██  ██▒▓██░  ██▒▓  ██▒ ▓▒▓██ ▒ ██▒▒██▒  ██▒ ██ ▀█   █ ");
            Console.WriteLine("▓███▄░   ▒██ ██░▓██░ ██▓▒▒ ▓██░ ▒░▓██ ░▄█ ▒▒██░  ██▒▓██  ▀█ ██▒");
            Console.WriteLine("▓██ █▄   ░ ▐██▓░▒██▄█▓▒ ▒░ ▓██▓ ░ ▒██▀▀█▄  ▒██   ██░▓██▒  ▐▌██▒");
            Console.WriteLine("▒██▒ █▄  ░ ██▒▓░▒██▒ ░  ░  ▒██▒ ░ ░██▓ ▒██▒░ ████▓▒░▒██░   ▓██░");
            Console.WriteLine("▒ ▒▒ ▓▒   ██▒▒▒ ▒▓▒░ ░  ░  ▒ ░░   ░ ▒▓ ░▒▓░░ ▒░▒░▒░ ░ ▒░   ▒ ▒ ");
            Console.WriteLine("░ ░▒ ▒░ ▓██ ░▒░ ░▒ ░         ░      ░▒ ░ ▒░  ░ ▒ ▒░ ░ ░░   ░ ▒░");
            Console.WriteLine("░ ░░ ░  ▒ ▒ ░░  ░░         ░        ░░   ░ ░ ░ ░ ▒     ░   ░ ░ ");
            Console.WriteLine("░  ░    ░ ░                          ░         ░ ░           ░ ");
            Console.WriteLine("        ░ ░                                                    ");
            Console.WriteLine("");
            Console.WriteLine("[ == CREDITS == ]");
            Console.WriteLine("Array0x - Base Project, Connection, Switching Levels");
            Console.WriteLine("GD - Replication\n");
            Console.WriteLine("[KYPTRON LOG]: Starting Kyptron Server");
            Globals.Udp = new UdpClient(Port);
            new Thread(new ThreadStart(async () =>
            {
                Console.WriteLine("KYPTRON LOG: Started Kyptron Server");
                if(Globals.EnableMaxPlayers == true)
                {
                    Console.WriteLine("[KYPTRON LOG]: Client Slots: " + Globals.MaxPlayers);
                }
                else
                {
                    Console.WriteLine("[KYPTRON LOG]: Client Queue: ∞");
                }
                var Sender = new IPEndPoint(IPAddress.Any, Globals.Port);
                while (true)
                {
                    if (Sender != null)
                    {
                        var bytes = Globals.Udp.Receive(ref Sender);
                        if (bytes != null)
                        {
                            await Process(bytes, Sender);
                        }
                    }
                }

            })).Start();
        }

        private static async Task Process(byte[] Data, IPEndPoint Sender)
        {
            foreach (var Player in Globals.Players)
            {
                if (Player.Endpoint.Port == Sender.Port)
                {
                    await Player.Process(Data);
                    return;
                }
            }

            var NewPlayer = new Player(Sender);
            await NewPlayer.Process(Data);
        }
    }
}

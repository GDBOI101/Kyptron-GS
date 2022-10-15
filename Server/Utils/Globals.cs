using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net.Sockets;
using System.Threading;
using Kyptron.Game;
using Kyptron.Server;

namespace Kyptron.Utils
{
    public struct FVector
    {
        public float X;
        public float Y;
        public float Z;
    }

    public struct FRotator
    {
        public float Yaw;
        public float Pitch;
        public float Roll;
    }

    enum E_Direction : int
    {
        Forward = 0,
        Backwards = 1,
        Left = 2,
        Right = 3,
        NotMoving = 4
    }
    class Globals
    {
        // [ == UDP Settings == ]
        public static int Port = 7777;
        public static UdpClient Udp;
        public static List<Player> Players = new List<Player>();

        // [ == Player Settings == ]
        public static bool EnableMaxPlayers = true;
        public static int MaxPlayers = 69;
        public static bool BanPlayersAllow = true;
        public static string DefaultPawn = "BlueprintGeneratedClass /Game/Abilities/Player/Pawns/PlayerPawn_Generic.PlayerPawn_Generic_C";

        // [ == Game Settings == ]
        public static string MapName = "Zone_Outpost_CannyValley";
        public static bool AllowHusks = false;
        public static bool AllowDamage = false;

        // [ == Global Settings == ]
        public static float GSVersion = 1.0f;
        public static string GameVersion = "4.";
        public static int TickSpeed = 120;
        static long lastTime = 0;

        public static double GetDeltaTime()
        {
            long now = DateTime.Now.Millisecond;
            double dT = (now - lastTime);
            lastTime = now;
            return dT;
        }

        public static void StartConsole()
        {
            new Thread(new ThreadStart(() =>
            {
                while (true)
                {
                    Console.WriteLine("Enter a Command:\n");
                    String Command = Console.ReadLine();
                    Commands(Command);
                }
            })).Start();
        }
        public static async void Commands(String command)
        {
            switch (command)
            {
                case "CrashServer":
                    Console.WriteLine("Crashing Server");
                    foreach (var _P in Globals.Players)
                    {
                        if (_P != null)
                        {
                            await Functions.ForceLeave(_P);
                        }
                    }
                    break;
            }
        }
        public static void StartPlayerLoop()
        {
            new Thread(new ThreadStart(() =>
            {
                while (true)
                {
                    foreach (var _Player in Globals.Players.ToArray())
                    {
                        var Writer = new BitWriter();

                        Writer.Write("Tick");
                        Writer.Write(_Player.PlayerName);
                        Writer.Write(_Player.Location.X);
                        Writer.Write(_Player.Location.Y);
                        Writer.Write(_Player.Location.Z);
                        Writer.Write(_Player.Rotation.Pitch);
                        Writer.Write(_Player.Rotation.Yaw);
                        Writer.Write(_Player.Rotation.Roll);
                        Thread.Sleep(500);
                        _Player.Send(Writer.Dump());
                    }
                    Thread.Sleep(1000);
                }
            })).Start();
        }
    }
}

using Kyptron.Server;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Kyptron.Game
{

    class Functions
    {
        private static Random random = new Random();

        public static string RandomString(int length)
        {
            const string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
            return new string(Enumerable.Repeat(chars, length)
                .Select(s => s[random.Next(s.Length)]).ToArray());
        }
        public static async Task SwitchLevel(Player TargetPlayer, String Map)
        {
            var Writer = new BitWriter();
            Writer.Write("SwitchLevel");
            Writer.Write(TargetPlayer.PlayerName);
            Writer.Write(Map);
            Console.WriteLine("[KYPTRON LOG]: A new client has connected");
            Console.WriteLine(Map);
            await TargetPlayer.SendAsync(Writer.Dump());
        }

        public static async Task InitPlayer(Player TargetPlayer, string NewPlayerName)
        {
            var Writer = new BitWriter();
            Writer.Write("InitPlayer");
            var Player = NewPlayerName;
            Writer.Write(Player);
            await TargetPlayer.SendAsync(Writer.Dump());
        }

        public static async Task PlayerTick(Player TargetPlayer, Player sender)
        {
            var Writer = new BitWriter();

            Writer.Write("Tick");
            Writer.Write(sender.PlayerName);
            //Location (FVector)
            Writer.Write(sender.Location.X);
            Writer.Write(sender.Location.Y);
            Writer.Write(sender.Location.Z);
            //Rotation (FRotator)
            Writer.Write(sender.Rotation.Yaw);
            Writer.Write(sender.Rotation.Pitch);
            Writer.Write(sender.Rotation.Roll);
            await TargetPlayer.SendAsync(Writer.Dump());
        }

        public static async Task ForceLeave(Player TargetPlayer)
        {
            var Writer = new BitWriter();
            Writer.Write("ForceLeave");
            await TargetPlayer.SendAsync(Writer.Dump());
        }

    }
}

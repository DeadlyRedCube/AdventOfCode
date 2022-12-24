// See https://aka.ms/new-console-template for more information
using AdventOfCode2022;
using System.Diagnostics;
using System.Timers;

Console.WriteLine("Hello, World!");

var sw = Stopwatch.StartNew();
//D01.Run(File.ReadAllText("D01.txt"));
//D02.RunP1(File.ReadAllText("D02.txt"));
//D02.RunP2(File.ReadAllText("D02.txt"));
//D03.Run(File.ReadAllText("D03.txt"));
//D04.Run(File.ReadAllText("D04.txt"));
//D05.RunP1(File.ReadAllText("D05.txt"));
//D05.RunP2(File.ReadAllText("D05.txt"));
//D06.RunP1(File.ReadAllText("D06.txt"));
//D06.RunP2(File.ReadAllText("D06.txt"));
//D07.Run(File.ReadAllText("D07.txt"));
//D08.Run(File.ReadAllText("D08.txt"));
//D09.RunP1(File.ReadAllText("D09.txt"));
//D09.RunP2(File.ReadAllText("D09.txt"));
//D10.Run(File.ReadAllText("D10.txt"));
//D11.RunP1(File.ReadAllText("D11.txt"));
//D11.RunP2(File.ReadAllText("D11.txt"));
//D12.Run(File.ReadAllText("D12.txt"));
//D13.Run(File.ReadAllText("D13.txt"));
//D14.Run(File.ReadAllText("D14.txt"));
//D15.Run(File.ReadAllText("D15.txt"));
//D16.Run(File.ReadAllText("D16.txt"));
//D17.RunP1(File.ReadAllText("D17.txt"));
//D17.RunP2(File.ReadAllText("D17.txt"));
//D18.Run(File.ReadAllText("D18.txt"));
//D19.Run(File.ReadAllText("D19.txt"));
//D20.RunP1(File.ReadAllText("D20.txt"));
//D20.RunP2(File.ReadAllText("D20.txt"));
//D21.Run(File.ReadAllText("D21.txt"));
//D22.Run(File.ReadAllText("D22.txt"));
//D23.Run(File.ReadAllText("D23.txt"));
D24.Run(File.ReadAllText("D24.txt"));

long elapsed = sw.ElapsedMilliseconds;
Console.WriteLine($"Run time: {elapsed / 1000 / 60:D2}:{(elapsed / 1000) % 60:D2}:{elapsed % 1000:D4}");
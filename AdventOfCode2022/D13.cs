using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace AdventOfCode2022
{
  internal static class D13
  {
    class Packet
    {
      public List<object> entries = new List<object>();

      public Packet() { }
      public Packet(object item)
      {
        entries.Add(item);
      }

      public static int Compare(Packet a, Packet b)
      {
        for (int i = 0; i < Math.Min(a.entries.Count, b.entries.Count); i++)
        {
          var ea = a.entries[i];
          var eb = b.entries[i];
          if (ea is int && eb is int)
          {
            int cmp = ((int)ea).CompareTo((int)eb);
            if (cmp != 0)
              { return cmp; }
          }
          else
          {
            if (ea is int)
              { ea = new Packet(ea); }
            if (eb is int)
              { eb = new Packet(eb); }
            
            int cmp = Compare((ea as Packet)!, (eb as Packet)!);
            if (cmp != 0)
            {
              return cmp;
            }
          }
        }

        // If we got here we ran out of elements before comparison 
        return a.entries.Count.CompareTo(b.entries.Count);
      }
    }

    static Packet ParsePacket(string input, ref int i)
    {
      Debug.Assert(input[i] == '[');

      var p = new Packet();
      i++;
      for (; input[i] != ']'; i++)
      {
        // skip commas, whatever
        if (input[i] == ',')
          { continue; }

        if (input[i] == '[')
          { p.entries.Add(ParsePacket(input, ref i)); }
        else
        {
          Debug.Assert(char.IsDigit(input[i]));
          StringBuilder b = new StringBuilder();
          while (char.IsDigit(input[i]))
          {
            b.Append(input[i]);
            i++;
          }

          p.entries.Add(int.Parse(b.ToString()));

          // drop back to the end of the int so that our loop increments properly
          i--;
        }
      }

      return p;
    }

    public static void Run(string input)
    {
      // trim all whitespace
      input = String.Concat(input.Where(c => !char.IsWhiteSpace(c)));

      List<Packet> packs = new List<Packet>();
      for (int i = 0; i < input.Length; i++)
        { packs.Add(ParsePacket(input, ref i)); }

      // Figure out which pairs are in proper order
      int sumCorrect = 0;
      for (int i = 0; i < packs.Count; i += 2)
      {
        if (Packet.Compare(packs[i], packs[i + 1]) <= 0)
          { sumCorrect += i / 2 + 1; }
      }

      Console.WriteLine($"[p1] correct sum: {sumCorrect}");

      // Part 2: add these packets
      var div2 = new Packet(new Packet(2)); 
      var div6 = new Packet(new Packet(6));
      packs.Add(div2);
      packs.Add(div6);

      // Sort
      packs.Sort(Packet.Compare);

      // Multiply the 1-based indices of them in the sorted array to get our score
      int divProd = packs.Select((p, i) => (p, i)).Where(v => v.p == div2 || v.p == div6).Aggregate(1, (prod, v) => prod *= v.i + 1);
      Console.WriteLine($"[p2] divider packet score: {divProd}");
    }
  }
}

using System;
using System.Runtime.InteropServices;

class TestKunzip
{
  [DllImport ("kunzip.dll", CallingConvention=CallingConvention.StdCall)]
  private static extern int kunzip_inflate_init();

  [DllImport ("kunzip.dll", CallingConvention=CallingConvention.StdCall)]
  private static extern int kunzip_inflate_free();

  [DllImport ("kunzip.dll", CallingConvention=CallingConvention.StdCall)]
  private static extern int kunzip_get_offset_by_name(
    [MarshalAs(UnmanagedType.LPStr)] string zip_filename,
    [MarshalAs(UnmanagedType.LPStr)] string compressed_filename,
    int match_flags,
    int skip_offset);

  [DllImport ("kunzip.dll", CallingConvention=CallingConvention.StdCall)]
  private static extern int kunzip_next(
    [MarshalAs(UnmanagedType.LPStr)] string zip_filename,
    [MarshalAs(UnmanagedType.LPStr)] string base_dir,
    int offset);

  public static void Main(string[] args)
  {
    int offset;

    if (args.Length != 1)
    {
      Console.WriteLine("Usage: TestKunzip <filename>");
      return;
    }

    Console.WriteLine(args[0]);
/*
    kunzip_inflate_init();

    offset = kunzip_get_offset_by_name(args[1], "test.txt", 3, -1);
    Console.WriteLine(offset);

    offset = kunzip_next(args[1], ".", offset);
    Console.WriteLine(offset);

    kunzip_inflate_free();
*/
  }
}


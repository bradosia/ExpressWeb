namespace ExampleProgram {
    using System.Runtime.InteropServices;
    using System;

    class Callback : ResourceFileUtility.CallbackHandler {
        public override int fileComplete(string filePath) {
            Console.WriteLine("Sucessfully Packed: " + filePath);
            return 0;
        }
        public override int packComplete(string filePath) {
            Console.WriteLine("Sucessfully Packed all files!");
            return 0;
        }
    }

    class Program {
        static void Main(string[] args) {
            ResourceFileUtility.Compiler RFUCompiler = new ResourceFileUtility.Compiler();
            RFUCompiler.info("resources.json");
            RFUCompiler.setCallback(new Callback());
            RFUCompiler.pack("assets.data");

            // Keep the console window open in debug mode.
            Console.WriteLine("Press any key to exit.");
            Console.ReadKey();
        }
    }
}
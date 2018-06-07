namespace ResourceFileUtility {
    using System.Runtime.CompilerServices;
    using System.Runtime.InteropServices;
    using System;

    public abstract class CallbackHandler {
        public abstract int fileComplete(string filePath);
        public abstract int packComplete(string filePath);
    }

    class Loader {
        [DllImport("ResourceFileUtility.dll", CallingConvention = CallingConvention.StdCall)]
        static extern IntPtr loader_new();

        private IntPtr thisPtr;

        public Loader() {
            thisPtr = loader_new();
        }
        public IntPtr ptr() {
            return thisPtr;
        }
    }

    public class Compiler {
        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int CBintString(string text);
        [DllImport("ResourceFileUtility.dll", CallingConvention = CallingConvention.StdCall)]
        static extern IntPtr compiler_new();
        [DllImport("ResourceFileUtility.dll", CallingConvention = CallingConvention.StdCall)]
        static extern void compiler_info(IntPtr ptr, string fileName);
        [DllImport("ResourceFileUtility.dll", CallingConvention = CallingConvention.StdCall)]
        static extern void compiler_pack(IntPtr ptr, string fileName);
        [DllImport("ResourceFileUtility.dll", CallingConvention = CallingConvention.StdCall)]
        static extern void compiler_setCallbackFileComplete(IntPtr ptr, CBintString handler_);
        [DllImport("ResourceFileUtility.dll", CallingConvention = CallingConvention.StdCall)]
        static extern void compiler_setCallbackPackComplete(IntPtr ptr, CBintString handler_);


        private IntPtr thisPtr;
        private CBintString callbackFileComplete;   // Ensure it doesn't get garbage collected
        private CBintString callbackPackComplete;

        public Compiler() {
            thisPtr = compiler_new();
        }
        public IntPtr ptr() {
            return thisPtr;
        }
        public void info(string fileName) {
            compiler_info(thisPtr, fileName);
        }
        public void pack(string fileName) {
            compiler_pack(thisPtr, fileName);
        }
        public void setCallback(CallbackHandler cb) {
            callbackFileComplete = (arg1) => {
                return cb.fileComplete(arg1);
            };
            callbackPackComplete = (arg1) => {
                return cb.packComplete(arg1);
            };
            compiler_setCallbackFileComplete(thisPtr, callbackFileComplete);
            compiler_setCallbackPackComplete(thisPtr, callbackPackComplete);
        }

    }

    class Stream {
        [DllImport("ResourceFileUtility.dll", CallingConvention = CallingConvention.StdCall)]
        static extern IntPtr stream_new();

        private IntPtr thisPtr;

        public Stream() {
            thisPtr = stream_new();
        }
        public IntPtr ptr() {
            return thisPtr;
        }
    }
}
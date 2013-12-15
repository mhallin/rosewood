#import <Cocoa/Cocoa.h>

#import <Python.h>

int main(int argc, char *argv[])
{
    @autoreleasepool {
        NSString *resourcePath = [NSBundle mainBundle].resourcePath;
        NSArray *pythonPathArray = @[resourcePath, [resourcePath stringByAppendingPathComponent:@"PyObjC"]];

        setenv("PYTHONPATH", [pythonPathArray componentsJoinedByString:@":"].UTF8String, 1);

        Py_SetProgramName("/usr/bin/python");
        Py_Initialize();
        PySys_SetArgv(argc, (char **)argv);

        return NSApplicationMain(argc, (const char **)argv);

    }
}

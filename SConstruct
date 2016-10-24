import os, sys

def uniqueCheckLib(conf, lib, header = None):
    """ Checks for a library and appends it to env if not already appended. """
    if header:
        if not conf.CheckLibWithHeader(lib, header = header, autoadd=0, language="C++"):
            print "ERROR: Library '" + lib + "' or header '" + header + "' not found."
            Exit(1)
    else:
        if not conf.CheckLib(lib, autoadd=0, language="C++"):
            print "ERROR: Library '" + lib + "' not found!"
            Exit(1)

    conf.env.AppendUnique(LIBS = [lib])
        
vars = Variables(None, ARGUMENTS)

vars.Add(EnumVariable('build', 'Build type, either release or debug', "debug", allowed_values=('release', 'debug')))
vars.Add("compiler", "Compiler to use.", "mpicxx")

env = Environment(variables = vars, ENV = os.environ)
conf = Configure(env)

Help(vars.GenerateHelpText(env))

preciceRoot = os.getenv("PRECICE_ROOT")
if preciceRoot == None:
    print("PRECICE_ROOT is not set.")
    Exit(1)

env.Replace(CXX = env["compiler"])

env.Append(CCFLAGS = ['-Wall', '-std=c++11'])

if env["build"] == "debug":
    env.Append(CCFLAGS = ['-O0', '-g3'])
    env.Append(LINKFLAGS = ["-rdynamic"])
elif env["build"] == "release":
    env.Append(CCFLAGS = ['-O3'])

    
env.Append(CPPPATH = [os.path.join(preciceRoot, "src")])
env.Append(LIBPATH = [os.path.join(preciceRoot, "build/last")])
    
uniqueCheckLib(conf, "precice", header = "precice/SolverInterface.hpp")
uniqueCheckLib(conf, "boost_system")
uniqueCheckLib(conf, "boost_program_options", header = "boost/program_options.hpp")
uniqueCheckLib(conf, "boost_filesystem", header = "boost/filesystem.hpp")
    
env = conf.Finish()
    
env.Program ( 'aste', ['main.cpp'] )
env.Program ( 'readMesh', ['readMesh.cpp'] )

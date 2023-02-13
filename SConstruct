import os
#Binary in ./build
VariantDir('build','.',duplicate=0)

#Absolute Include paths 
env = Environment(ENV= os.environ, tools = ["default"],toolpath=".", CPPPATH= ['#/include'],LIBPATH= ['#lib'], LINKFLAGS=[''])

#CCFLAGS
env.Append(CCFLAGS='-std=c++0x -O3 -pedantic -Wall  -frounding-math -W -Wno-long-long -DNDEBUG -D_FLOATTEST')
#env.Append(CCFLAGS='-O0 -g  -Wall -W -pedantic -Wno-long-long -std=c++0x ')


Export('env')
SConscript('test/SConscript')

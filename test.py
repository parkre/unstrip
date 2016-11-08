import inject, sys

sys.settrace

inject.injectSyms('./hello','./symbols_file','./fakeout')

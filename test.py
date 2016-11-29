import inject, sys

sys.settrace

inject.injectSyms('./test','./symbols_file','./fakeout')

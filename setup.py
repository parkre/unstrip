from distutils.core import setup, Extension

module = Extension('inject', 
                   sources = ['injectModule.c'],
                   extra_compile_args = ['-I/libelfsh/include', '-DERESI64', '-I/libaspect/include', '-I/libasm/include'],
                   extra_link_args = ['-L/libelfsh/', '-lelfsh64', '-L/libaspect/', '-laspect64', '-L/libasm/', '-lasm64'])

setup(name = 'unstrip',
      verison = '1.0',
      description = 'This is a package for injectModule',
      ext_modules = [module])




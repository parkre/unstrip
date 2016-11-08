#! /usr/local/lib/python2.7

import bap
from sys import argv


def scan_sections():
    sections = lib.sections.values()
    functions = []
    for sym in symbols:
        func = {}
        for sec in sections:
            if sym['begin'] > sec.beg and sym['end'] < sec.end:
                func['name'] = sym['name']
                func['begin'] = sym['begin']
                func['length'] = sym['length']
                func['offset'] = sym['begin'] - sec.beg
                sub_data = sec.data[func['offset'] : func['offset'] + sym['length']]
                func['code'] = ':'.join(x.encode('hex') for x in sub_data)
                functions.append(func)
                #print 'Symbol: %s\n%s\n' % (sym['name'], func['code'])
    return functions

def get_symbols():
    memmap = lib.memmap
    syms = []
    for mem in memmap:
        memstr = str(mem)
        sym_index = memstr.find('symbol')
        sym = {}
        if (sym_index > -1):
            sym['name'] = memstr[sym_index+10:-3]
            part = memstr[18:].partition(',')
            sym['begin'] = int(part[0], 16)
            sym['end'] = int(part[2][1:].partition(')')[0], 16)
            sym['length'] = int(sym['end'] - sym['begin'] + 1)
            syms.append(sym)
    return syms

def find_matches():
    text = stripped.sections['.text']
    code = ':'.join(x.encode('hex') for x in stripped.sections['.text'].data)
    print 'Symbol Table'
    for func in functions:
        index = code.find(func['code'])
        if index > -1:
            print "<%s, %s, %d>" % (func['name'], hex(text.beg + index), func['length'])
        

# run 
filename = argv[1]
lib = bap.run('a.out')
symbols = get_symbols()
functions = scan_sections()
stripped = bap.run(filename)
find_matches()


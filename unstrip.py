#! /usr/local/lib/python2.7

import bap
from sys import argv


def scan_sections():
    sections = proj.sections.values()
    for sym in symbols:
        for sec in sections:
            if sym['begin'] > sec.beg and sym['end'] < sec.end:
                #print 'section_begin: %d, function_begin: %d' % (sec.beg, sym['begin'])
                offset = sym['begin'] - sec.beg
                sub_data = sec.data[offset : offset + sym['length']]
                code = ':'.join(x.encode('hex') for x in sub_data)
                print 'Symbol: %s\n%s\n' % (sym['name'], code)

def get_symbols():
    memmap = proj.memmap
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
            #print "Name: %s, Begin: %d, End: %d" % (sym['name'], sym['begin'], sym['end'])
    return syms

# run 
filename = argv[1]
proj = bap.run(filename)
symbols = get_symbols()
scan_sections()

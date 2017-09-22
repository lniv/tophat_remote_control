#!/usr/bin/python
"""
process an xcsoar / tophat input event configuration file and create a graph from it
niv levy april 2014 gpl v2 or later
"""

def xci2dot(in_filename, out_filename, desired_modes = ['key', 'gesture'], only_mode_targets = False, drop_set_default = True):
    print 'Processing ', in_filename, ' and creating ', out_filename
    print 'Only creating lines for mode changes = ', only_mode_targets
    print 'dropping auxiliary setting of default mode = ', drop_set_default
    print 'Limiting to the following modes: ', desired_modes
    f = open(in_filename, 'r')
    out_f = open(out_filename, 'w')
    out_f.write('digraph  xci {\n') 
    lines = f.readlines()
    # drop all comments
    lines = [line for line in lines if not line.startswith('#')]
    
    # find main nodes (= modes)
    premodes = [line.strip().split('=')[1].split(' ') for line in lines if line.startswith('mode=')]
    modes = set(reduce(lambda x, y: x + y, premodes,[]))
    capital_modes = tuple([mode.capitalize() for mode in modes]) # useful later, mostly for pan, maybe others
    out_f.writelines(['"' + mode.strip() +  '" [style=filled, fillcolor=red]\n' for mode in modes])
    
    
    # make event / input chunkcs
    pre_chunks = reduce(lambda x, y: x + y, lines,"").split('\n\n')
    chunks = [line.lstrip() for line in pre_chunks if len(line) > 0]
    
    
    for i, chunk in enumerate(chunks):
        # if we have more than one event, we'd like to rpoduce two arrows, but the problem is that dot will naively connect other things (since the first event will be likely something common like 'Mode default' - bests bet is probbaly to add it in the label for the line in this case?
        targets = [l.split('=')[1].strip() for l in chunk.split('\n') if l.startswith('event')]
        # lots of actions set default at some point besides doing something else; it's too annoying and cluttering, hence the option to hide it, though i think i can choose a better name - hide_default?
        if drop_set_default and len(targets) > 1:
            if targets[0].rstrip().endswith("default") :
                targets.pop(0)
                #targets[0] = '(default) +' + targets[0]
                print 'dropped leading set default'
            elif targets[-1].rstrip().endswith("default") :
                targets.pop()
                #targets[-1] = targets[-1] + ' + (default)'
                print 'dropped trailing set default'
        sources = [l for l in chunk.split('\n') if l.startswith('mode')][0].split('=')[1].split(' ')
        if len(targets) > 0:
            d = dict([line.split('=') for line in chunk.split('\n') if not line.startswith('event') and len(line) > 0])
            if d['type'] in desired_modes:
                d['event'] = reduce(lambda x,y: x + ' "' + y + '"', [line.rpartition('Mode ')[2] for line in targets if line.startswith('Mode') or line.capitalize().startswith(capital_modes) or not only_mode_targets], "{") + ' }'
                #print targets, d['event']
                out_f.writelines(['"' + source + '" -> ' + d['event'].strip().rpartition('Mode ')[2] + '[ label = "'  + d['data'].strip() + '"]\n' for source in sources])
            else:
                print 'skipping - type= ', d['type']

    f.close()
    out_f.write('}\n')
    out_f.close()
    
if __name__ == '__main__':
    import sys
    if len(sys.argv) > 1:
        xci2dot(sys.argv[1], sys.argv[2])
    else:
        print 'usage: xci2dot xci_file_name output_dot_file_name'
#!/usr/bin/env python3

import sys, os
from pycparser import c_parser, c_ast, parse_file, c_generator


def remove_tabs(text):
    return text.replace('\t', '').replace('* ', '*')
    

def make_func_proto(node, is_fnptr=False):
    func_decl = node
    args = []
    for arg in func_decl.args.params:
        if isinstance(arg, c_ast.Typename):
            args.append(remove_tabs(make_proto(arg, omit_name=True)))
        elif isinstance(arg, c_ast.Decl):
            args.append(remove_tabs(make_proto(arg, omit_name=True)))
        elif isinstance(arg, c_ast.EllipsisParam):
            args.append('...')
        else:
            arg.show()
    return '%s\t(%s)' % (make_proto(func_decl, is_fnptr=is_fnptr), ', '.join(args))
    

def make_proto(node, omit_name=False, is_fnptr=False):
    if isinstance(node.type, c_ast.PtrDecl):
        if isinstance(node.type.type, c_ast.FuncDecl):
            return make_func_proto(node.type.type, is_fnptr=True)
        ptr_type = node.type.type        
        quals = ptr_type.quals
        declname = ptr_type.declname
        ret_type = 'void *' # all pointers become void
    else:
        try:
            quals = node.type.quals
            declname = node.type.declname
            ret_type = node.type.type.names[0]
        except:
            node.type.show()
            return ''
    comps = quals + [ret_type] 
    if declname is not None: 
        comps += ['\t' + ('(*%s)' % declname if is_fnptr else declname)]
    elif is_fnptr:
        comps += ['\t(*)']
    return ' '.join(comps)
    

# A simple visitor for FuncDef nodes that prints the names and
# locations of function definitions.
class FuncDeclVisitor(c_ast.NodeVisitor):
    def __init__(self, where):
        c_ast.NodeVisitor.__init__(self)
        self.where = where
        self.protos = []
        
    def visit_Typedef(self, node):
        if self.where in str(node.coord):
            if not isinstance(node.type, c_ast.TypeDecl):
                cgen = c_generator.CGenerator()
                self.protos.append(cgen.visit(node) + ';')
        
    def visit_FuncDecl(self, node):
        if self.where in str(node.coord):
            #print(dir(node))
            self.protos.append(make_func_proto(node) + ';')


def show_func_defs(filename):
    # Note that cpp is used. Provide a path to your own cpp or
    # make sure one exists in PATH.
    ast = parse_file(filename, use_cpp=True,
                     cpp_path='clang',
                     cpp_args=['-E',
                         r'-std=c11', 
                         r'-Iinclude',
                         r'-Ilang/stubs',
                         r'-I../build-cplus-Desktop-Debug',
                         r'-I/Users/jaakko/src/pycparser/utils/fake_libc_include'
                     ])
    #ast.show()
    v = FuncDeclVisitor(os.path.basename(filename))
    v.visit(ast)
    print('#include "protodefs.h"\nextern "C" {')
    protos = []
    # Determine field widths.
    field_widths = [0, 0, 0]
    for line in v.protos:
        parts = [s.strip() for s in line.split('\t')]
        #print(parts)
        i = 0
        while i < len(parts):
            part = parts[i]
            if not part.startswith('typedef'):
                width = len(part)
                field_widths[i] = max(field_widths[i], width)
            i += 1      
    # Print the final prototypes.
    for line in v.protos:
        parts = [s.strip() for s in line.split('\t')]
        prototype = ''
        i = 0
        while i < len(parts):
            prototype += parts[i] + ' '*(1 + field_widths[i] - len(parts[i]))
            i += 1
        print(prototype)
        
    print('} // extern "C"')


if __name__ == "__main__":
    show_func_defs(sys.argv[1])
    
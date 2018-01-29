#!/usr/bin/env python3

import sys, os
from pycparser import c_parser, c_ast, parse_file, c_generator

def make_func_proto(node, is_fnptr=False):
    func_decl = node
    #cgen = c_generator.CGenerator()
    #print(cgen.visit(func_decl))
    args = []
    for arg in func_decl.args.params:
        if isinstance(arg, c_ast.Typename):
            args.append(make_proto(arg, omit_name=True))
        elif isinstance(arg, c_ast.Decl):
            args.append(make_proto(arg, omit_name=True))
        elif isinstance(arg, c_ast.EllipsisParam):
            args.append('...')
        else:
            arg.show()
    return '%s(%s)' % (make_proto(func_decl, is_fnptr=is_fnptr), ', '.join(args))
    

def make_proto(node, omit_name=False, is_fnptr=False):
    if isinstance(node.type, c_ast.PtrDecl):
        if isinstance(node.type.type, c_ast.FuncDecl):
            return make_func_proto(node.type.type, is_fnptr=True)
        ptr_type = node.type.type        
        quals = ptr_type.quals
        declname = ptr_type.declname
        ret_type = 'void *' # all pointers become void
    else:
        quals = node.type.quals
        declname = node.type.declname
        ret_type = node.type.type.names[0]
    comps = quals + [ret_type] 
    if declname is not None: 
        comps += ['(*%s)' % declname if is_fnptr else declname]
    elif is_fnptr:
        comps += ['(*)']
    #print(is_fnptr, comps)
    return ' '.join(comps)
    

# A simple visitor for FuncDef nodes that prints the names and
# locations of function definitions.
class FuncDeclVisitor(c_ast.NodeVisitor):
    def __init__(self, where):
        c_ast.NodeVisitor.__init__(self)
        self.where = where
        
    def visit_Typedef(self, node):
        if self.where in str(node.coord):
            if not isinstance(node.type, c_ast.TypeDecl):
                cgen = c_generator.CGenerator()
                print(cgen.visit(node) + ';')
        
    def visit_FuncDecl(self, node):
        if self.where in str(node.coord):
            #print(dir(node))
            print(make_func_proto(node) + ';')


def show_func_defs(filename):
    # Note that cpp is used. Provide a path to your own cpp or
    # make sure one exists in PATH.
    ast = parse_file(filename, use_cpp=True,
                     cpp_path='clang',
                     cpp_args=['-E',
                         r'-std=c11', 
                         r'-Iinclude',
                         r'-I../build-cplus-Desktop-Debug',
                         r'-I/Users/jaakko/src/pycparser/utils/fake_libc_include'
                     ])
    #ast.show()
    v = FuncDeclVisitor(os.path.basename(filename))
    v.visit(ast)


if __name__ == "__main__":
    show_func_defs(sys.argv[1])
import waflib.Task
import os

# configure can provide...
# cfg.extensions_includes  - Append directory names relative to top that will
def configure(cfg):
    cfg.check_cxx(stlib='expat', cflags='-Wall', uselib_store='EXPAT')
    cfg.extensions_stlib += cfg.env.STLIB_EXPAT
    cfg.extensions_stlib += cfg.env.STLIB_Z
    cfg.define("BUILD_EXTENSION",1)   # add this whenever building an extension
#    cfg.define("DEBUG_ENERGY_FUNCTION",1)


def build(bld):
    print("In extensions build bld.cclasp_executable = %s" % bld.cclasp_executable)
    print("      bld.stage_val = %s" % bld.stage_val)
    cando_project_headers_name = "include/cando/main/project_headers.h"
    cando_project_headers = bld.path.find_node(cando_project_headers_name)
    assert cando_project_headers!=None, "Could not find %s in %s" % (cando_project_headers_name,bld.path.abspath())
    bld.extensions_gcinterface_include_files.append(cando_project_headers)
    bld.extensions_include_dirs.append("extensions/cando/include")
    bld.extensions_include_dirs.append("extensions/cando/include/cando/main")
    bld.recurse('include')
    bld.recurse('src')
    if (bld.stage_val>=4):
        bld_extensions = build_extension(env=bld.env)
        bld_extensions.set_inputs([bld.cclasp_executable,bld.asdf_fasl])
        bld_extensions.set_outputs([bld.path.parent.parent.find_or_declare("fasl/cando.fasb"),
                                    bld.path.parent.parent.find_or_declare("fasl/cando-jupyter.fasb")])
        bld.install_files('${PREFIX}/lib/clasp/', bld_extensions.outputs, relative_trick = True, cwd = bld.path.parent.parent)   #source
        
        bld.add_to_group(bld_extensions)
    print("Leaving extensions build without cclasp_executable")

class build_extension(waflib.Task.Task):
    def run(self):
        cmd = [ self.inputs[0].abspath(),
                "-l", "source-dir:extensions;cando;src;lisp;build-cando.lisp",
                "-e", "(asdf:make :build-cando :print t)",
                "-l", "source-dir:extensions;cando;src;lisp;build-cando-jupyter.lisp",
                "-e", "(asdf:make :build-cando-jupyter :print t)",
                "-e", "(core:quit)" ]
        print("build_extension cmd -> %s" % cmd)
        print("build_extension outputs -> %s" % self.outputs)
        return self.exec_command(cmd)

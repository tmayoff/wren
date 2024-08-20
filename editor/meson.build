deps = [wren_dep]
args = []

bfd = cpp.find_library('bfd', required: false)
if bfd.found()
    args += '-DBACKWARD_HAS_BFW=1'
    deps += bfd
endif

dw = cpp.find_library('dw', required: false)
if dw.found()
    args += '-DBACKWARD_HAS_DW=1'
    deps += dw
endif

dwarf = dependency('libdwarf', required: false)
if dwarf.found()
    args += '-DBACKWARD_HAS_DWARF=1'
    deps += dwarf
endif

executable(
    'wren_editor',
    'src/main.cpp',
    dependencies: deps,
    cpp_args: args,
    install: true,
)

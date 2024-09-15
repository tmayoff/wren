bfd = cpp.find_library('bfd', required: false)
dw = cpp.find_library('dw', required: false)
unwind = dependency('libunwind', required: false)
dwarf = dependency('libdwarf', required: false)

deps = [bfd, dw, dwarf, unwind]

if bfd.found()
    add_project_arguments('-DBACKWARD_HAS_BFD=1', language: 'cpp')
    if meson.version().version_compare('>= 0.62')
        deps += dependency('dl')
    else
        deps += cpp.find_library('dl')
    endif
endif

if dw.found()
    add_project_arguments('-DBACKWARD_HAS_DW=1', language: 'cpp')
endif

if dwarf.found()
    # add_project_arguments('-DBACKWARD_HAS_DWARF=1', language: 'cpp')
endif

backward = static_library('backward', 'backward.cpp', dependencies: deps)

backward_dep = declare_dependency(link_with: backward)

project(
        'dbtool',
        'cpp',
        license: 'GPL',
        version: '1.9.1',
        meson_version: '>=1.3',
        default_options: [
                'warning_level=2',
                'werror=true',
        ],
)

add_project_arguments(
        [
                '-Wno-unused-parameter',
                '-Wno-unused-result',
                '-Wno-missing-braces',
                '-Wno-format-zero-length',
                '-Wvla',
                '-Wno-sign-compare',
                '-Wno-narrowing'
        ],
        language: 'cpp',
)


cpp = meson.get_compiler('cpp')
conf = configuration_data()
dbtool_inc = include_directories('.')

if host_machine.system().startswith('freebsd')
  dbtool_inc = include_directories('.', '/usr/local/include')
  add_project_link_arguments('LDFLAGS=/usr/local/lib')
endif


# check for funcs.
foreach func : ['getopt', 'fdopen', 'fgetc', 'getenv', 'getpass']
    conf.set('HAVE_'+func.to_upper(), cpp.has_function(func, prefix : '#include <unistd.h>\n#include <stdio.h>\n#include <stdlib.h>'))
endforeach



# check for libraries with CMAKE or pkg-config
pcre2 = dependency('libpcre2-8')


# manually check for libraries
lib_berkeley = cpp.find_library('db_cxx', required: false,
               dirs : ['/usr', '/usr/local'])
inc_berkeley = include_directories('/usr', '/usr/local')

lib_gdbm = cpp.find_library('gdbm', required: false,
               dirs : ['/usr', '/usr/local'])
inc_gdbm = include_directories('/usr', '/usr/local')


if not lib_gdbm.found() and not lib_berkeley.found()
  error('neither GDBM nor BerkeleyDB are installed')
endif

# check commandline options
prefix = get_option('prefix')

if get_option('buildtype') == 'debug'
    conf.set('DEBUG', '1')
endif



# setup conf map
version = '@0@'.format(meson.project_version())

conf.set('HAVE_LIBPCRE', pcre2.found())
conf.set('HAVE_BERKELEY', lib_berkeley.found())
conf.set('HAVE_GDBM', lib_gdbm.found())
conf.set('prefix', prefix)
conf.set('VERSION', version)


# write out the config header
m = configure_file(
  input : 'platform.h.in',
  output : 'platform.h',
  configuration : conf,
)


# code
dbtool_sources = files(
  'cipher.cc',
  'config.cc',
  'dbtool.cc',
  'digest.cc',
  'engine.cc',
  'rijndael.cc'
)

# add dependencies, manual libs are added directly below
dbtool_deps = [
  pcre2
]


executable(
        'dbtool',
        [dbtool_sources],
        include_directories: [dbtool_inc, inc_berkeley, inc_gdbm],
        dependencies: [dbtool_deps, lib_berkeley, lib_gdbm],
        install: true
)

# build manual page
pod2man = find_program('pod2man', native: true)
if pod2man.found()
  res = run_command(pod2man.full_path(), 'dbtool.pod', 'dbtool.1', check:true)
  if res.returncode() == 0
    install_man('dbtool.1')
  endif
endif

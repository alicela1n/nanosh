project('nanosh', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

readline = dependency('readline', required : true)

executable('nanosh',
           'src/sh.c',
           'src/str.c',
           dependencies : readline,
           install : true)

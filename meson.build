project('sen', 'c')

gtkdep = dependency('gtk4')

gnome=import('gnome')
resources = gnome.compile_resources('resources','sen.gresource.xml')

sourcefiles=files('senapplication.c', 'sennotebook.c', 'sentextview.c')

executable('sen', sourcefiles, resources, dependencies: gtkdep, export_dynamic: true)

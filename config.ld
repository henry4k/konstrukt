-- vim: set filetype=lua:
project = 'Konstrukt'
title = 'Reference'
description = 'Scripting API'
format = 'markdown'
topics = { 'README.md',
           'documentation/Packages.md',
           'documentation/Coordinates.md',
           'documentation/Static World.md' }
package = 'core'
file = 'core'
not_luadoc = true
no_space_before_args = true
sort_modules = true
backtick_references = false

new_type('topic', 'Topics', true)
new_type('script', 'Scripts', true, 'topic')
new_type('module', 'Modules', true, 'script')
new_type('classmod', 'Classes', true, 'module')
new_type('mixin', 'Mixins', true, 'classmod')

new_type('event', 'Events', false)
new_type('control', 'Mapped controls', false)
new_type('configkey', 'Config keys', false)

alias('internal', 'local')

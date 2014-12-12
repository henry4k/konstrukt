-- vim: set filetype=lua:
project = 'Apoapsis'
title = 'Reference'
description = 'Scripting API'
format = 'markdown'
topics = { 'README.md', 'documentation/Packages.md' }
package = 'core'
file = 'core'
not_luadoc = true
no_space_before_args = true
backtick_references = true

new_type('topic', 'Topics', true)
new_type('script', 'Scripts', true, 'topic')
new_type('module', 'Modules', true, 'script')

new_type('type', 'Classes', false)
new_type('mixin', 'Mixins', false, 'type')

custom_tags = {
    { 'callback', hidden = true },
    { 'extends' },
    { 'includes' }
}

custom_display_name_handler = function( item, default_handler )
    local display_name = default_handler(item)
    if item.tags.callback then
        display_name = display_name .. ' [callback]'
    end
    return display_name
end

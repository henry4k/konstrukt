set makeprg=tup

let &path = &path . "," . expand("<sfile>:p:h")
let &path = &path . "," . expand("<sfile>:p:h") . "/engine"
let &path = &path . "," . expand("<sfile>:p:h") . "/third-party"

let g:proveargs = expand("<sfile>:p:h")

let b:syntastic_lua_luacheck_args = '--config ' . expand('<sfile>:p:h') . '/.luacheckrc'

vnoremap <LocalLeader>r :s_\v^([/a-z]+/)?([a-z]+)$_local \2 = require '\0'_<CR>

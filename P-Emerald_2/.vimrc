map =G :%py3f /usr/share/clang/clang-format.py<cr>
map <C-K> :%py3f /usr/share/clang/clang-format.py<cr>
imap <C-K> <Esc> :%py3f /usr/share/clang/clang-format.py<cr> i
au BufWritePre * :%py3f /usr/share/clang/clang-format.py

set tags=tags;/
let &path.="arm7/include, arm7/source, arm9/include, arm9/source, /opt/devkitpro/libnds/include/,"
nnoremap <F6> :make!<cr>:!./.generate_ctags.sh<cr>
nnoremap <F5> :make!<cr>:!./.generate_ctags.sh<cr>:!desmume P-Emerald_2.nds<cr>
nnoremap <F4> :!desmume P-Emerald_2.nds<cr>
nnoremap <F7> :!./.generate_ctags.sh<cr>

let g:ycm_collect_identifiers_from_tags_files = 1
let g:ycm_confirm_extra_conf = 0
let g:ycm_max_diagnostics_to_display = 1000
let g:ycm_allow_changing_updatetime = 0

"autocmd vimenter * NERDTree
autocmd bufenter * if (winnr("$") == 1 && exists("b:NERDTree") && b:NERDTree.isTabTree()) | q | endif

set fdm=syntax

:let g:easytags_cmd = ""

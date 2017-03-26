map =G :pyf /usr/share/clang/clang-format.py<cr>
imap <C-K> <c-o> :pyf /usr/share/clang/clang-format.py<cr>
au BufWrite * :pyf /usr/share/clang/clang-format.py
au BufWrite * :UpdateTags! --fields=+l %<
let &path.="arm7/include, arm7/source, arm9/include, arm9/source, /opt/devkitpro/libnds/include/,"
nnoremap <F6> :make!<cr>
nnoremap <F5> :make!<cr>:!desmume P-Emerald_2.nds<cr>
nnoremap <F4> :!desmume P-Emerald_2.nds<cr>

let g:ycm_collect_identifiers_from_tags_files = 1
let g:ycm_confirm_extra_conf = 0
let g:ycm_max_diagnostics_to_display = 1000

autocmd vimenter * NERDTree
autocmd bufenter * if (winnr("$") == 1 && exists("b:NERDTree") && b:NERDTree.isTabTree()) | q | endif


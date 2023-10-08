let prj_base = expand('%:p:h')
let build = './bin/picker'
let run_build  = "!" . build . " -nop"

nmap \m :wa \| :make! picker \| execute run_build<CR>
nmap \d :wa \| :make! picker_debug

" https://stackoverflow.com/questions/37552913/vim-how-to-keep-folds-on-save
" Save folds between vim sessions
augroup remember_folds
	autocmd!
	autocmd BufWinLeave * mkview
	autocmd BufWinEnter * silent! loadview
augroup END

" start up a faux-IDE view
edit "source/main.c"
split
resize 10
term
NERDTreeToggle

let $src=prj_base.'/source'
let $bin=prj_base.'/bin'

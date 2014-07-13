if match($TERM, "screen")!=-1
        set term=xterm
        " Fix keycodes
"       map ^[[1~ <Home>
"       map ^[[4~ <End>
"       imap ^[[1~ <Home>
"       imap ^[[4~ <End>
endif

syntax on
filetype plugin on

colorscheme evening

set background=dark
set showmatch
set comments=sl:/*,mb:*,elx:*/
set ruler
set incsearch
set scrolloff=3

set sw=4
set ts=4
set sts=4

set nu
highlight LineNr ctermfg=DarkCyan ctermbg=darkblue

set ai
set nocompatible
set backspace=start,indent,eol

set encoding=utf-8
setglobal fileencoding=utf-8
if has("win32") || has("win16")
  set fileencodings=ucs-bom,utf-8,cp949,default,latin1
  language messages ko_kr.utf-8
  set langmenu=ko_kr.utf-8
  source $VIMRUNTIME/delmenu.vim
  source $VIMRUNTIME/menu.vim
else
  set fileencodings=ucs-bom,utf-8
endif

set hi=1000

set ru

set noet

set magic

set sol
set scs

set smartindent

set hlsearch

set mouse=a

map <up> gk
map <down> gj
imap <up> <C-o>gk
imap <down> <C-o>gj

""""""""""""""""""""""""""""""""""""""""""""""""""
" FFmpeg code rule
" indentation rules for FFmpeg: 4 spaces, no tabs
"set expandtab
"set shiftwidth=4
"set softtabstop=4
"set cindent
"set cinoptions=(0
"" allow tabs in Makefiles
"autocmd FileType make set noexpandtab shiftwidth=8 softtabstop=8
" Trailing whitespace and tabs are forbidden, so highlight them.
highlight ForbiddenWhitespace ctermbg=red guibg=red
match ForbiddenWhitespace /\s\+$/
" Do not highlight spaces at the end of line while typing on that line.
autocmd InsertEnter * match ForbiddenWhitespace /\t\|\s\+\%#\@<!$/

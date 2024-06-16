# Lambda Calculus Toy Lang

Toy lambda calculus interpreter.

To test it out, you can simply write a lambda expression such as:
`(@x.x a)` and run `make` for compilation and `make run` or `./lc` to run the executable.

Note that it will parse your Lambda Calculus expression and then reduce it.

Another useful thing you can do is have *definitions*. 
Definitions are simply more lambda expressions that you bind to a name.
Everytime you write the name bound to the definition, before the reduction, the evaluator will expand the definition into its original expanded form. For example, assume a file `expr.lambda`:

```
def a = @x.x 

def b = @y.y

(a b)
```

It will reduce further to:

`(@x.x @y.y)` => `(@y.y)`

This is a WIP and there are a lot of TODOs to enhance parsing, error handling, usability and reduction steps.
Some of the TODOs are listed in the end of this file.

## Syntax Highlighting

It is boring to write lambda expressions in plain white characters.

If you want to highlight your `.lambda` files in `vim`, you can:

- Copy `vim-lambda` dir to `~/.vim/bundle/` with `cp ./vim-lambda ~/.vim/bundle/ -r`
- Copy syntax and ftdetect individually to `.vim/syntax` and `.vim/ftdetect`

Or use your plugin manage of preference, the two files are really simple.


## TODOs
These are not in order of priority.

- Syntax Highlighting for vscode and tutorial for neovim
- Completions for common defs (terms, lists, tuples etc)
- Implement Applicative and normal order (currently just one) option
- Allow step-by-step reduction through cli to work out the entire reduction (not possible today)
- Fix name clashes to work with strings as well (and generate x_1)
- Fix name permissions to allow _ character too
- Maybe an import system?


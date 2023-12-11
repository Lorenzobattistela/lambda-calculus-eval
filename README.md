# Lambda Calculus Compiler

This is a WIP and a learning project.

This repository aims to pick up lambda expressions (using @ as the lambda sign) such as:
`@x.x` , `@x.(x) z` , `@x.(@z.(z x)) n m` 

Read, scan, parse, spot syntax errors and finally reduce these expressions.

I chose to write from scratch, and picked C to the task.

For now, we have the parsing step (that does not support sugaring, so we need parenthesis everywhere) and the reducing step.
We can parse and reduce, for example, the following expression:
`(@x.(@z.(@n.(n z x) a) b) c)` -> `a b c`

Note that the responses are still in a AST pretty printing way, and not the lambda text yet.
You can test things out by compiling and running `make run` or running the `./out` executable.

Feel free to change stuff and contribute!

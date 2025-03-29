# 💻 Custom Lisp Implementation (To be named) 💻

Each program in this dialect of Lisp starts from the `Main` function. The `Main` function essentially encapsulates the entire program, working as a sort of 'root node' for the interpreter.


## Examples
### Hello, World!
```lisp
(define Main () (
    (print "Hello, World!\n")
))
```

### Factorial
```lisp
(define Main() (
    (define Factorial (x) (
        (if (= x 0) (1) (
            (* (x) (Factorial (- x 1)))
        ))
    ))
))
```

### Multiple Statements
```lisp
(define Main () (
    (group
        (var x 0)
        (= x (+ x 1))
        (if (= x 1) (
            (print "x = 1")
        ) (0))
    )
))
```
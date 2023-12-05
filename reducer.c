#include "reducer.h"

// Alpha conversion -> if we have two lambda expressions with same variable name, change one of
// them to create a new variable: (@x.xx)(@x.x) -> (@x.xx)(@y.y) or -> (@x.xx)(@x'.x')

// Beta reduction: substitution -> we have to look for scope (@x.xy)z => zy

// Eta Conversion / reduction -> Converts between @x.(f x) and f whenever x does not appear free in f 
// which means @x.(f x) = f if f does not make use of x
// @x.(@y.yy)x) is equivalent to (@y.yy) because f does not make use of x.



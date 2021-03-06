# lisp

Интерпретатор для lisp-подобного языка программирования в режиме REPL. 

Выполнение языка происходит в 3 этапа:

**Токенизация** - преобразует текст программы в последовательность
   атомарных лексем.
     
**Синтаксический анализ** - преобразует последовательность токенов
   в AST (в виде списков)

**Вычисление** - рекурсивно обходит AST программы и преобразует его
   в соответствии с набором правил.
     
   - `2 => 2` - числа преобразуются сами в себя.
   - `a => 4` - символы преобразуются в значение переменной.
   - `(+ a 3) => (#<plus-function> 2 3) => #<plus-function>(2, 3) => 5` - списки вычисляют в 2 этапа.
    1. Все элементы списка вычисляются рекурсивно.
    2. Значение первого элемента интерпретируется как функция и
       применяется к оставщимся элементам списка.
   - `(set! x 1)` - Если первым элементом списка является специальная
     форма, то вычисление происходит по специальным правилам
     вычисления этой формы. Например форма `set!` принимает `x` первым
     аргументом просто как имя. По правилам вычисления функций `x`
     должен был бы быть преобразован в значение переменной `x`

## Списки и пары

Единственный композитный тип - это пара. Записывается как 
`'(1 . 2)`. Списки строятся из пар.

`'(1 2 3)` то же самое, что и `'(1 . (2 . (3 . ())))`.

Пустой список `'()` - это синоним `nullptr`.

## Обработка ошибок

* Интерпретатор различает 3 вида ошибок:

  1. Ошибки синтаксиса. Возникают когда программа не соответствует
     формальному синтаксису языка. Или когда программа неправильно
     использует особые формы.

  2. Ошибки обращения к неопределённым переменным.

  3. Ошибки времени исполнения. К этим ошибкам относятся все остальные
     ошибки которые могу возникнуть во время выполнения
     программы. Например: неправильное количество аргументов передано в
     функцию, неправильный тип аргумента.
    

* В логическом контексте ложным является только значение `#f`, все
остальные значения (в том числе `0` и пустой список `'()`) являются
истинными.

* Идентификаторы case-sensetive.

## Список лексем

1. `(` - открывающаяся скобка.
2. `)` - закрывающаяся скобка.
3. `3`, `-6` - число.
4. `'` - одинарная кавычка. Используется как сокращенная запись для
   особой формы `quote`.
5. `.` - точка. Используется для записи пары `(1 . 2)` и списка `(1 2 . 3)`.
6. `foo-bar` - представляет имя переменной в программе. Имя не может
   начинаться с `+` или `-`, за исключением особых случаев `+` и
   `-`. *`+1` - это число, а `+` - это идентификатор `+`*

## Список особых форм

### `if`

Возможны 2 формы записи.

* `(if condition true-branch)`
* `(if condition true-branch false-branch)`
    
Сначала вычисляет `condition` и проверяет значение на истинность
(с.м. определение истинности). Затем вычисляет либо `true-branch`,
либо `false-branch` и возвращает как результат всего `if`-а.

### `quote`

Возможны 2 формы записи, полная и короткая.

* `(quote (1 2 3))`
* `'(1 2 3)`
  
Возвращает единственный аргумент AST, не вычисляя его.

### `eval`

* `(eval '(max 1 2 3 4 5))`

Вычисляет единственный аргумент AST.

### Лямбда выражение.
 
* `(lambda (x) (+ 1 x))`
* `(lambda (x y) (* y x))`
* `(lambda () 1)`
  
Создаёт новую функцию. Сначала перечисляется список аргументов
функции, затем её тело. Тело может состоять из нескольких выражений,
в этом случае они вычисляются по порядку а результат последнего
выражения становится результатом функции.

### `and`, `or` - логические выражения с _short-circuit evaluation_.

* `(and)`, `(and (= 2 2) (> 2 1))`
* `(or #f)`, `(or #f (launch-nukes))`
  
Вычисляют аргументы по
порядку, останавливаясь когда значение всего выражения уже не может поменяться.

### `define`

Объявляет новую переменную или перезаписывает переменную новым значением.

* `(define x '(1 2))` - создаёт переменную `x` со значением `(1 2)`
* `(define (inc x) (+ x 1))` - создаёт новую функцию `inc`.

Запись `(define (fn-name <args>) <body>)` эквивалентна
`(define fn-name (lambda (<args>) <body>))`

### `set!`

`set!` изменяет значение уже существующей переменной. Если переменной
небыло, `set!` завершается ошибкой.

* `(set! x 1)`

## Список встроенных функций

### Предикаты

1. `null?`
2. `pair?`
3. `number?`
4. `boolean?`
5. `symbol?`
6. `list?`
7. `eq?`, `equal?`
8. `integer-equal?`

### Логические функции

1. `not`

### Функции для работы с целыми числами

1. `+`, `-`, `*`, `/`
2. `=`, `>`, `<`, `>=`, `<=`
3. `min`, `max`, `abs`
 
### Функции для работы со списками

1. `cons`
2. `car`, `cdr`
3. `set-car!`, `set-cdr!`
4. `list`
5. `list-ref`, `list-tail`

## Встроенные переменные

1. `#t`, `#f`.



##TODO 
 * Реализовать mark-and-sweep GC, освобождающий недостижимые
   циклы объектов.


a = 0

do
    a += 1
    if a < 10
        continue
    else
        break
    end
while True
println(a == 10)

a = 0

do
    a += 1
    if a < 10
        redo
    end
while False
println(a == 10)

a = 0

do
    a += 1
while False
println(a == 1)

a = 0
c = 0
do
    b = 0
    do
        if (a + b) % 2 == 0
            c += 1
            break
        end
        b += 1
    while b < 10
    a += 1
while a < 10
println(a == 10 and c == 10)
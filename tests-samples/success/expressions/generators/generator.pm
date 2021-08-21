my_generator = (a for a in "hello")
println(my_generator.Next() == "h")
println(my_generator.HasNext())
index = 0
reference = "Hello"
my_generator = (char for char in ((char2 * 10) for char2 in "Hello"))
for a in my_generator
    println(a == reference[index] * 10)
    index += 1
end

println(index == 5)

reference = [(2, 1), (4, 3)]
index = 0
for a in ((y, x) for x, y in  [(1, 2), (3, 4)])
    println(a == reference[index])
    index += 1
end

println(index == 2)
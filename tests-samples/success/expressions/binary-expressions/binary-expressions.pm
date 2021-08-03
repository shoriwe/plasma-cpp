a = 10
println(a + 10 == 20)
a.name = "a"
println(a.name + "nthony" == "anthony")
a.date = [2020, 12, 12]
a.date[2] = 11
println(a.date[2] == 11)
println(a.date != [2020, 12, 12])
println((1 + 2 / 3) == 1.66666666666666651864)
println(25 ** (1 / 2) == 5)
println("Hello " * 5 == "Hello Hello Hello Hello Hello ")
println((1, 2, 4 + 5 / 6 ** 2, 10, "hello * 5 " * 0) == (1, 2, 4 + 5 / 6 ** 2, 10, ""))
println(1 and 1)
println(1 or 1)
println(1 xor 0)
println(1 and (1, 2, 3, 4))
println(1 or (1, 2, 3, 4))
println(1 xor (1, 2, 3, 4) == False)
println(1 // 2 == 0)
println((1, 2, 3, "Hello") * 2 == (1, 2, 3, "Hello", 1, 2, 3, "Hello"))
println(((1 + 2) / 3) == 1)
println(1 in (1, 2, 3, 4))
println(1 in [1, 2, 3, 4, 5])
println(1 in (1, 2, 3, 4, 5))
println(1 in {1: 2, 2: 3, 3: 4, 4: 5, 5: 6})
println(1 / 2 == 0.5)
println(1 / 2 ** 2 + 5 * 1 - 3 == 2.25)
println(1 / 2 == 0.5)
class A
    def Equals(other)
        return self.Class() == other.Class()
    end

    def Copy()
        return self
    end
end
println(A() == A())
println((A(), A(), A(), A()) * 2 == (A(), A(), A(), A(), A(), A(), A(), A()))
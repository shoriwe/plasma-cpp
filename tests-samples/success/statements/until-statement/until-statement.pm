# Test Redo

a = 0
until False
    a += 1
    if a < 100
        redo
    else
        break
    end
end

println(a == 100)

# Test Break and Continue
a = 0
until False
    if a == 100
        break
    else
        a += 1
        continue
    end
end
println(a == 100)
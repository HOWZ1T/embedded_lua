for n = 0, 20 do
	str_n = tostring(n)
	str_f = tostring(c_fib(n))
	print("fib(" .. str_n .. "): " .. str_f)
end

require()
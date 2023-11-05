//&T-
// function definition
dot(x1, y1, x2, y2: integer): integer
begin
	var result: integer;
	result := sum(product(x1, y1), product(x2, y2));
	return result;
end
end

//&S-
//&T-
//&D-

realtest2;

var gv : real;
var gc : 4.0;

sum(a, b: real): real
begin
	var result: real;
	result := a + b;
	return result;
end
end

begin

var lv : real;
var lc : 2.2;
lv := 1.1;
print sum(lv, lc);

if( lv > lc ) then
begin
    print lv;
end
else
begin
    print lc;
end
end if

gv := 2.0;
print (gv / gc);

end
end

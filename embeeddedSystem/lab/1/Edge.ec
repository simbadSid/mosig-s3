node Edge	(b: bool)	
returns		(endReception, x0, x1, x2, x3, p: bool);
var t0, t1, t2, t3 : bool;
let
  t0		= true  -> pre endReception;
  t1		= false -> pre t0;
  t2		= false -> pre t1;
  t3		= false -> pre t2;
  endReception	= false -> pre t3;

  x0 = if (t0) then b else 0;
  x1 = if (t1) then b else 0;
  x2 = if (t2) then b else 0;
  x3 = if (t3) then b else 0;
  p  = if (endReception) then b else 0;

	
tel





import sys
import random

if len(sys.argv) != 4:
	sys.stderr.write('Usage: <expression> <delim> <rows>\n');
	sys.stderr.write('Example: 2*x+0.5*x+x ; 10\n');
	sys.exit(1);

expr = sys.argv[1]
delim = sys.argv[2]
rows = int(sys.argv[3])
vars = expr.count('x')

for k in range(0, rows):
	ready_expr = expr;
	for i in range(0, vars):
		val = round(random.random()*100.0, 3)
		ready_expr = ready_expr.replace('x', str(val), 1);
		sys.stdout.write(str(val))
		sys.stdout.write(delim)
	sys.stdout.write(str(eval(ready_expr)))
	sys.stdout.write('\n')
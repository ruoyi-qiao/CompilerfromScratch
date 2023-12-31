from graphviz import Digraph
import sys


## state 
'''
0:
	<program'> ::= * <program> , eof
	<program> ::= * <compoundstmt> , eof
	<compoundstmt> ::= * "{" <stmts> "}" , eof

1:
	<program'> ::= <program> *, eof

2:
	<program> ::= <compoundstmt> *, eof

'''

## transition
'''
(0, '<program>') : 1
(0, '<compoundstmt>') : 2
(0, '"{"') : 3
(3, '<stmt>') : 4
(3, '""') : 14
(4, '<compoundstmt>') : 5
'''




state = dict()
transition = []

current_state, current_rule = None, None

for line in sys.stdin:
    if line[0] == '(':
        transition.append(line)
    elif line[0] == '\n': # end of a state
        state[current_state] = current_rule
        current_state, current_rule = None, None
    elif line[0].isdigit(): # start of a state
        current_state = int(line.split(':')[0])
        current_rule = []
    else: # rule
        current_rule.append(line)

    
# label the dot with state number
        
dot = Digraph(comment='LR(1) Automaton', engine='dot', strict=True)


for k, v in state.items():
    label = '#{}:\n'.format(k)
    reduce = False
    
    for rule in v:
        label += rule

        starIndex = rule.index('*')
        if starIndex < len(rule) - 1 and rule[starIndex + 1] == ',':
            reduce = True
            
    dot.node(str(k), label=label, color='blue' if reduce else 'black')



for t in transition:
    s, r = t.split(' : ')
    s = s[1:-1].split(', ')
    print(s[0], s[1], r)
    dot.edge(s[0], r, label=s[1])


dot.format = 'png'
dot.render('figure/LR1_automaton') 
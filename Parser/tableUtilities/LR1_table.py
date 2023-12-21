from itertools import groupby
from LR0_items import *
from bnf import *
from sets import *
from LR1_items import *

def CStyleTable(tableName: str, table: dict, mapping : dict, isAction: bool) -> str:
    howToMap = [terminal for terminal, _ in groupby(sorted(table.keys(), key=lambda x: x[1]), lambda x: x[1])]

    for i in range(len(howToMap)):
        mapping[howToMap[i]] = i
    
    maxState = max((state for state, _ in table.keys())) + 1

    output = [[-1 for i in range(len(howToMap))] for j in range(maxState)]
    if isAction:
        for (key, value) in table.items():
            if isinstance(value, int):
                output[key[0]][howToMap.index(key[1])] = value
            elif value == 'Accept!':
                output[key[0]][howToMap.index(key[1])] = 0
            else:
                output[key[0]][howToMap.index(key[1])] = value.Index | (1 << 8)
    else:
        for (key, value) in table.items():
            output[key[0]][howToMap.index(key[1])] = value

    for i in range(len(output)):
        # output[i] = ''.join('%s, ' % (s) for s in output[i])
        output[i] = ''.join(f'{s:>4},' for s in output[i])
        output[i] = '\t{ %s},\n' % output[i]
    output = ''.join(output)

    output = '// {}\nstatic const int {}[{}][{}] = {{\n{}}};'.format(
        'how do terminals map to indices: %s' % ''.join(('%s -> %d; ' % (howToMap[i], i) for i in range(len(howToMap)))),\
        tableName, maxState, len(howToMap), output
    )
    return output


def CppStyleMap(mapping : dict, keyType : str, valueType : str, name : str):
    output = []
    if keyType == 'int':
        output.append('std::map<%s, %s> %s = {\n' % (keyType, valueType, name))
        for key, value in mapping.items():
            output.append('\t{%s, \"%s\"},\n' % (key, value))
        output.append('};')
    else:
        output.append('std::map<%s, %s> %s = {\n' % (keyType, valueType, name))
        for key, value in mapping.items():
            output.append('\t{%s, %s},\n' % (key, value))
        output.append('};') 

    return ''.join(output)

def CppStyleVector(vector : list, valueType : str, name : str):
    output = []
    output.append('std::vector<%s> %s = {\n' % (valueType, name))
    for value in vector:
        output.append('\t%s,\n' % value)
    output.append('};')

    return ''.join(output)

if __name__ == '__main__':
    nts = ParseBNF('../LR1/rules.txt').Build()
    augmentedSymbol = '<program\'>'

    init = LRState()
    init += Item1('<program\'>', ['<program>',], 0, 'eof')
    C, transition = GetStates(init, nts, '<program>', closureLR1)

    ########################################
    for state in C:
        print(state)
    for (key, value) in transition.items():
        print(key, ':', value, sep=' ')
    ########################################
        
    actionTable = dict()
    gotoTable = dict()

    for state in C:
        name = state.name
        for item in state:
            if item.name == augmentedSymbol and (not item.Where()):
                actionTable[(name, 'eof')] = 'Accept!'
            else:
                next2Dot = item.Where()
                if not next2Dot or next2Dot == '""':
                    actionTable[(name, item.lookahead)] = item # an item to reduce
                    # print(item.Index, item.name, item._rule, item._pos, item.lookahead)
                elif next2Dot and next2Dot[0] != '<':
                    to = transition[(name, next2Dot)]
                    actionTable[(name, next2Dot)] = to # shift to some state
    
    for state in C:
        for symbolName in nts.GetName():
            key = (state.name, symbolName)
            if key in transition.keys():  
                gotoTable[key] = transition[key]

    print('ACTION TABLE:')
    for key, value in actionTable.items():
        print(key, ': ', sep=' ', end = '')
        if isinstance(value, Item1):
            print('reduce ' + str(value))
        elif value == 'Accept!':
            print(value)
        else:
            print("shift ", value, sep = ' ')
            
    # print('----------------------------------')
    # print('GOTO TABLE')
    # for key, value in gotoTable.items():
    #     print(key, ': ', value, sep=' ')


    # print()

    # print(CStyleTable('LR1ActionTable', actionTable, True))
    # print(CStyleTable('LR1GotoTable', gotoTable, False))
    
    nonTerminal2Index = dict()
    Index2NonTerminal = dict()
    terminal2Index = dict()
    Index2NonTerminal = dict()

    print(CStyleTable('LR1ActionTable', actionTable, terminal2Index, True))
    print(CStyleTable('LR1GotoTable', gotoTable, nonTerminal2Index, False))

    Index2NonTerminal = {v: k for k, v in nonTerminal2Index.items()}
    Index2Terminal = {v: k for k, v in terminal2Index.items()}

    nonTerminal2Index = {'\"' +  k.strip('<>') + '\"': v for k, v in nonTerminal2Index.items()}

    print(CppStyleMap(Index2NonTerminal, 'int', 'std::string', 'Index2NonTerminal'))
    print(CppStyleMap(Index2Terminal, 'int', 'std::string', 'Index2Terminal'))
    print(CppStyleMap(nonTerminal2Index, 'std::string', 'int', 'NonTerminal2Index'))
    print(CppStyleMap(terminal2Index, 'std::string', 'int', 'Terminal2Index'))

    prodcutionCount = 0
    productionLength = list()
    prodcutionName = list()


    for nonTerminal in nts.GetName():
        for rule in nts[nonTerminal]:
            if len(rule) == 1 and rule[0] == '""':
                productionLength.append(0)
            else:
                productionLength.append(len(rule))
            # remove <> and wrap with ""
            prodcutionName.append('\"' + nonTerminal.strip('<>') + '\"')
    
    prodcutionCount = len(productionLength)

    # print(productionLength)
    # print(prodcutionName)

    print(CppStyleVector(productionLength, 'int', 'productionLength'))
    print(CppStyleVector(prodcutionName, 'std::string', 'productionName'))
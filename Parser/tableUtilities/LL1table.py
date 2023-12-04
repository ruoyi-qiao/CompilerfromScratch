from bnf import *
from leftrec import *
from sets import *


if __name__ == '__main__':
    nts = ParseBNF('rules.txt').Build()
    
    # comment this line to keep the original syntax
    # nts = EliminateLeftRecursion(nts) 

    terminalName = nts.GetTermName()
    terminalName.append('eof')
    print(terminalName)
    first = FirstSet(nts, terminalName)
    follow = FollowSet(nts, first, '<program>')

    print('First Set:')
    for (name, rules) in nts:
        print('%s: %s' % (name, first[name]))
    print('Follow Set:')
    for (name, rules) in nts:
        print('%s: %s' % (name, follow[name]))


    ntsNumber = len(nts.GetName())
    terminalNumber = len(terminalName)
    
    symbol2index = dict()
    for i in range(len(nts.GetName())):
        symbol2index[nts.GetName()[i]] = i
    for i in range(len(terminalName)):
        symbol2index[terminalName[i]] = i + len(nts.GetName())
        
    index2symbol = dict()
    for (k, v) in symbol2index.items():
        index2symbol[v] = k
    
    LL1Table = [[None for i in range(len(terminalName))] for j in range(len(nts.GetName()))]
    
    # make rule hashable
    productionCount = 0
    productionMap = dict()
    maxRuleLength = 0
    for (name, rules) in nts:
        for rule in rules:
            productionMap[str(rule)] = rule
            maxRuleLength = max(maxRuleLength, len(rule))

    rulesTableSize = productionMap.__len__()
    rulesTable = [None for _ in range(rulesTableSize)]
    
    # fill rulesTable
    rulesIndex = dict()
    for i, (strRule, rule) in enumerate(productionMap.items()):
        indexies = []
        for ele in rule:
           indexies.append(symbol2index[ele])
        while len(indexies) < maxRuleLength:
            indexies.append(-1) 
        rulesIndex[strRule] = i
        rulesTable[i] = indexies
    
    
    for (name, rules) in nts:
        for rule in rules:
            for t in terminalName:
                if t in first[rule[0]]:
                    print('[%s][%s]: %s -> %s' % (name, t, name, rule))
            if '""' in first[rule[0]]:
                for t in follow[name]:
                    print('[%s][%s]: %s -> %s' % (name, t, name, rule))
    
    for (name, rules) in nts:
        for rule in rules:
            for t in terminalName:
                if t in first[rule[0]]:
                    LL1Table[symbol2index[name]][symbol2index[t] - ntsNumber] = productionMap[str(rule)]
            if '""' in first[rule[0]]:
                for t in follow[name]:
                    LL1Table[symbol2index[name]][symbol2index[t] - ntsNumber] = productionMap[str(rule)]
    
    # print c-style table of rulesTable
    print('int rulesTable[%d][%d] = {' % (rulesTableSize, maxRuleLength))
    for i in range(rulesTableSize):
        print('\t{', end='')
        for j in range(maxRuleLength):
            print('%d, ' % rulesTable[i][j], end='')
        print('},')
    print('};')
    
    # print c-style table of LL1Table
    # print sync when there is no rule and the terminal is in the follow set
    print('int LL1Table[%d][%d] = {' % (len(nts.GetName()), len(terminalName)))
    for i in range(len(nts.GetName())):
        print('\t{', end='')
        for j in range(len(terminalName)):
            if LL1Table[i][j] is None:
                print('-1, ', end='')
            else:
                print('%d, ' % rulesIndex[str(LL1Table[i][j])], end='')
        print('},')
    print('};')
    
    # print the index2symbol table
    for (k, v) in index2symbol.items():
        print('%d: %s' % (k, v))
    
    #print the rulesIndex table
    for (k, v) in rulesIndex.items():
        print('%d: %s' % (v, k))
    
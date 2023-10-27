#include <cassert>
#include <iostream>
#include "DFAConverter.hpp"

int main () {
    if (0) {
        // test Predicate usage
        Predicate p1 = single('a');
        Predicate p2 = single('b');
        Predicate p3 = andPredicate(p1, p2);
        Predicate p4 = orPredicate(p1, p2);
        Predicate p5 = ranged('a', 'z');
        assert(p1('a'));
        assert(!p1('b'));
        assert(!p3('a'));
        assert(!p3('b'));
        assert(p4('a'));
        assert(p4('b'));
        assert(!p4('c'));
        assert(p5('a'));
        assert(p5('z'));
        assert(!p5('A'));

        Predicate p6 = ranged('a', 'b');
        Predicate p7 = ranged('b', 'e');
        Predicate p8 = andPredicate(p6, p7);
        Predicate p9 = orPredicate(p6, p7);
        assert(p8('b'));
        assert(!p8('c'));
        assert(!p8('d'));
        assert(p9('a'));
        assert(p9('e'));
        assert(!p9('f'));

        std::cout << "=> Predicate test passed." << std::endl;
        // test NFANode usage
        NFANode n1;
        NFANode n2;
        NFANode n3;
        NFANode n4;
        NFANode n5;
        NFANode n6;
        NFANode n7;
        NFANode n8;
        NFANode n9;

        n1.addEpsilonEdge(&n2);
        n1.addEpsilonEdge(&n3);
        n2.addEpsilonEdge(&n4);

        n3.addCharEdge(p1, &n5);
        n4.addCharEdge(p2, &n6);

        n5.addCharEdge(p5, &n7);
        n6.addCharEdge(p9, &n8);
        n9.addCharEdge(p1, &n1);

        n7.addEpsilonEdge(&n9);

        // test NFANode output
        std::cout << n1 << std::endl;
        std::cout << n2 << std::endl;
        std::cout << n3 << std::endl;
        std::cout << n4 << std::endl;
        std::cout << n5 << std::endl;
        std::cout << n6 << std::endl;
        std::cout << n7 << std::endl;
        std::cout << n8 << std::endl;
        std::cout << n9 << std::endl;

        std::cout << "=> end of NFANode test." << std::endl;
        // test NFA usage
        NFA* nfa1 = new NFA();
        NFA* nfa2 = new NFA();
        NFA* nfa3 = new NFA();

        std::cout << "=> start of NFA test." << std::endl;

        nfa1->andAtom(p1);
        nfa1->andAtom(p2);
        nfa1->star();
        std::cout << "nfa1: " << std::endl;
        nfa1->andAtom(p3);
        nfa1->andAtom(p4);
        nfa1->print();
        std::cout << std::endl;

        nfa2->andAtom(p5);
        nfa2->andAtom(p6);

        nfa3->andAtom(p4);
        nfa3->plus();

        nfa1->orNFA(nfa2);
        nfa1->orNFA(nfa3);

        nfa1->plus();

        // test NFA output
        nfa1->print();
        std::cout << std::endl;

        std::cout << "=> end of NFA test." << std::endl;
    }

    if (1)
    {
        // test RegexParser
        RegexParser* parser = new RegexParser();
        std::cerr << "=> constructing RegexParser" << std::endl;
        parser->registerRegex("@d+(L|l)?|0(x|X)@h+", "interger");
        parser->registerRegex("@d+.@d+((e|E)(+|-)?@d+)?", "float");
        std::cerr << "=> Regex registered" << std::endl;
        parser->getNfa()->print();

        std::cerr << "=> constructing DFAConverter" << std::endl;
        DFAConverter* converter = new DFAConverter(parser);

        std::string token;
        int res = converter->runDFA("123", token);
        std::cout << res << " " << token << std::endl;

        res = converter->runDFA("0x123", token);
        std::cout << res << " " << token << std::endl;

        res = converter->runDFA("123L", token);
        std::cout << res << " " << token << std::endl;

        res = converter->runDFA("123l", token);
        std::cout << res << " " << token << std::endl;

        res = converter->runDFA("0x123a1g", token);
        std::cout << res << " " << token << std::endl;

        res = converter->runDFA("0xl1f23", token);
        std::cout << res << " " << token << std::endl;

        res = converter->runDFA("1.23", token);
        std::cout << res << " " << token << std::endl;

        res = converter->runDFA("1.23e+1", token);
        std::cout << res << " " << token << std::endl;

        res = converter->runDFA("1.23e-1", token);
        std::cout << res << " " << token << std::endl;

        res = converter->runDFA("1.23E1", token);
        std::cout << res << " " << token << std::endl;

        res = converter->runDFA("1.23eE-1", token);
        std::cout << res << " " << token << std::endl;

        res = converter->runDFA("1.23e-1.23", token);
        std::cout << res << " " << token << std::endl;

    }
    
    {
        // test RegexParser
        // RegexParser* parser = new RegexParser();
        // std::cerr << "=> constructing RegexParser" << std::endl;
        // parser->registerRegex("");
        // std::cerr << "=> Regex registered" << std::endl;
        // parser->getNfa()->print();
    }
    return 0;
}


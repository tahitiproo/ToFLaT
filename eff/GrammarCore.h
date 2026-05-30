#ifndef GRAMMARCORE_H
#define GRAMMARCORE_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include <QString>
#include <QStringList>
#include <QRegularExpression>

using Word = std::vector<std::string>;
using WordSet = std::set<Word>;

inline Word concatWord(const Word& a, const Word& b, int k) {
    Word res = a;
    for (const auto& s : b) {
        if (res.size() < k) {
            res.push_back(s);
        }
    }
    return res;
}

inline WordSet concatSet(const WordSet& A, const WordSet& B, int k) {
    WordSet res;
    if (A.empty() || B.empty()) return res;
    for (const auto& a : A) {
        for (const auto& b : B) {
            res.insert(concatWord(a, b, k));
        }
    }
    return res;
}

inline WordSet unionSet(const WordSet& A, const WordSet& B) {
    WordSet res = A;
    res.insert(B.begin(), B.end());
    return res;
}

struct Rule {
    std::string lhs;
    std::vector<std::string> rhs;
};

class Grammar {
public:
    std::vector<Rule> rules;
    std::set<std::string> nonTerminals;
    int k = 1;

    std::map<std::string, WordSet> first_k;
    std::map<std::string, WordSet> eff_k;

    bool isNonTerminal(const std::string& s) {
        return nonTerminals.count(s) > 0;
    }

    void parseRules(const std::string& text) {
        rules.clear();
        nonTerminals.clear();
        
        QString qtext = QString::fromStdString(text);
        QStringList lines = qtext.split("\n", Qt::SkipEmptyParts);
        for (auto line : lines) {
            QStringList parts = line.split("->");
            if (parts.size() != 2) continue;
            
            std::string lhs = parts[0].trimmed().toStdString();
            nonTerminals.insert(lhs);
        }
        
        for (auto line : lines) {
            QStringList parts = line.split("->");
            if (parts.size() != 2) continue;
            
            std::string lhs = parts[0].trimmed().toStdString();
            
            QStringList alternates = parts[1].split("|", Qt::SkipEmptyParts);
            for (auto alt : alternates) {
                Rule r;
                r.lhs = lhs;
                QStringList tokens = alt.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
                for (auto t : tokens) {
                    if (t != "eps" && t != "ε") {
                        r.rhs.push_back(t.toStdString());
                    }
                }
                rules.push_back(r);
            }
        }
    }

    void computeFirstK() {
        first_k.clear();
        for (const auto& nt : nonTerminals) {
            first_k[nt] = WordSet();
        }
        bool changed = true;
        while(changed) {
            changed = false;
            for (const auto& r : rules) {
                WordSet cur = {{}}; // set with empty word
                for (const auto& sym : r.rhs) {
                    if (isNonTerminal(sym)) {
                        cur = concatSet(cur, first_k[sym], k);
                    } else {
                        cur = concatSet(cur, {{sym}}, k);
                    }
                }
                WordSet old = first_k[r.lhs];
                first_k[r.lhs] = unionSet(old, cur);
                if (first_k[r.lhs] != old) changed = true;
            }
        }
    }

    void computeEffK() {
        eff_k.clear();
        for (const auto& nt : nonTerminals) {
            eff_k[nt] = WordSet();
        }
        bool changed = true;
        while(changed) {
            changed = false;
            for (const auto& r : rules) {
                if (r.rhs.empty()) continue; // eps produces empty contribution to EFF
                
                WordSet cur;
                if (isNonTerminal(r.rhs[0])) {
                    cur = eff_k[r.rhs[0]];
                } else {
                    cur = {{r.rhs[0]}};
                }
                
                for (size_t i = 1; i < r.rhs.size(); ++i) {
                    if (isNonTerminal(r.rhs[i])) {
                        cur = concatSet(cur, first_k[r.rhs[i]], k);
                    } else {
                        cur = concatSet(cur, {{r.rhs[i]}}, k);
                    }
                }
                
                WordSet old = eff_k[r.lhs];
                eff_k[r.lhs] = unionSet(old, cur);
                if (eff_k[r.lhs] != old) changed = true;
            }
        }
    }

    WordSet getFirstKOfSeq(const std::vector<std::string>& seq) {
        WordSet cur = {{}};
        for (const auto& sym : seq) {
            if (isNonTerminal(sym)) {
                cur = concatSet(cur, first_k[sym], k);
            } else {
                cur = concatSet(cur, {{sym}}, k);
            }
        }
        return cur;
    }

    WordSet getEffKOfSeq(const std::vector<std::string>& seq) {
        if (seq.empty()) return {{}};
        
        WordSet cur;
        if (isNonTerminal(seq[0])) {
            cur = eff_k[seq[0]];
        } else {
            cur = {{seq[0]}};
        }
        
        for (size_t i = 1; i < seq.size(); ++i) {
            if (isNonTerminal(seq[i])) {
                cur = concatSet(cur, first_k[seq[i]], k);
            } else {
                cur = concatSet(cur, {{seq[i]}}, k);
            }
        }
        return cur;
    }
};

inline std::string formatWordSet(const WordSet& ws) {
    if (ws.empty()) return "∅";
    std::string res = "{";
    int i = 0;
    for (const auto& w : ws) {
        if (i++ > 0) res += ", ";
        if (w.empty()) res += "eps";
        else {
            for (size_t j = 0; j < w.size(); ++j) {
                res += w[j];
            }
        }
    }
    res += "}";
    return res;
}

#endif // GRAMMARCORE_H
